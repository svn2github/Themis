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

ElementParser	::	ElementParser( SGMLTextPtr aDocText,
												  TDocumentShared aDTD,
												  TElementShared aParEntities,
												  TElementShared aCharEntities )
						:	BaseParser()	{

	//printf( "Constructing ElementParser\n" );
	
	mDocText = aDocText;
	mDTD = aDTD;
	mParEntities = aParEntities;
	mCharEntities = aCharEntities;

	// Document to store the element tree
	mDocument = TDocumentShared( new TDocument() );
	mDocument->setSmartPointer( mDocument );

	// Comment declaration parser
	commentParser = new CommentDeclParser( aDocText, aDTD, aParEntities, aCharEntities );

	TNodeListShared children = mDTD->getChildNodes();
	
	for ( unsigned int i = 0; i < children->getLength(); i++ )	{
		TNodeShared child = make_shared( children->item( i ) );
		if ( child->getNodeName() == "elements" )	{
			mElements = shared_static_cast<TElement>( child );
		}
	}
	
}

ElementParser	::	~ElementParser()	{

	//printf( "Destroying ElementParser\n" );
	
	delete commentParser;
	
}

void ElementParser	::	parse( const map<string, Position> & aEntityTexts,
										  const string & aName )	{

	mEntityTexts = aEntityTexts;

	parse( aName );
	
}

void ElementParser	::	parse( const string & aName )	{
	
	mName = aName;
	
	TElementShared elementDecl = getElementDecl( aName, mElements );
	processElement( elementDecl, mDocument );
	
	showTree( mDocument, 0 );

}

void ElementParser	::	processElement( const TElementShared & aElementDecl,
														 TNodeShared aParent )	{
		
	printf( "Trying to find: %s\n", aElementDecl->getNodeName().c_str() );
	// Minimization
	bool end = true;

	State save = mDocText->saveState();
	try	{
		processComments();
		processSStar();
		processStartTag( aElementDecl );
	}
	catch( ReadException r )	{
		printf( "Didn't find: %s\n", aElementDecl->getNodeName().c_str() );
		mDocText->restoreState( save );
		throw r;
	}
	
	TElementShared element =
		mDocument->createElement( aElementDecl->getNodeName() );
	aParent->appendChild( element );
	
	// See if this element declaration is part of a name group
	TNodeShared parent = make_shared( aElementDecl->getParentNode() );
	TNodeShared child = aElementDecl;
	TElementShared exceptions;
	if ( parent->getNodeName() != "elements" )	{
		// Is part of a name group
		child = make_shared( parent->getFirstChild() );
		TNodeShared base = make_shared( parent->getParentNode() );
		TElementShared baseElement = shared_static_cast<TElement>( base );
		if ( baseElement->getAttribute( "end" ) == "false" )	{
			end = false;
		}
		TNodeListShared list = baseElement->getChildNodes();
		for ( unsigned int i = 0; i < list->getLength(); i++ )	{
			TNodeShared child = list->item( i );
			if ( child->getNodeName() == "exceptions" )	{
				exceptions = shared_static_cast<TElement>( child );
			}
		}
	}
	else	{
		TElementShared childElement = shared_static_cast<TElement>( child );
		if ( childElement->getAttribute( "end" ) == "false" )	{
			end = false;
		}
		TNodeListShared list = childElement->getChildNodes();
		for ( unsigned int i = 0; i < list->getLength(); i++ )	{
			TNodeShared child = list->item( i );
			if ( child->getNodeName() == "exceptions" )	{
				exceptions = shared_static_cast<TElement>( child );
			}
		}
	}
	
	if ( child->hasChildNodes() )	{
		TNodeShared node = make_shared( child->getFirstChild() );
		TElementShared content	= shared_static_cast<TElement>( node );
		processContent( content, exceptions, element );
	}

	save = mDocText->saveState();
	try	{
		processSStar();
		processEndTag( aElementDecl );
	}
	catch( ReadException r )	{
		mDocText->restoreState( save );
		if ( end )	{
			throw r;
		}
	}
	
}

void ElementParser	::	processStartTag( const TElementShared & elementDecl )	{
	
	process( mStago );
	
	// Skipping the document type specification for now
	
	string name = "";
	try	{
		name = processGenIdSpec();
	}
	catch( ReadException r )	{
		throw r;
	}
	if ( name == elementDecl->getNodeName() )	{
		printf( "Correct element start tag found: %s\n", name.c_str() );
	}
	else	{
		string error = "Expected start tag ";
		error += elementDecl->getNodeName();
		throw ReadException( mDocText->getLineNr(), mDocText->getCharNr(),
										error, true, false, true );
	}
	
	// Skipping some stuff for now
	try	{
		processAttrSpecList();
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

void ElementParser	::	processEndTag( const TElementShared & elementDecl )	{
	
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
		throw ReadException( mDocText->getLineNr(), mDocText->getCharNr(),
										error, true, false, true );
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

void ElementParser	::	processAttrSpecList()	{

	bool attrSpecFound = true;
	while ( attrSpecFound )	{
		try	{
			processAttrSpec();
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

void ElementParser	::	processAttrSpec()	{

	processSStar();
	
	try	{
		processName();
		try	{
			processSStar();
			process( mVi );
			processSStar();
		}
		catch( ReadException r )	{
			r.setFatal();
			throw r;
		}
	}
	catch( ReadException r )	{
		// Optional. Do nothing
	}
	
	// Not entirely correct. Check later
	
	processAttrValueSpec();
	
}

void ElementParser	::	processContent( const TElementShared & aContent,
														 const TElementShared & aExceptions,
														 TNodeShared aParent )	{
	processExceptions( aExceptions, aParent );

	string contentName = aContent->getNodeName();
	
	// Using switch statement, by only looking at the first character.
	switch( contentName[ 0 ] )	{
		case '(':	{
			printf( "At brackets\n" );
			TNodeShared child = make_shared( aContent->getFirstChild() );
			TElementShared subContent = shared_static_cast<TElement>( child );
			processContent( subContent, aExceptions, aParent );
			printf( "Finished brackets\n" );
			break;
		}
		case '?':	{
			printf( "At optional element\n" );
			TNodeShared child = make_shared( aContent->getFirstChild() );
			TElementShared subContent = shared_static_cast<TElement>( child );
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
			break;
		}
		case '+':	{
			printf( "At plus element\n" );
			TNodeShared child = make_shared( aContent->getFirstChild() );
			TElementShared subContent = shared_static_cast<TElement>( child );
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
			break;
		}
		case '*':	{
			printf( "At star element\n" );
			TNodeShared child = make_shared( aContent->getFirstChild() );
			TElementShared subContent = shared_static_cast<TElement>( child );
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
			break;
		}
		case '|':	{
			printf( "At or element\n" );
			TNodeListShared children = aContent->getChildNodes();
			bool found = false;
			for ( unsigned int i = 0; i < children->getLength(); i++ )	{
				TNodeShared child = make_shared( children->item( i ) );
				TElementShared element = shared_static_cast<TElement>( child );
				try	{
					processContent( element, aExceptions, aParent );
					found = true;
					break;
				}
				catch( ReadException r )	{
					if ( ! r.isWrongTag() && r.isFatal() )	{
						throw r;
					}
				}
			}
			if ( ! found )	{
				throw ReadException( mDocText->getLineNr(), mDocText->getCharNr(),
												"No tag of or children found" );
			}
			printf( "Finished or element\n" );
			break;
		}
		case '&':	{
			printf( "At seq element\n" );
			TNodeShared child = make_shared( aContent->getFirstChild() );
			TElementShared subContent = shared_static_cast<TElement>( child );
			printf( "Processing first seq child\n" );
			processContent( subContent, aExceptions, aParent );
			child = make_shared( aContent->getLastChild() );
			subContent = shared_static_cast<TElement>( child );
			printf( "Processing second seq child\n" );
			processContent( subContent, aExceptions, aParent );
			printf( "Finished seq element\n" );
			break;
		}
		case ',':	{
			printf( "At and element\n" );
			TNodeListShared children = aContent->getChildNodes();
			for ( unsigned int i = 0; i < children->getLength(); i++ )	{
				TNodeShared child = make_shared( children->item( i ) );
				TElementShared element = shared_static_cast<TElement>( child );
				element->setAttribute( "used", "false" );
			}
			bool progress = true;
			while ( progress )	{
				progress = false;
				for ( unsigned int i = 0; i < children->getLength(); i++ )	{
					TNodeShared child = make_shared( children->item( i ) );
					TElementShared element = shared_static_cast<TElement>( child );
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
			printf( "Finished and element\n" );
			break;
		}
		case '#':	{
			printf( "At data text\n" );
			string text = "";
			bool stFound = false;
			while ( ! stFound )	{
				if ( mDocText->getChar() != '<' )	{
					// Not yet found. Process replacable character data
					try	{
						text += processRepCharData();
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
			if ( text == "" )	{
				throw ReadException( mDocText->getLineNr(), mDocText->getCharNr(),
												"PCDATA expected" );
			}
			printf( "Finished data text: %s\n", text.c_str() );
			break;
		}
		default:	{
			printf( "At tag\n" );
			TElementShared elementDecl = getElementDecl( contentName, mElements );
			processElement( elementDecl, aParent );
			printf( "Finished tag\n" );
		}
	}
	
}

void ElementParser	::	processExceptions( const TElementShared & aExceptions,
															TNodeShared aParent )	{
	
	bool exceptionFound = true;
	while ( exceptionFound )	{
		try	{
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
void ElementParser	::	processException( const TElementShared & aExceptions,
															TNodeShared aParent )	{
	
	if ( aExceptions.get() == 0 )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(), "No exceptions" );
	}
	TNodeShared plusNode = make_shared( aExceptions->getFirstChild() );
	TElementShared plus = shared_static_cast<TElement>( plusNode );
	if ( plus->getNodeName() != mPlus )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(), "No plus exceptions" );
	}
	TNodeShared plusChild = make_shared( plus->getFirstChild() );
	TNodeShared connectorNode = make_shared( plusChild->getFirstChild() );
	TElementShared connector = shared_static_cast<TElement>( connectorNode );
	TNodeListShared list = connector->getChildNodes();
	for ( unsigned int i = 0; i < list->getLength(); i++ )	{
		TNodeShared child = make_shared( list->item( i ) );
		try	{
			printf( "Trying an exception tag\n" );
			TElementShared elementDecl =
				getElementDecl( child->getNodeName(), mElements );
			processElement( elementDecl, aParent );
			return;
		}
		catch( ReadException r )	{
			// Not the one. Try next one
		}
	}

	throw ReadException( mDocText->getLineNr(),
									mDocText->getCharNr(), "No exception found" );

}

TElementShared ElementParser	::	getElementDecl( const string & aName,
																			TElementShared declarations ) const	{
	
	//printf( "Trying to get declaration: %s\n", aName.c_str() );
	
	TNodeListShared children = declarations->getChildNodes();
	for ( unsigned int i = 0; i < children->getLength(); i++ )	{
		TNodeShared child = make_shared( children->item( i ) );
		if ( child->getNodeName() == aName )	{
			return shared_static_cast<TElement>( child );
		}
		if ( child->getNodeName() == "()" )	{
			TElementShared declaration = shared_static_cast<TElement>( child );
			try	{
				return getElementDecl( aName, declaration );
			}
			catch( ElementDeclException e )	{
				// Not found. Ah well.
			}
		}
		if ( child->getNodeName() == "|" )	{
			TElementShared declaration = shared_static_cast<TElement>( child );
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
		printf( "Comment found\n" );
		return;
	}
	catch( ReadException r )	{
		if ( ! r.isFatal() )	{
			mDocText->restoreState( save );
		}
		throw r;
	}
	
}

void ElementParser	::	showTree( const TNodeShared aNode, int aSpacing )	{
	
	TNodeListShared children = aNode->getChildNodes();
	int length = children->getLength();
	if ( length != 0 )	{
		for ( int i = 0; i < length; i++ )	{
			TNodeShared child = make_shared( children->item( i ) );
			for ( int j = 0; j < aSpacing; j++ )	{
				cout << "  ";
			}
			cout << "Child name: " << child->getNodeName().c_str() << endl;
			if ( child->getNodeType() == ELEMENT_NODE )	{
				// Check for attributes
				TNamedNodeMapShared attributes = child->getAttributes();
				for ( unsigned int j = 0; j < attributes->getLength(); j++ )	{
					TNodeWeak attr = attributes->item( j );
					TNodeShared tempAttr = make_shared( attr );
					for ( int j = 0; j < aSpacing + 1; j++ )	{
						cout << "  ";
					}
					cout << "Attribute " << tempAttr->getNodeName();
					cout << " with value " << tempAttr->getNodeValue() << endl;
				}
			}
			showTree( child, aSpacing + 1 );
		}
	}	
}
