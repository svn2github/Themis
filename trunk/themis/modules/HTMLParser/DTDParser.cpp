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

DTDParser	::	DTDParser( const char * aFileName, TDocumentShared aDTD )
					:	BaseParser()	{
	
	printf( "DTDParser constructed\n" );

	// Load text
	mDocText = SGMLTextPtr( new SGMLText() );
	ifstream file( aFileName );
	
	char ch;
	while ( file.get( ch ) )	{
		mDocText->addChar( ch );
	};
	
	mDTD = aDTD;
	
	// Create declaration parsers
	commentDecl =
		new CommentDeclParser( mDocText, mDTD, mParEntities, mCharEntities );
	markedSecDecl =
		new MarkedSecDeclParser( mDocText, mDTD, mParEntities, mCharEntities );
	entityDecl =
		new EntityDeclParser( mDocText, mDTD, mParEntities, mCharEntities );
	elementDecl =
		new ElementDeclParser( mDocText, mDTD, mParEntities, mCharEntities );
	attrListDecl =
		new AttrListDeclParser( mDocText, mDTD, mParEntities, mCharEntities );

}

DTDParser	::	~DTDParser()	{
	
	printf( "DTDParser destroyed\n" );
	
	delete commentDecl;
	delete markedSecDecl;
	delete entityDecl;
	delete elementDecl;
	delete attrListDecl;
	
}

void DTDParser	::	processDeclaration()	{

	State save = mDocText->saveState();

	try	{
		entityDecl->parse( entityDecl->getEntityTexts() );
		//printf( "Entity declaration parsed\n" );
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
		elementDecl->parse( entityDecl->getEntityTexts() );
		//printf( "Element declaration parsed\n" );
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
		attrListDecl->parse( entityDecl->getEntityTexts() );
		//printf( "AttList declaration parsed\n" );
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

	throw ReadException( mDocText->getLineNr(), mDocText->getCharNr(), "Declaration expected" );
	
}

void DTDParser	::	processDs()	{

	try	{
		processS();
		return;
	}
	catch( ReadException r )	{
		if ( r.isFatal() )	{
			throw r;
		}
	}
	try	{
		processEe();
		return;
	}
	catch( ReadException r )	{
		if ( r.isFatal() )	{
			throw r;
		}
	}
	try	{
		processParEntityReference();
		return;
	}
	catch( ReadException r )	{
		if ( r.isFatal() )	{
			throw r;
		}
	}

	State save = mDocText->saveState();

	try	{
		commentDecl->parse();
		//printf( "Comment declaration parsed\n" );
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
		markedSecDecl->parse( entityDecl->getEntityTexts() );
		//printf( "Marked Section declaration parsed\n" );
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

	throw ReadException( mDocText->getLineNr(), mDocText->getCharNr(), "Ds expected" );
	
}

void DTDParser	::	processDsStar()	{

	bool dsFound = true;
	while ( dsFound )	{
		try	{
			processDs();
		}
		catch( ReadException r )	{
			if ( r.isFatal() )	{
				throw r;
			}
			else	{
				dsFound = false;
			}
		}
	}
	
}

TDocumentShared DTDParser	::	parse()	{
	
	while ( true )	{
		try	{
			processDsStar();
			processDeclaration();
		}
		catch( ReadException r )	{
			if ( r.isEof() )	{
				printf( "End of file reached. No errors encountered\n" );
			}
			else	{
				printf( "Error on line %i, char %i, message: %s\n", r.getLineNr(), r.getCharNr(),
						  r.getErrorMessage().c_str() );
			}
			return mDocument;
		}
	}

	return mDocument;
	
}
