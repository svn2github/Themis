/*	ElementParser implementation
	See ElementParser.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// SGMLParser headers
#include "ElementParser.hpp"
#include "ReadException.hpp"
#include "ElementDeclException.hpp"
#include "SGMLSupport.hpp"
#include "State.hpp"
#include "CommentDeclParser.hpp"
#include "TSchema.hpp"
#include "TElementDeclaration.hpp"

// DOM headers
#include "TElement.h"
#include "TNodeList.h"
#include "TNamedNodeMap.h"
#include "TText.h"
#include "TAttr.h"

ElementParser	::	ElementParser(SGMLTextPtr aDocText,
								  TSchemaPtr aSchema )
				:	BaseParser( aSchema )	{

	mDocText = aDocText;

	// Comment declaration parser
	commentParser = new CommentDeclParser( aDocText, mSchema );

	// Document to store the element tree
	mDocument = TDocumentPtr( new TDocument() );


}

ElementParser	::	~ElementParser()	{

	delete commentParser;
	
}

void ElementParser	::	setDocText( SGMLTextPtr aDocText )	{
	
	mDocText = aDocText;
	commentParser->setDocText( aDocText );
	
}

void ElementParser	::	parse( const string & aName )	{
	
	mName = aName;
	
	TElementDeclarationPtr declaration = getElementDecl( aName, mElements );
	processElementContent( aName, declaration, mDocument );
	
}

TDocumentPtr ElementParser	::	getDocument() const	{
	
	return mDocument;
	
}

void ElementParser	::	processElementContent( const TDOMString & aName,
																	const TElementDeclarationPtr & aDeclaration,
																	TNodePtr aParent )	{

	bool contentFound = true;
	while ( contentFound )	{
		if ( ! processCommentDeclaration() )	{
			if ( ! processS() )	{
				contentFound = false;
			}
		}
	}

	processElement( aName, aDeclaration, aParent );
																		
}

void ElementParser	::	processUnknownTags( TNodePtr aParent )	{

	bool contentFound = true;
	while ( contentFound )	{
		processSStar();
		if ( processUnknownStartTag( aParent ) )	{
			continue;
		}
		if ( processUnknownEndTag() )	{
			continue;
		}
		contentFound = false;
	}
	
}

bool ElementParser	::	processUnknownStartTag( TNodePtr aParent )	{

	State save = mDocText->saveState();

	if ( ! process( mStago, false ) )	{
		return false;
	}
	
	// Skipping the document type specification for now
	
	string name = "";
	try	{
		name = processGenIdSpec();
	}
	catch( ReadException r )	{
		mDocText->restoreState( save );
		if ( r.isFatal() )	{
			throw r;
		}
		else	{
			return false;
		}
	}
	
	try	{
		getElementDecl( name, mElements );
		mDocText->restoreState( save );
		return false;
	}
	catch( ElementDeclException e )	{
		// Element does not exist in DTD. Skip it
		printf( "WARNING: Found element that does not exist in DTD: %s.", name.c_str() );
		printf( " Check the HTML source or make sure this is the right DTD for this file\n" );
		try	{
			printf( "Processing AttrSpecList\n" );
			processAttrSpecList( aParent );
			processSStar();
			process( mTagc );
		}
		catch( ReadException r )	{
			r.setFatal();
			throw r;
		}
	}
	
	return true;

}

bool ElementParser	::	processUnknownEndTag()	{
	
	State save = mDocText->saveState();
	
	if ( ! process( mEtago, false ) )	{
		return false;
	}
	
	// Skipping the document type specification for now
	
	string name = "";
	try	{
		name = processGenIdSpec();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}

	try	{
		getElementDecl( name, mElements );
		mDocText->restoreState( save );
		return false;
	}
	catch( ElementDeclException e )	{
		// Element does not exist in DTD. Skip it
		printf( "WARNING: Found element that does not exist in DTD: %s.", name.c_str() );
		printf( " Check the HTML source or make sure this is the right DTD for this file\n" );

		processSStar();
		
		try	{
			process( mTagc );
		}
		catch( ReadException r )	{
			r.setFatal();
			throw r;
		}
		
	}
	
	return true;
	
}

void ElementParser	::	processElement( const TDOMString & aName,
														const TElementDeclarationPtr & aDeclaration,
														 TNodePtr aParent )	{
		
	printf( "Trying to find: %s\n", aName.c_str() );

	// Setup the minimization
	bool start = true;
	bool end = true;
	aDeclaration->getMinimization( start, end );

	TNodePtr tag;
	State save = mDocText->saveState();
	try	{
		TElementPtr element = processStartTag( aName, aDeclaration, aParent );
		// Cast it to a node
		tag = shared_static_cast<TNode>( element );
	}
	catch( ReadException r )	{
		printf( "Didn't find: %s\n", aName.c_str() );
		mDocText->restoreState( save );
		if ( start || r.isEndTag() )	{
			throw r;
		}
		printf( "Start not required\n" );
		// Should actually still add the element to the tree. Bit too complicated atm.
		tag = shared_static_cast<TNode>( aParent );
	}

	// Take out any unknown tags
	processUnknownTags( aParent );

	// Get the content model
	TElementPtr content = getElement( aDeclaration, "content" );
	if ( content.get() != NULL )	{
		TNodePtr childContent = content->getFirstChild();
		TElementPtr baseContent = shared_static_cast<TElement>( childContent );
		TElementPtr exceptions = getElement( content, "exceptions" );
		processContent( baseContent, exceptions, tag );
	}

	// Take out any unknown tags
	processUnknownTags( aParent );

	printf( "Done with content. Trying to close up %s\n", aName.c_str() );
	
	save = mDocText->saveState();
	processSStar();
	try	{
		processEndTag( aName );
		printf( "Closed up %s correctly\n", aName.c_str() );
	}
	catch( ReadException r )	{
		mDocText->restoreState( save );
		if ( end )	{
			printf( "Throwing exception on closing of %s\n", aName.c_str() );
			throw r;
		}
		printf( "Closed up %s implicitly\n", aName.c_str() );
	}

	// Take out any unknown tags
	processUnknownTags( aParent );
	
}

TElementPtr ElementParser	::	processStartTag( const TDOMString & aName,
																	  const TElementPtr & elementDecl,
																	  TNodePtr aParent )	{
	
	try	{
		process( mStago );
	}
	catch( ReadException r )	{
		r.setReason( NO_TAG_FOUND );
		throw r;
	}
	
	// Skipping the document type specification for now

	TElementPtr element;
	
	string name = "";
	try	{
		name = processGenIdSpec();
	}
	catch( ReadException r )	{
		throw r;
	}
	if ( name == aName )	{
		printf( "Correct element start tag found: %s\n", name.c_str() );
		element = mDocument->createElement( name );
		aParent->appendChild( element );
	}
	else	{
		string error = "Expected start tag ";
		error += aName;
		ReadException exception( mDocText->getLineNr(), mDocText->getCharNr(),
											  error, WRONG_TAG_FOUND, true );
		exception.setWrongTag( name );
		throw exception;
	}

	// Skipping some stuff for now
	try	{
		printf( "Processing AttrSpecList\n" );
		processAttrSpecList( element );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}

	processSStar();

	try	{
		process( mTagc );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
	return element;
	
}

void ElementParser	::	processEndTag( const TDOMString & aName )	{
	
	process( mEtago );
	
	// Skipping the document type specification for now
	
	string name = "";
	try	{
		name = processGenIdSpec();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}

	if ( name != aName )	{
		string error = "Expected closing tag ";
		error += aName;
		ReadException exception( mDocText->getLineNr(), mDocText->getCharNr(),
											  error, WRONG_TAG_FOUND, true );
		exception.setWrongTag( name );
		throw exception;
	}
	
	processSStar();
	
	try	{
		process( mTagc );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
}

string ElementParser	::	processGenIdSpec()	{

	// Ignoring rank stem

	return processGI();
	
}

void ElementParser	::	processAttrSpecList( TNodePtr aParent )	{

	bool attrSpecFound = true;
	while ( attrSpecFound )	{
		try	{
			attrSpecFound = processAttrSpec( aParent );
		}
		catch( ReadException r )	{
			if ( r.isFatal() )	{
				throw r;
			}
		}
	}
	
}

bool ElementParser	::	processAttrSpec( TNodePtr aParent )	{

	processSStar();

	TAttrPtr attr;
	
	bool result = false;
	
	State save = mDocText->saveState();
	try	{
		string name = processName();
		processSStar();
		process( mVi );
		processSStar();
		attr = mDocument->createAttribute( name );
		TNamedNodeMapPtr map = aParent->getAttributes();
		map->setNamedItem( attr );
		printf( "Found attr spec name: %s\n", name.c_str() );
		result = true;
	}
	catch( ReadException r )	{
		// Optional.
		mDocText->restoreState( save );
	}

	// Not entirely correct. Check later
	string value = processAttrValueSpec( false );
	if ( value != "")	{
		printf( "Found AttrValueSpec: %s\n", value.c_str() );
		if ( attr.get() != NULL )	{
			attr->setValue( value );
		}
		result = true;
	}

	return result;
	
}

void ElementParser	::	skipContent( const TElementPtr & aContent )	{

	string contentName = aContent->getNodeName();
	printf( "Trying to skip content: %s\n", contentName.c_str() );

	int minOccurs = atoi(aContent->getAttribute("minOccurs").c_str());
	printf("Minimum occurence: %i\n", minOccurs);
	
	if (minOccurs == 0)	{
		printf("Skipping %s\n", contentName.c_str());
	}
	else	{
		if ( contentName == "CDATA" || contentName == "EMPTY")	{
			printf("Skipping %s\n", contentName.c_str());
		}
		else	{
			string exceptionText = "Can't skip " + contentName;
			throw ReadException( mDocText->getLineNr(),
											mDocText->getCharNr(),
											exceptionText);
		}
	};
	
}

void ElementParser	::	processContent( const TElementPtr & aContent,
														 const TElementPtr & aExceptions,
														 TNodePtr aParent )	{
	
	// Moved to just before tag
	//processExceptions( aExceptions, aParent );

	string contentName = aContent->getNodeName();
	printf( "Processing content: %s\n", contentName.c_str() );
	
	// Using switch statement, by only looking at the first character.
	switch( contentName[ 0 ] )	{
		case '(':	{
			processBrackets( aContent, aExceptions, aParent );
			break;
		}
		case '?':	{
			processOptional( aContent, aExceptions, aParent );
			break;
		}
		case '+':	{
			processPlus( aContent, aExceptions, aParent );
			break;
		}
		case '*':	{
			processStar( aContent, aExceptions, aParent );
			break;
		}
		case '|':	{
			bool contentFound = true;
			int count = 0;
			while (contentFound)	{
				try	{
					processOr( aContent, aExceptions, aParent );
					count++;
				}
				catch( ReadException r )	{
					if ( r.isFatal() )	{
						// Something went wrong here
						throw r;
					}
					contentFound = false;
				}
			}
			break;
		}
		case '&':	{
			processAnd( aContent, aExceptions, aParent );
			break;
		}
		case ',':	{
			processComma( aContent, aExceptions, aParent );
			break;
		}
		case '#':	{
			processDataText( aContent, aExceptions, aParent );
			break;
		}
		default:	{
			processExceptions( aExceptions, aParent );
			if ( contentName == "CDATA" )	{
				printf( "At cdata\n" );
				string cdata = processCharData( mEtago, false );
				TTextPtr text =
					mDocument->createText( cdata );
				aParent->appendChild( text );
				printf( "CDATA: %s\n", cdata.c_str() );
				break;
			}
			if ( contentName == "EMPTY" )	{
				printf( "At empty\n" );
				break;
			}
			printf( "At tag\n" );
			TElementDeclarationPtr declaration = getElementDecl( contentName, mElements );
			processElementContent( contentName, declaration, aParent );
			printf( "Finished tag\n" );
		}
	}

}

void ElementParser	::	processBrackets( const TElementPtr & aContent,
														  const TElementPtr & aExceptions,
														  TNodePtr aParent )	{

	printf( "At brackets\n" );
	TNodePtr child = aContent->getFirstChild();
	TElementPtr subContent = shared_static_cast<TElement>( child );
	processContent( subContent, aExceptions, aParent );
	printf( "Finished brackets\n" );
														  	
}

void ElementParser	::	processOptional( const TElementPtr & aContent,
														 const TElementPtr & aExceptions,
														 TNodePtr aParent )	{

	printf( "At optional element\n" );
	TNodePtr child = aContent->getFirstChild();
	TElementPtr subContent = shared_static_cast<TElement>( child );
	try	{
		processContent( subContent, aExceptions, aParent );
	}
	catch( ReadException r )	{
		if ( ! r.isWrongTag() && r.isFatal() )	{
			// Something went wrong here
			throw r;
		}
		if ( r.isEndTag() )	{
			// Let them know this.
			throw r;
		}
	}
	printf( "Finished optional element\n" );
														  	
}

void ElementParser	::	processPlus( const TElementPtr & aContent,
												   const TElementPtr & aExceptions,
												   TNodePtr aParent )	{

	printf( "At plus element\n" );
	TNodePtr child = aContent->getFirstChild();
	TElementPtr subContent = shared_static_cast<TElement>( child );
	processContent( subContent, aExceptions, aParent );
	bool plusFound = true;
	while ( plusFound )	{
		try	{
			printf( "Going for another plus\n" );
			processContent( subContent, aExceptions, aParent );
		}
		catch( ReadException r )	{
			printf( "No plus found anymore\n" );
			if ( r.isFatal() )	{
				throw r;
			}
			else	{
				plusFound = false;
			}
		}
	}
	printf( "Finished plus element\n" );
													  	
}

void ElementParser	::	processStar( const TElementPtr & aContent,
												   const TElementPtr & aExceptions,
												   TNodePtr aParent )	{

	printf( "At star element\n" );
	TNodePtr child = aContent->getFirstChild();
	TElementPtr subContent = shared_static_cast<TElement>( child );
	bool starFound = true;
	while ( starFound )	{
		try	{
			printf( "Going for another star\n" );
			processContent( subContent, aExceptions, aParent );
		}
		catch( ReadException r )	{
			printf( "No star found anymore\n" );
			if ( ! r.isWrongTag() && r.isFatal() )	{
				throw r;
			}
			else	{
				starFound = false;
			}
		}
	}
	printf( "Finished star element\n" );

}

void ElementParser	::	processOr( const TElementPtr & aContent,
												 const TElementPtr & aExceptions,
												 TNodePtr aParent )	{

	printf( "At or element\n" );

	TNodeListPtr children = aContent->getChildNodes();
	
	unsigned int i = 0;
	unsigned int length = children->getLength();
	while ( i < length )	{
		TNodePtr child = children->item( i );
		TElementPtr element = shared_static_cast<TElement>( child );
		try	{
			processContent( element, aExceptions, aParent );
			return;
		}
		catch( ReadException r )	{
			TNodeListPtr results;
			switch ( r.getReason() )	{
				case WRONG_TAG_FOUND:	{
					results = aContent->getElementsByTagName( r.getWrongTag() );
					break;
				}
				case NO_TAG_FOUND:	{
					results = aContent->getElementsByTagName( "#PCDATA" );
					if ( results->getLength() == 0 )	{
						results = aContent->getElementsByTagName( kCDATA );
					}
					break;
				}
				case NO_PCDATA_FOUND:	{
					i++;
					continue;
				}
				case END_TAG_FOUND:
				case END_OF_FILE_REACHED:	{
					throw r;
					break;
				}
				default:	{
					if ( r.isFatal() )	{
						throw r;
					}
					break;
				}
			}
			if ( results.get() != NULL )	{
				if ( results->getLength() > 0 )	{
					TNodePtr node = results->item( 0 );
					element = shared_static_cast<TElement>( node );
					processContent( element, aExceptions, aParent );
					return;
				}
				else	{
					throw ReadException( mDocText->getLineNr(),
													mDocText->getCharNr(),
													"No tag of or children found" );
				}
			}
		}
	}

	throw ReadException( mDocText->getLineNr(), mDocText->getCharNr(),
									"No tag of or children found" );

}

void ElementParser	::	processAnd( const TElementPtr & aContent,
												   const TElementPtr & aExceptions,
												   TNodePtr aParent )	{

	printf( "At seq element\n" );
	TNodeListPtr children = aContent->getChildNodes();
	bool skip = false;
	for ( unsigned int i = 0; i < children->getLength(); i++ )	{
		TNodePtr child = children->item( i );
		TElementPtr element = shared_static_cast<TElement>( child );
		if ( skip )	{
			skipContent( element );
		}
		else	{
			try	{
				processContent( element, aExceptions, aParent );
			}
			catch( ReadException r )	{
				if ( r.isEndTag() && ! r.isFatal() )	{
					// See if we can skip the rest of the tags.
					skip = true;
				}
			}
		}
	}
	printf( "Finished seq element\n" );

}

void ElementParser	::	processComma( const TElementPtr & aContent,
														const TElementPtr & aExceptions,
														TNodePtr aParent )	{

	printf( "At comma element\n" );
	TNodeListPtr children = aContent->getChildNodes();
	unsigned int length = children->getLength();
	vector<bool> elementsDone( length, false );
	bool progress = true;
	while ( progress )	{
		progress = false;
		for ( unsigned int i = 0; i < length; i++ )	{
			if ( elementsDone[ i ] )	{
				// Already done. Skip to next
				continue;
			}
			TNodePtr child = children->item( i );
			TElementPtr element = shared_static_cast<TElement>( child );
			try	{
				processContent( element, aExceptions, aParent );
				elementsDone[ i ] = false;
				progress = true;
			}
			catch( ReadException r )	{
				if ( ! r.isWrongTag() && r.isFatal() )	{
					throw r;
				}
				if ( r.isEndTag() )	{
					// Nothing going to happen anymore.
					break;
				}
			}
		}
	}
	printf( "Finished comma element\n" );
	
}

void ElementParser	::	processDataText( const TElementPtr & aContent,
														   const TElementPtr & aExceptions,
														   TNodePtr aParent )	{

	printf( "At data text\n" );
	string dataText = "";
	bool stFound = false;
	while ( ! stFound )	{
		if ( mDocText->getChar() != '<' )	{
			// Not yet found. Process replacable character data
			dataText += processRepCharData();
		}
		else	{
			stFound = true;
		}
	}
	if ( dataText == "" )	{
		throw ReadException( mDocText->getLineNr(), mDocText->getCharNr(),
										"PCDATA expected", NO_PCDATA_FOUND );
	}
	TTextPtr text = mDocument->createText( dataText );
	aParent->appendChild( text );
	printf( "Finished data text: %s\n", dataText.c_str() );

}

void ElementParser	::	processExceptions( const TElementPtr & aExceptions,
															TNodePtr aParent )	{
	
	// Figure out if there actually are exceptions.
	if (aExceptions.get() != NULL) {
		bool exceptionFound = true;
		while ( exceptionFound )	{
			try	{
				processExceptionOtherContent();
				processException( aExceptions, aParent );
			}
			catch( ReadException r )	{
				if ( r.isFatal() || r.isEndTag() )	{
					throw r;
				}
				else	{
					exceptionFound = false;
				}
			}
		}
	}

}

void ElementParser	::	processException( const TElementPtr & aExceptions,
															TNodePtr aParent )	{

	if ( aExceptions.get() == NULL || aExceptions->hasChildNodes() == false)	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(), "No exceptions" );
	}
	TNodePtr plusNode = aExceptions->getFirstChild();
	TElementPtr plus = shared_static_cast<TElement>( plusNode );
	if ( plus->getNodeName() != mPlus )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(), "No plus exceptions" );
	}
	TNodePtr elementsNode = plus->getFirstChild();
	TElementPtr elements = shared_static_cast<TElement>(elementsNode);

	TNodeListPtr children = elements->getChildNodes();
	string token = "";
	for ( unsigned int i = 0; i < children->getLength(); i++ )	{
		TNodePtr child = children->item( i );
		TDOMString name = child->getNodeName();
		printf( "Looking at exception %s\n", name.c_str() );
		if ( name == token || token == "" )	{
			try	{
				TElementDeclarationPtr declaration = getElementDecl( name, mElements );
				processElement( name, declaration, aParent );
				printf( "Finished exception\n" );
				return;
			}
			catch( ReadException r )	{
				switch ( r.getReason() )	{
					case WRONG_TAG_FOUND:	{
						token = r.getWrongTag();
						printf( "Token: %s\n", token.c_str() );
						break;
					}
					case NO_TAG_FOUND:	{
						printf( "NO TAG FOUND\n" );
						TNodeListPtr results = elements->getElementsByTagName( "#PCDATA" );
						if ( results->getLength() != 0 )	{
							token = "#PCDATA";
						}
						else	{
							results = elements->getElementsByTagName( "CDATA" );
							if ( results->getLength() != 0 )	{
								token = "CDATA";
							}
						}
						break;
					}
					case END_TAG_FOUND:
					case END_OF_FILE_REACHED:	{
						throw r;
						break;
					}
					default:	{
						if ( r.isFatal() )	{
							throw r;
						}
						break;
					}
				}
			}
		}
	}

	throw ReadException( mDocText->getLineNr(), mDocText->getCharNr(),
									"No exception found" );

}

void ElementParser	::	processExceptionOtherContent()	{

	bool otherContentFound = true;
	while ( otherContentFound )	{
		if ( ! processCommentDeclaration() )	{
			if ( ! processS() )	{
				otherContentFound = false;
			}
		}
	}

}

TElementDeclarationPtr ElementParser	::	getElementDecl( const string & aName,
																					TElementPtr declarations ) const	{
	
	//printf( "Trying to get declaration: %s\n", aName.c_str() );

	TNodeListPtr results = declarations->getElementsByTagName( aName );
	unsigned int length = results->getLength();
	unsigned int i = 0;
	while ( i < length )	{
		TNodePtr node = results->item( i );
		TNodePtr parent = node->getParentNode();
		if ( parent->getNodeName() == "elements" )	{
			parent = parent->getParentNode();
			if ( parent->getNodeName() == "declaration" )	{
				// This is the one we want.
				TElementDeclarationPtr declaration =
					shared_static_cast<TElementDeclaration>( parent );
				return declaration;
			}
		}
		i++;
	}

	throw ElementDeclException();
	
}	

void ElementParser	::	processComments()	{

	bool commentFound = true;
	while ( commentFound )	{
		commentFound = processCommentDeclaration();
	}
	
}

bool ElementParser	::	processCommentDeclaration()	{

	return commentParser->parse();
	
}

TElementPtr ElementParser	::	getElement( TElementPtr aElement,
															   const TDOMString & aName )	{

	TNodeListPtr children = aElement->getChildNodes();
	TElementPtr result;
	unsigned int i = 0;
	unsigned int length = children->getLength();
	bool found = false;
	while ( i < length && ! found )	{
		TNodePtr child = children->item( i );
		if ( child->getNodeName() == aName )	{
			result = shared_static_cast<TElement>( child );
			found = true;
		}
		i++;
	}

	return result;

}
