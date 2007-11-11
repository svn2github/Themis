/*	DocTypeDeclParser implementation
	See DocTypeDeclParser.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// SGMLParser headers
#include "DocTypeDeclParser.hpp"
#include "ReadException.hpp"
#include "SGMLSupport.hpp"
#include "TSchema.hpp"

DocTypeDeclParser	::	DocTypeDeclParser( SGMLTextPtr aDocText,
															 TSchemaPtr aDTD )
							:	DeclarationParser( aDocText, aDTD )	{

	//printf( "Constructing DocTypeDeclParser\n" );
	
	mDocTypeName = "";
	
}

DocTypeDeclParser	::	~DocTypeDeclParser()	{

	//printf( "Destroying DocTypeDeclParser\n" );
	
}

bool DocTypeDeclParser	::	processDeclaration()	{
	
	bool result = process( mMdo, false );
	if (result) {
		result = process( kDOCTYPE, false );
	
		if (result) {
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
			
			TElementPtr extId = mSchema->createElement( "externalId" );
			
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
	}
	
	return result;
	
}

string DocTypeDeclParser	::	processDocTypeName()	{
	
	// Is the same as a generic identifier
	return processGI();
	
}

string DocTypeDeclParser	::	getDocTypeName() const	{
	
	return mDocTypeName;
	
}
