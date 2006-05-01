/*	DTDParser implementation
	See DTDParser.hpp for more information
	
	Also includes main()
*/

// Standard C headers
#include <stdio.h>

// Standard C++ headers
#include <fstream>

// DTDParser headers
#include "DTDParser.hpp"
#include "ReadException.hpp"
#include "State.hpp"
#include "SGMLSupport.hpp"
#include "CommentDeclParser.hpp"
#include "MarkedSecDeclParser.hpp"
#include "EntityDeclParser.hpp"
#include "ElementDeclParser.hpp"
#include "AttrListDeclParser.hpp"

// DOM headers
#include "TNode.h"
#include "TElement.h"
#include "TNodeList.h"

DTDParser	::	DTDParser( const char * aFileName, TDocumentPtr aDTD )
					:	BaseParser()	{
	
	//printf( "DTDParser constructed\n" );

	// Load text
	mDocText = SGMLTextPtr( new SGMLText() );
	mDocText->loadText( aFileName );
		
	mDTD = aDTD;
	
	// Create declaration parsers
	commentDecl =
		new CommentDeclParser( mDocText, mDTD );
	markedSecDecl =
		new MarkedSecDeclParser( mDocText, mDTD );
	entityDecl =
		new EntityDeclParser( mDocText, mDTD );
	elementDecl =
		new ElementDeclParser( mDocText, mDTD );
	attrListDecl =
		new AttrListDeclParser( mDocText, mDTD );

}

DTDParser	::	~DTDParser()	{
	
	//printf( "DTDParser destroyed\n" );
	
	delete commentDecl;
	delete markedSecDecl;
	delete entityDecl;
	delete elementDecl;
	delete attrListDecl;
	
}

void DTDParser	::	setDTD( TDocumentPtr aDTD )	{
	
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
	}
	
	commentDecl->setDTD( mDTD );
	markedSecDecl->setDTD( mDTD );
	entityDecl->setDTD( mDTD );
	elementDecl->setDTD( mDTD );
	attrListDecl->setDTD( mDTD );

	mEntityTexts.clear();
	
}

bool DTDParser	::	processDeclaration()	{

	if ( ! process( mMdo, false ) )	{
		return false;
	}

	State save = mDocText->saveState();

	if ( entityDecl->parse( entityDecl->getEntityTexts() ) )	{
		return true;
	}
	if ( elementDecl->parse( entityDecl->getEntityTexts() ) )	{
		return true;
	}
	if ( attrListDecl->parse( entityDecl->getEntityTexts() ) )	{
		return true;
	}

	return false;
	
}

bool DTDParser	::	processDs()	{

	if ( processS() )	{
		return true;
	}
	if ( processEe() )	{
		return true;
	}

	if ( processParEntityReference() )	{
		return true;
	}

	State save = mDocText->saveState();

	if ( commentDecl->parse() )	{
		return true;
	}
	else	{
		mDocText->restoreState( save );
	}
	if ( markedSecDecl->parse( entityDecl->getEntityTexts() ) )	{
		//printf( "Marked Section declaration parsed\n" );
		return true;
	}
	else	{
		mDocText->restoreState( save );
	}

	return false;
	
}

void DTDParser	::	processDsStar()	{

	bool dsFound = true;
	while ( dsFound )	{
		dsFound = processDs();
	}

}

TDocumentPtr DTDParser	::	parse()	{
	
	bool contentFound = true;
	
	while ( contentFound )	{
		try	{
			processDsStar();
			if ( ! processDeclaration() )	{
				throw ReadException( mDocText->getLineNr(),
												mDocText->getCharNr(),
												"Declaration expected",
												GENERIC, true );
			}
		}
		catch( ReadException r )	{
			if ( r.isEof() )	{
				printf( "End of DTD file reached. No errors encountered\n" );
			}
			else	{
				printf( "Error on line %i, char %i, message: %s\n", r.getLineNr(), r.getCharNr(),
						  r.getErrorMessage().c_str() );
			}
			contentFound = false;
		}
	}

	return mDocument;
	
}

TDocumentPtr DTDParser	::	parse( const char * aFileName )	{
	
	mDocText->loadText( aFileName );
	
	return parse();
	
}
