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
																   TDocumentShared aDTD,
																   TElementShared aParEntities,
																   TElementShared aCharEntities )
								:	DeclarationParser( aDocText, aDTD, aParEntities, aCharEntities )	{

	//printf( "Constructing CommentDeclParser\n" );
	
}

CommentDeclParser	::	~CommentDeclParser()	{

	//printf( "Destroying CommentDeclParser\n" );
	
}

void CommentDeclParser	::	processDeclaration()	{
	
	process( mMdo );
	processComment();

	try	{
		process( mMdc );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
}
