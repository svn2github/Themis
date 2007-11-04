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
#include "TSchema.hpp"

// DOM headers
#include "TNode.h"
#include "TElement.h"
#include "TNodeList.h"

DTDParser	::	DTDParser( TSchemaPtr aSchema )
			:	BaseParser( aSchema )	{
	
	mDocText = SGMLTextPtr( new SGMLText() );
	
	// Create declaration parsers
	commentDecl =
		new CommentDeclParser( mDocText, mSchema );
	markedSecDecl =
		new MarkedSecDeclParser( mDocText, mSchema );
	entityDecl =
		new EntityDeclParser( mDocText, mSchema );
	elementDecl =
		new ElementDeclParser( mDocText, mSchema );
	attrListDecl =
		new AttrListDeclParser( mDocText, mSchema );

}

DTDParser	::	~DTDParser()	{
	
	//printf( "DTDParser destroyed\n" );
	
	delete commentDecl;
	delete markedSecDecl;
	delete entityDecl;
	delete elementDecl;
	delete attrListDecl;
	
}

void DTDParser	::	setSchema( TSchemaPtr aSchema )	{

	commentDecl->setSchema( mSchema );
	markedSecDecl->setSchema( mSchema );
	entityDecl->setSchema( mSchema );
	elementDecl->setSchema( mSchema );
	attrListDecl->setSchema( mSchema );

	BaseParser::setSchema( mSchema );

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

TSchemaPtr DTDParser	::	parse( const char * aFileName )	{
	
	mDocText->loadText( aFileName );
	
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

	return mDTD;
	
}
