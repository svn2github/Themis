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
	
	process( mMdo );
	if ( ! processComment() )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(),
										"Comment expected" );
	}
	bool moreData = true;
	while ( moreData )	{
		if ( processS( false ) )	{
			continue;
		}
		if ( ! processComment() )	{
			moreData = false;
		}
	}

	try	{
		process( mMdc );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
	return true;
	
}
