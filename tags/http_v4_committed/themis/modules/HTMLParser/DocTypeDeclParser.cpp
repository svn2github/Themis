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
															 TDocumentPtr aDTD )
							:	DeclarationParser( aDocText, aDTD )	{

	//printf( "Constructing DocTypeDeclParser\n" );
	
	mDocTypeName = "";
	
}

DocTypeDeclParser	::	~DocTypeDeclParser()	{

	//printf( "Destroying DocTypeDeclParser\n" );
	
}

bool DocTypeDeclParser	::	processDeclaration()	{
	
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
	
	TElementPtr extId = mDTD->createElement( "externalId" );
	
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
	
	return true;
	
}

string DocTypeDeclParser	::	processDocTypeName()	{
	
	// Is the same as a generic identifier
	return processGI();
	
}

string DocTypeDeclParser	::	getDocTypeName() const	{
	
	return mDocTypeName;
	
}
