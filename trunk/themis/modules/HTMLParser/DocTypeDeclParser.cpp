/*	DocTypeDeclParser implementation
	See DocTypeDeclParser.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// SGMLParser headers
#include "DocTypeDeclParser.hpp"
#include "ReadException.hpp"
#include "SGMLSupport.hpp"

DocTypeDeclParser	::	DocTypeDeclParser( SGMLTextPtr aDocText,
															 TDocumentShared aDTD,
															 TElementShared aParEntities,
															  TElementShared aCharEntities )
							:	DeclarationParser( aDocText, aDTD, aParEntities, aCharEntities )	{

	printf( "Constructing DocTypeDeclParser\n" );
	
	mDocTypeName = "";
	
}

DocTypeDeclParser	::	~DocTypeDeclParser()	{

	printf( "Destroying DocTypeDeclParser\n" );
	
}

void DocTypeDeclParser	::	processDeclaration()	{
	
	process( mMdo );
	process( kDOCTYPE );

	try	{
		processPsPlus();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
	try	{
		mDocTypeName = processDocTypeName();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
	TElementShared extId = mDTD->createElement( "externalId" );
	
	try	{
		processPsPlus();
		processExternalId( extId );
	}
	catch( ReadException r )	{
		if ( r.isFatal() )	{
			throw r;
		}
		// Is optional
	}

	// Leaving out document type declaration subset
	
	try	{
		process( mMdc );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
}

string DocTypeDeclParser	::	processDocTypeName()	{
	
	// Is the same as a generic identifier
	return processGI();
	
}

string DocTypeDeclParser	::	getDocTypeName() const	{
	
	return mDocTypeName;
	
}
