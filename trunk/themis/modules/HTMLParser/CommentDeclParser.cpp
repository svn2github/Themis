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

void CommentDeclParser	::	processDeclaration()	{
	
	process( mMdo );
	processComment();
	bool moreData = true;
	while ( moreData )	{
		try	{
			processS();
			continue;
		}
		catch( ReadException r )	{
			if ( r.isFatal() )	{
				throw r;
			}
		}
		try	{
			processComment();
		}
		catch( ReadException r )	{
			if ( r.isFatal() )	{
				throw r;
			}
			else	{
				moreData = false;
			}
		}
	}

	try	{
		process( mMdc );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
}
