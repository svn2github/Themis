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

// DOM headers
#include "TElement.h"
#include "TNodeList.h"
#include "TNamedNodeMap.h"
#include "TText.h"
#include "TAttr.h"

ElementParser	::	ElementParser( SGMLTextPtr aDocText,
												  TDocumentPtr aDTD )
						:	BaseParser()	{

	//printf( "Constructing ElementParser\n" );
	
	mDocText = aDocText;

	setDTD( aDTD );	

	// Comment declaration parser
	commentParser = new CommentDeclParser( aDocText, mDTD );

	// Document to store the element tree
	mDocument = TDocumentPtr( new TDocument() );


}

ElementParser	::	~ElementParser()	{

	//printf( "Destroying ElementParser\n" );
	
	delete commentParser;
	
}

void ElementParser	::	setDocText( SGMLTextPtr aDocText )	{
	
	mDocText = aDocText;
	commentParser->setDocText( aDocText );
	
}

void ElementParser	::	setDTD( TDocumentPtr aDTD )	{

	mDTD = aDTD;
	
	TNodeListPtr list = mDTD->getChildNodes();
	unsigned int length = list->getLength();
	for ( unsigned int i = 0; i < length; i++ )	{
		TNodePtr node = list->item( i );
		TElementPtr element = shared_static_cast<TElement>( node );
		if ( element->getNodeName() == "parEntities" )	{
			mParEntities = element;
		}
		if ( element->getNodeName() == "charEntities" )	{
			mCharEntities = element;
		}
		if ( element->getNodeName() == "elements" )	{
			mElements = element;
		}
	}

	mEntityTexts.clear();
	
}

void ElementParser	::	parse( const map<string, Position> & aEntityTexts,
										  const string & aName )	{

	mEntityTexts = aEntityTexts;

	parse( aName );
	
}

void ElementParser	::	parse( const string & aName )	{
	
	mName = aName;
	
	TElementPtr elementDecl = getElementDecl( aName, mElements );
	processElementContent( elementDecl, mDocument );
	
}

TDocumentPtr ElementParser	::	getDocument() const	{
	
	return mDocument;
	
}

void ElementParser	::	processElementContent( const TElementPtr & aElementDecl,
																	TNodePtr aParent )	{

	bool contentFound = true;
	while ( contentFound )	{
		try	{
			processComment();
		}
		catch( ReadException r )	{
			if ( r.isFatal() )	{
				throw r;
			}
			try	{
				processS();
			}
			catch( ReadException r )	{
				if ( r.isFatal() )	{
					throw r;
				}
				else	{
					contentFound = false;
				}
			}
		}
	}

	processElement( aElementDecl, aParent );
																		
}

void ElementParser	::	processUnknownTags( TNodePtr aParent )	{

	bool contentFound = true;
	while ( contentFound )	{
		try	{
			processComment();
		}
		catch( ReadException r )	{
			if ( r.isFatal() )	{
				throw r;
			}
			try	{
				processS();
			}
			catch( ReadException r )	{
				if ( r.isFatal() )	{
					throw r;
				}
				try	{
					processUnknownStartTag( aParent );
				}
				catch( ReadException r )	{
					if ( r.isFatal() )	{
						throw r;
					}
					try	{
						processUnknownEndTag();
					}
					catch( ReadException r )	{
						if ( r.isFatal() )	{
							throw r;
						}
						else	{
							contentFound = false;
						}
					}
				}
			}
		}
	}
	
}

void ElementParser	::	processUnknownStartTag( TNodePtr aParent )	{

	State save = mDocText->saveState();

	try	{
		process( mStago );
	}
	catch( ReadException r )	{
		r.setReason( NO_TAG_FOUND );
		mDocText->restoreState( save );
		throw r;
	}
	
	// Skipping the document type specification for now
	
	string name = "";
	try	{
		name = processGenIdSpec();
	}
	catch( ReadException r )	{
		mDocText->restoreState( save );
		throw r;
	}
	
	try	{
		getElementDecl( name, mElements );
		ReadException exception( mDocText->getLineNr(), mDocText->getCharNr(),
											  "Found valid tag", WRONG_TAG_FOUND );
		exception.setWrongTag( name );
		mDocText->restoreState( save );
		throw exception;
	}
	catch( ElementDeclException e )	{
		// Element does not exist in DTD. Skip it
		printf( "WARNING: Found element that does not exist in DTD: %s.", name.c_str() );
		printf( " Check the HTML source or make sure this is the right DTD for this file\n" );
		try	{
			printf( "Processing AttrSpecList\n" );
			processAttrSpecList( aParent );
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
		
	}

}

void ElementParser	::	processUnknownEndTag()	{
	
	State save = mDocText->saveState();
	
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

	try	{
		getElementDecl( name, mElements );
		ReadException exception( mDocText->getLineNr(), mDocText->getCharNr(),
											  "Found valid tag", WRONG_TAG_FOUND );
		exception.setWrongTag( name );
		mDocText->restoreState( save );
		throw exception;
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
	
}

void ElementParser	::	processElement( const TElementPtr & aElementDecl,
														 TNodePtr aParent )	{
		
	printf( "Trying to find: %s\n", aElementDecl->getNodeName().c_str() );
	// Minimization
	bool start = true;
	bool end = true;

	// See if this element declaration is part of a name group
	TNodePtr parent = aElementDecl->getParentNode();
	TElementPtr child = aElementDecl;
	TElementPtr exceptions;
	if ( parent->getNodeName() != "elements" )	{
		// Is part of a name group
		TNodePtr firstChild = parent->getFirstChild();
		child = shared_static_cast<TElement>( firstChild );
	}
	// Get the start tag minimization
	if ( child->getAttribute( "start" ) == "false" )	{
		start = false;
	}
	// Get the end tag minimization
	if ( child->getAttribute( "end" ) == "false" )	{
		printf( "End tag not required\n" );
		end = false;
	}
	// Get the exception list
	TNodeListPtr list = child->getChildNodes();
	for ( unsigned int i = 0; i < list->getLength(); i++ )	{
		TNodePtr item = list->item( i );
		if ( item->getNodeName() == "exceptions" )	{
			exceptions = shared_static_cast<TElement>( item );
		}
	}

	TNodePtr tag;
	State save = mDocText->saveState();
	try	{
		TElementPtr element = processStartTag( aElementDecl, aParent );
		// Cast it to a node
		tag = shared_static_cast<TNode>( element );
	}
	catch( ReadException r )	{
		printf( "Didn't find: %s\n", aElementDecl->getNodeName().c_str() );
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

	if ( child->hasChildNodes() )	{
		TNodePtr node = child->getFirstChild();
		TElementPtr content	= shared_static_cast<TElement>( node );
		processContent( content, exceptions, tag );
	}

	// Take out any unknown tags
	processUnknownTags( aParent );

	printf( "Done with content. Trying to close up %s\n", aElementDecl->getNodeName().c_str() );
	
	save = mDocText->saveState();
	try	{
		processSStar();
		processEndTag( aElementDecl );
		printf( "Closed up %s correctly\n", aElementDecl->getNodeName().c_str() );
	}
	catch( ReadException r )	{
		mDocText->restoreState( save );
		if ( end )	{
			printf( "Throwing exception on closing of %s\n", aElementDecl->getNodeName().c_str() );
			throw r;
		}
		printf( "Closed up %s implicitly\n", aElementDecl->getNodeName().c_str() );
	}

	// Take out any unknown tags
	processUnknownTags( aParent );
	
}

TElementPtr ElementParser	::	processStartTag( const TElementPtr & elementDecl, TNodePtr aParent )	{
	
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
	if ( name == elementDecl->getNodeName() )	{
		printf( "Correct element start tag found: %s\n", name.c_str() );
		element = mDocument->createElement( name );
		aParent->appendChild( element );
	}
	else	{
		string error = "Expected start tag ";
		error += elementDecl->getNodeName();
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

void ElementParser	::	processEndTag( const TElementPtr & elementDecl )	{
	
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
	if ( name == elementDecl->getNodeName() )	{
		printf( "Correct element end tag found: %s\n", name.c_str() );
	}
	else	{
		string error = "Expected closing tag ";
		error += elementDecl->getNodeName();
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
			processAttrSpec( aParent );
		}
		catch( ReadException r )	{
			if ( r.isFatal() )	{
				throw r;
			}
			else	{
				attrSpecFound = false;
			}
		}
	}
	
}

void ElementParser	::	processAttrSpec( TNodePtr aParent )	{

	processSStar();
	
	TAttrPtr attr;
	
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
	}
	catch( ReadException r )	{
		// Optional.
		mDocText->restoreState( save );
	}
	
	// Not entirely correct. Check later
	string value = processAttrValueSpec();
	if ( attr.get() != NULL )	{
		attr->setValue( value );
	}

	printf( "Found AttrValueSpec: %s\n", value.c_str() );
	
}

void ElementParser	::	processContent( const TElementPtr & aContent,
														 const TElementPtr & aExceptions,
														 TNodePtr aParent )	{
	
	// Moved to just before tag
	//processExceptions( aExceptions, aParent );

	string contentName = aContent->getNodeName();
	
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
			processOr( aContent, aExceptions, aParent );
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
			TElementPtr elementDecl = getElementDecl( contentName, mElements );
			processElementContent( elementDecl, aParent );
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
	string token = "";
	for ( unsigned int i = 0; i < children->getLength(); i++ )	{
		TNodePtr child = children->item( i );
		TElementPtr element = shared_static_cast<TElement>( child );
		if ( element->getTagName() == token || token == "" )	{
			try	{
				processContent( element, aExceptions, aParent );
				printf( "Finished or element\n" );
				return;
			}
			catch( ReadException r )	{
				switch ( r.getReason() )	{
					case WRONG_TAG_FOUND:	{
						token = r.getWrongTag();
						break;
					}
					case NO_TAG_FOUND:	{
						printf( "NO TAG FOUND\n" );
						TNodeListPtr results = aContent->getElementsByTagName( "#PCDATA" );
						if ( results->getLength() != 0 )	{
							token = "#PCDATA";
						}
						else	{
							results = aContent->getElementsByTagName( "CDATA" );
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
									"No tag of or children found" );

}

void ElementParser	::	processAnd( const TElementPtr & aContent,
												   const TElementPtr & aExceptions,
												   TNodePtr aParent )	{

	printf( "At seq element\n" );
	TNodeListPtr children = aContent->getChildNodes();
	for ( unsigned int i = 0; i < children->getLength(); i++ )	{
		TNodePtr child = children->item( i );
		TElementPtr element = shared_static_cast<TElement>( child );
		processContent( element, aExceptions, aParent );
	}
	printf( "Finished seq element\n" );

}

void ElementParser	::	processComma( const TElementPtr & aContent,
														const TElementPtr & aExceptions,
														TNodePtr aParent )	{

	printf( "At comma element\n" );
	TNodeListPtr children = aContent->getChildNodes();
	for ( unsigned int i = 0; i < children->getLength(); i++ )	{
		TNodePtr child = children->item( i );
		TElementPtr element = shared_static_cast<TElement>( child );
		element->setAttribute( "used", "false" );
	}
	bool progress = true;
	while ( progress )	{
		progress = false;
		for ( unsigned int i = 0; i < children->getLength(); i++ )	{
			TNodePtr child = children->item( i );
			TElementPtr element = shared_static_cast<TElement>( child );
			if ( element->getAttribute( "used" ) == "true" )	{
				// Already done. Skip to next
				continue;
			}
			unsigned int before = mDocText->getIndex();
			try	{
				processContent( element, aExceptions, aParent );
			}
			catch( ReadException r )	{
				if ( ! r.isWrongTag() && r.isFatal() )	{
					throw r;
				}
			}
			unsigned int after = mDocText->getIndex();
			if ( before != after )	{
				printf( "Content processed\n" );
				element->setAttribute( "used", "true" );
				progress = true;
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
			try	{
				dataText += processRepCharData();
			}
			catch( ReadException r )	{
				r.setFatal();
				throw r;
			}
		}
		else	{
			stFound = true;
		}
	}
	if ( dataText == "" )	{
		throw ReadException( mDocText->getLineNr(), mDocText->getCharNr(),
										"PCDATA expected" );
	}
	TTextPtr text = mDocument->createText( dataText );
	aParent->appendChild( text );
	printf( "Finished data text: %s\n", dataText.c_str() );

}

void ElementParser	::	processExceptions( const TElementPtr & aExceptions,
															TNodePtr aParent )	{
	
	bool exceptionFound = true;
	while ( exceptionFound )	{
		try	{
			processExceptionOtherContent();
			processException( aExceptions, aParent );
		}
		catch( ReadException r )	{
			if ( r.isFatal() )	{
				throw r;
			}
			else	{
				exceptionFound = false;
			}
		}
	}

}
void ElementParser	::	processException( const TElementPtr & aExceptions,
															TNodePtr aParent )	{
	
	if ( aExceptions.get() == 0 )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(), "No exceptions" );
	}
	TNodePtr plusNode = aExceptions->getFirstChild();
	TElementPtr plus = shared_static_cast<TElement>( plusNode );
	if ( plus->getNodeName() != mPlus )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(), "No plus exceptions" );
	}
	TNodePtr plusChild = plus->getFirstChild();
	TNodePtr connectorNode = plusChild->getFirstChild();
	TElementPtr connector = shared_static_cast<TElement>( connectorNode );

	TNodeListPtr children = connector->getChildNodes();
	string token = "";
	for ( unsigned int i = 0; i < children->getLength(); i++ )	{
		TNodePtr child = children->item( i );
		TElementPtr element = getElementDecl( child->getNodeName(), mElements );
		if ( element->getTagName() == token || token == "" )	{
			try	{
				processElement( element, aParent );
				printf( "Finished exception\n" );
				return;
			}
			catch( ReadException r )	{
				switch ( r.getReason() )	{
					case WRONG_TAG_FOUND:	{
						token = r.getWrongTag();
						break;
					}
					case NO_TAG_FOUND:	{
						printf( "NO TAG FOUND\n" );
						TNodeListPtr results = connector->getElementsByTagName( "#PCDATA" );
						if ( results->getLength() != 0 )	{
							token = "#PCDATA";
						}
						else	{
							results = connector->getElementsByTagName( "CDATA" );
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
		try	{
			processComment();
		}
		catch( ReadException r )	{
			if ( r.isFatal() )	{
				throw r;
			}
			try	{
				processS();
			}
			catch( ReadException r )	{
				if ( r.isFatal() )	{
					throw r;
				}
				else	{
					otherContentFound = false;
				}
			}
		}
	}

}

TElementPtr ElementParser	::	getElementDecl( const string & aName,
																			TElementPtr declarations ) const	{
	
	//printf( "Trying to get declaration: %s\n", aName.c_str() );
	
	TNodeListPtr children = declarations->getChildNodes();
	for ( unsigned int i = 0; i < children->getLength(); i++ )	{
		TNodePtr child = children->item( i );
		if ( child->getNodeName() == aName )	{
			return shared_static_cast<TElement>( child );
		}
		if ( child->getNodeName() == "()" )	{
			TElementPtr declaration = shared_static_cast<TElement>( child );
			try	{
				return getElementDecl( aName, declaration );
			}
			catch( ElementDeclException e )	{
				// Not found. Ah well.
			}
		}
		if ( child->getNodeName() == "|" )	{
			TElementPtr declaration = shared_static_cast<TElement>( child );
			try	{
				return getElementDecl( aName, declaration );
			}
			catch( ElementDeclException e )	{
				// Not found. Ah well.
			}
		}
	}

	throw ElementDeclException();
	
}	

void ElementParser	::	processComments()	{

	bool commentFound = true;
	while ( commentFound )	{
		try	{
			processComment();
		}
		catch( ReadException r )	{
			if ( r.isFatal() )	{
				throw r;
			}
			else	{
				commentFound = false;
			}
		}
	}
	
}

void ElementParser	::	processComment()	{

	State save = mDocText->saveState();

	try	{
		commentParser->parse();
		return;
	}
	catch( ReadException r )	{
		if ( ! r.isFatal() )	{
			mDocText->restoreState( save );
		}
		throw r;
	}
	
}
