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

	mDocText = SGMLTextPtr( new SGMLText() );
	mDocText->loadText( aDocument );

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
	delete mElementParser;
	
}

void SGMLParser	::	setupParsers( const char * aDtd )	{

	mCommentDecl =
		new CommentDeclParser( mDocText, mDTD );
	mDtdParser =
		new DTDParser( aDtd, mDTD );
	mDocTypeDecl =
		new DocTypeDeclParser( mDocText, mDTD );
	mElementParser =
		new ElementParser( mDocText, mDTD );

	mDefaultDtd = aDtd;

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

TDocumentPtr SGMLParser	::	parse()	{
	
	parseDTD();

	mDocText->reset();
	
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
	printf( "Finished with document\n" );

	return mDocument;
	
}

TDocumentPtr SGMLParser	::	parse( const char * aDocument )	{


	mDocText->loadText( aDocument );
	
	TDocumentPtr result = parse();
	
	return result;
	
}

TDocumentPtr SGMLParser	::	parse( SGMLTextPtr aDocument )	{

	// Load text
	mDocText = aDocument;
	mCommentDecl->setDocText( mDocText );
	mDocTypeDecl->setDocText( mDocText );
	mElementParser->setDocText( mDocText );

	return parse();
	
}

void SGMLParser	::	parseDTD()	{

	if ( mDtds.count( mDefaultDtd ) == 0 )	{
		printf( "Parsing DTD\n" );
		createDTD();
		mCommentDecl->setDTD( mDTD );
		mDtdParser->setDTD( mDTD );
		mDocTypeDecl->setDTD( mDTD );
		mElementParser->setDTD( mDTD );
		mDtdParser->parse( mDefaultDtd.c_str() );
		mDtds.insert( map<string, TDocumentPtr>::value_type( mDefaultDtd, mDTD ) );
	}
	else	{
		map<string, TDocumentPtr>::iterator i = mDtds.find( mDefaultDtd );
		mDTD = (*i).second;
	}
	
}

void SGMLParser	::	parseDTD( const char * aDtd )	{

	mDefaultDtd = aDtd;
	parseDTD();
	
}

void SGMLParser	::	showTree( const TNodePtr aNode, int aSpacing )	{
	
	TNodeListPtr children = aNode->getChildNodes();
	int length = children->getLength();
	if ( length != 0 )	{
		for ( int i = 0; i < length; i++ )	{
			TNodePtr child = children->item( i );
			for ( int j = 0; j < aSpacing; j++ )	{
				cout << "  ";
			}
			cout << "Child name: " << child->getNodeName().c_str() << endl;
			if ( child->getNodeType() == ELEMENT_NODE )	{
				// Check for attributes
				TNamedNodeMapPtr attributes = child->getAttributes();
				for ( unsigned int j = 0; j < attributes->getLength(); j++ )	{
					TNodePtr attr = attributes->item( j );
					for ( int j = 0; j < aSpacing + 1; j++ )	{
						cout << "  ";
					}
					cout << "Attribute " << attr->getNodeName();
					cout << " with value " << attr->getNodeValue() << endl;
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
	TDocumentPtr dtd = sgmlParser->parse();

	delete sgmlParser;
	
	return 0;
	
}
