/*	SGMLParser implementation
	See SGMLParser.hpp for more information
	
	Also includes main()
*/

// Standard C headers
#include <stdio.h>

// SGMLParser headers
#include "SGMLParser.hpp"
#include "ReadException.hpp"
#include "State.hpp"
#include "SGMLSupport.hpp"
#include "CommentDeclParser.hpp"
#include "DTDParser.hpp"
#include "DocTypeDeclParser.hpp"
#include "ElementParser.hpp"
#include "TSchema.hpp"

// DOM headers
#include "TNode.h"
#include "TElement.h"
#include "TNodeList.h"
#include "TNamedNodeMap.h"

SGMLParser	::	SGMLParser(const char * aDocument )
			:	BaseParser( TSchemaPtr() )	{
	
	mDocText = SGMLTextPtr( new SGMLText() );
	mDocText->loadText( aDocument );

	createDTD();
	
	// Create declaration parsers
	setupParsers();
	
}

SGMLParser	::	SGMLParser(SGMLTextPtr aDocument )
			:	BaseParser( TSchemaPtr() )	{
	
	// Load text
	mDocText = aDocument;

	createDTD();
	
	// Create declaration parsers
	setupParsers();
	
}

SGMLParser	::	~SGMLParser()	{
	
	//printf( "SGMLParser destroyed\n" );
	
	delete mCommentDecl;
	delete mDtdParser;
	delete mDocTypeDecl;
	delete mElementParser;
	
}

void SGMLParser	::	createDTD()	{

	// Document to store information about the dtd
	mSchema = TSchemaPtr( new TSchema() );
	mSchema->setup();
	setSchema( mSchema );

}

void SGMLParser	::	setupParsers()	{

	mCommentDecl =
		new CommentDeclParser( mDocText, mSchema );
	mDtdParser =
		new DTDParser( mSchema );
	mDocTypeDecl =
		new DocTypeDeclParser( mDocText, mSchema );
	mElementParser =
		new ElementParser( mDocText, mSchema );

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

	if ( mCommentDecl->parse() )	{
		return;
	}
	else	{
		mDocText->restoreState( save );
	}

	if ( processS() )	{
		return;
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

TDocumentPtr SGMLParser	::	parse( const char * aDtd, SGMLTextPtr aDocument )	{

	// Load text
	mDocText = aDocument;
	mCommentDecl->setDocText( mDocText );
	mDocTypeDecl->setDocText( mDocText );
	mElementParser->setDocText( mDocText );

	parseDTD( aDtd );
	
//	showTree( mDTD, 0 );

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

	TDocumentPtr document = mElementParser->getDocument();

	showTree( document, 0 );
	printf( "Finished with document\n" );
	return document;
	
	
}

TDocumentPtr SGMLParser	::	parse( const char * aDtd, const char * aDocument )	{

	mDocText->loadText( aDocument );
	
	TDocumentPtr result = parse( aDtd, mDocText );
	
	return result;
	
}

void SGMLParser	::	parseDTD( const char * aDtd )	{

	printf("Requested dtd: %s\n", aDtd);
	printf("Default dtd: %s\n", mDefaultDtd.c_str());
	
	string dtd = aDtd;
	// Find out if the requested DTD is the one currently loaded.
	if (dtd != mDefaultDtd) {
		if ( mDtds.count( dtd ) == 0 )	{
			// The DTD is not in the repository yet.
			// Figure out if this is the first DTD we create.
			if (mDtds.size() != 0) {
				// We already have a DTD, so we have to create a new one.
				createDTD();
				mCommentDecl->setSchema( mSchema );
				mDtdParser->setSchema( mSchema );
				mDocTypeDecl->setSchema( mSchema );
				mElementParser->setSchema( mSchema );
			}
			// Parse it and add it to the repository.
			printf( "Parsing DTD\n" );
			mDtdParser->parse(aDtd);
			mDtds.insert( map<string, TSchemaPtr>::value_type( mDefaultDtd, mSchema ) );
		}
		else	{
			map<string, TSchemaPtr>::iterator i = mDtds.find( dtd );
			printf("Found an existing DTD\n");
			mSchema = (*i).second;
			mCommentDecl->setSchema( mSchema );
			mDtdParser->setSchema( mSchema );
			mDocTypeDecl->setSchema( mSchema );
			mElementParser->setSchema( mSchema );
		}
		mDefaultDtd = aDtd;
	}
	
}

void SGMLParser	::	showTree( const TNodePtr aNode, int aSpacing )	{
	
	TNodeListPtr children = aNode->getChildNodes();
	int length = children->getLength();
	if ( length != 0 )	{
		for ( int i = 0; i < length; i++ )	{
			TNodePtr child = children->item( i );
			for ( int j = 0; j < aSpacing; j++ )	{
				printf("  ");
			}
			printf("Child name: %s\n", child->getNodeName().c_str());
			if ( child->getNodeType() == ELEMENT_NODE )	{
				// Check for attributes
				TNamedNodeMapPtr attributes = child->getAttributes();
				for ( unsigned int j = 0; j < attributes->getLength(); j++ )	{
					TNodePtr attr = attributes->item( j );
					for ( int j = 0; j < aSpacing + 1; j++ )	{
						printf("  ");
					}
					printf("Attribute %s", attr->getNodeName().c_str());
					printf(" with value %s\n", attr->getNodeValue().c_str());
				}
			}
			showTree( child, aSpacing + 1 );
		}
	}	
}

int main( int argc, char * argv[] )	{
	
	if ( argc < 3 )	{
		printf("Please supply a dtd and a document to load\n");
		return 1;
	}
	
	SGMLParser * sgmlParser = new SGMLParser();
	TDocumentPtr dtd = sgmlParser->parse( argv[ 1 ], argv[ 2 ] );

	delete sgmlParser;
	
	return 0;
	
}
