/*	SGMLParser implementation
	See SGMLParser.hpp for more information
	
	Also includes main()
*/

// Standard C headers
#include <stdio.h>

// Standard C++ headers
#include <fstream>

// SGMLParser headers
#include "SGMLParser.hpp"
#include "ReadException.hpp"
#include "State.hpp"
#include "SGMLSupport.hpp"
#include "CommentDeclParser.hpp"
#include "DTDParser.hpp"
#include "DocTypeDeclParser.hpp"
#include "ElementParser.hpp"

// DOM headers
#include "TNode.h"
#include "TElement.h"
#include "TNodeList.h"
#include "TNamedNodeMap.h"

SGMLParser	::	SGMLParser( const char * aDtd, const char * aDocument )
					:	BaseParser()	{
	
	//printf( "SGMLParser constructed\n" );

	// Load text
	mDocText = SGMLTextPtr( new SGMLText() );
	ifstream file( aDocument );
	
	char ch;
	while ( file.get( ch ) )	{
		mDocText->addChar( ch );
	};
	
	// Create declaration parsers
	setupParsers( aDtd );
	
}

SGMLParser	::	SGMLParser( const char * aDtd, SGMLTextPtr aDocument )
					:	BaseParser()	{
	
	//printf( "SGMLParser constructed\n" );

	// Load text
	mDocText = aDocument;

	// Create declaration parsers
	setupParsers( aDtd );
	
}

SGMLParser	::	~SGMLParser()	{
	
	//printf( "SGMLParser destroyed\n" );
	
	delete mCommentDecl;
	delete mDtdParser;
	delete mDocTypeDecl;
	
}

void SGMLParser	::	setupParsers( const char * aDtd )	{

	mCommentDecl =
		new CommentDeclParser( mDocText, mDTD, mParEntities, mCharEntities );
	mDtdParser =
		new DTDParser( aDtd, mDTD );
	mDocTypeDecl =
		new DocTypeDeclParser( mDocText, mDTD, mParEntities, mCharEntities );
	mElementParser =
		new ElementParser( mDocText, mDTD, mParEntities, mCharEntities );

}

void SGMLParser	::	processSGMLDocument()	{
	
	processSGMLDocEntity();
	
}

void SGMLParser	::	processSGMLDocEntity()	{

	processSStar();
	// Haven't actually seen one of these anywhere. Leaving it out for now
	//processSGMLDeclaration();
	processProlog();
	
}

void SGMLParser	::	processProlog()	{

	// Not very complete, but works for html files

	processOtherPrologStar();
	processBaseDocTypeDecl();
	processOtherPrologStar();
	
}

void SGMLParser	::	processOtherProlog()	{

	// Missing processing instruction

	State save = mDocText->saveState();

	try	{
		mCommentDecl->parse();
		return;
	}
	catch( ReadException r )	{
		if ( r.isFatal() )	{
			throw r;
		}
		else	{
			mDocText->restoreState( save );
		}
	}

	try	{
		processS();
	}
	catch( ReadException r )	{
		if ( r.isFatal() )	{
			throw r;
		}
		else	{
			mDocText->restoreState( save );
		}
	}
	
	throw ReadException( mDocText->getLineNr(), mDocText->getCharNr(),
									"Other prolog expected" );
	
}

void SGMLParser	::	processOtherPrologStar()	{

	bool otherFound = true;
	while ( otherFound )	{
		try	{
			processOtherProlog();
		}
		catch( ReadException r )	{
			if ( r.isFatal() )	{
				throw r;
			}
			else	{
				otherFound = false;
			}
		}
	}
	
}

void SGMLParser	::	processBaseDocTypeDecl()	{

	try	{
		mDocTypeDecl->parse();
	}
	catch( ReadException r )	{
		// Only throwing when fatal, because most documents lack the doctype declaration
		if ( r.isFatal() )	{
			throw r;
		}
	}
	
}

void SGMLParser	::	processDocInstanceSet()	{

	processBaseDocElement();
	processOtherPrologStar();
	
}

void SGMLParser	::	processBaseDocElement()	{
	
	processDocElement();
	
}

void SGMLParser	::	processDocElement()	{
	
	string docTypeName = mDocTypeDecl->getDocTypeName();
	if ( docTypeName == "" )	{
		// No doctype declaration. Assuming this is html
		docTypeName = "HTML";
	}
		
	mElementParser->parse( docTypeName );
	
}

TDocumentShared SGMLParser	::	parse()	{
	
	mDtdParser->parse();
	
	try	{
		processSStar();
		processProlog();
		processDocInstanceSet();
	}
	catch( ReadException r )	{
		if ( r.isEof() )	{
			printf( "End of file reached. No errors encountered\n" );
		}
		else	{
			printf( "Error on line %i, char %i, message: %s\n", r.getLineNr(), r.getCharNr(),
					  r.getErrorMessage().c_str() );
		}
	}

	mDocument = mElementParser->getDocument();
	showTree( mDocument, 0 );

	return mDocument;
	
}

void SGMLParser	::	showTree( const TNodeShared aNode, int aSpacing )	{
	
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

int main( int argc, char * argv[] )	{
	
	if ( argc < 3 )	{
		cout << "Please supply a dtd and a document to load\n";
		return 1;
	}
	
	SGMLParser * sgmlParser = new SGMLParser( argv[ 1 ], argv[ 2 ] );
	TDocumentShared dtd = sgmlParser->parse();

	delete sgmlParser;
	
	return 0;
	
}
