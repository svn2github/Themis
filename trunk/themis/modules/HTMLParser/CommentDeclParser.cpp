/*	CommentDeclParser implementation
	See CommentDeclParser.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// SGMLParser headers
#include "CommentDeclParser.hpp"
#include "ReadException.hpp"
#include "SGMLSupport.hpp"

CommentDeclParser	::	CommentDeclParser( SGMLTextPtr aDocText,
																   TDocumentPtr aDTD )
								:	DeclarationParser( aDocText, aDTD )	{

	//printf( "Constructing CommentDeclParser\n" );
	
}

CommentDeclParser	::	~CommentDeclParser()	{

	//printf( "Destroying CommentDeclParser\n" );
	
}

bool CommentDeclParser	::	processDeclaration()	{

	State save = mDocText->saveState();

	if ( ! process( mMdo, false ) )	{
		return false;
	}
	if ( ! processComment() )	{
		mDocText->restoreState( save );
		return false;
	}
	bool moreData = true;
	while ( moreData )	{
		if ( processS() )	{
			continue;
		}
		if ( ! processComment() )	{
			moreData = false;
		}
	}

	if ( ! process( mMdc, false ) )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(),
										"Comment declaration not closed correctly",
										GENERIC, true );
	}		
	
	return true;
	
}
