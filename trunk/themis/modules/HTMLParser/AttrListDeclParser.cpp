/*	AttrListDeclParser implementation
	See AttrListDeclParser.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// AttrListDeclParser headers
#include "AttrListDeclParser.hpp"
#include "ReadException.hpp"
#include "SGMLSupport.hpp"

// DOM headers
#include "TDocument.h"
#include "TElement.h"

AttrListDeclParser	::	AttrListDeclParser( SGMLTextPtr aDocText,
															TDocumentShared aDTD,
															TElementShared aParEntities,
															TElementShared aCharEntities )
							:	DeclarationParser( aDocText, aDTD, aParEntities, aCharEntities )	{

	//printf( "Constructing AttrListDeclParser\n" );
	
}

AttrListDeclParser	::	~AttrListDeclParser()	{

	//printf( "Destroying AttrListDeclParser\n" );
	
}

void AttrListDeclParser	::	processDeclaration()	{

	process( mMdo );
	process( kATTLIST );

	try	{
		processPsPlus();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
	try	{
		processAssElementType();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}

	try	{
		processPsPlus();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}

	try	{
		processAttrDefList();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
	try	{
		processPsStar();
	}
	catch( ReadException r )	{
		if ( r.isFatal() )	{
			throw r;
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

void AttrListDeclParser	::	processAssElementType()	{
	
	try	{
		string name = processGI();
		//printf( "Element of attribute list: %s\n", name.c_str() );
	}
	catch( ReadException r )	{
		// Not a generic identifier. Must be a name group
		processNameGroup();
	}
	
}

void AttrListDeclParser	::	processAttrDefList()	{

	processAttrDef();
	
	bool adFound = true;
	while ( adFound )	{
		try	{
			processPsPlus();
			processAttrDef();
		}
		catch( ReadException r )	{
			if ( r.isFatal() )	{
				throw r;
			}
			adFound = false;
		}
	}
	
}

void AttrListDeclParser	::	processAttrDef()	{

	//printf( "processAttrDef: %c %i %i %i\n", mDocText->getChar(), mDocText->getIndex(), mDocText->getLineNr(), mDocText->getCharNr() );

	processAttrName();

	try	{
		processPsPlus();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}

	try	{	
		processDeclValue();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
	try	{
		processPsPlus();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
	try	{	
		processDefValue();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
}

void AttrListDeclParser	::	processAttrName()	{

	// Attribute name and name are equivalent
	string name = processName();
	//printf( "Attribute name: %s\n", name.c_str() );
	
}

void AttrListDeclParser	::	processDeclValue()	{

	try	{
		process( kCDATA );
		//printf( "Declared value: %s\n", mCDATA.c_str() );
		return;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kENTITY );
		//printf( "Declared value: %s\n", kENTITY.c_str() );
		return;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kENTITIES );
		//printf( "Declared value: %s\n", kENTITIES.c_str() );
		return;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kIDREFS );
		//printf( "Declared value: %s\n", kIDREFS.c_str() );
		return;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kIDREF );
		//printf( "Declared value: %s\n", kIDREF.c_str() );
		return;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kID );
		//printf( "Declared value: %s\n", kID.c_str() );
		return;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kNAMES );
		//printf( "Declared value: %s\n", mNAMES.c_str() );
		return;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kNAME );
		//printf( "Declared value: %s\n", mNAME.c_str() );
		return;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kNMTOKENS );
		//printf( "Declared value: %s\n", mNMTOKENS.c_str() );
		return;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kNMTOKEN );
		//printf( "Declared value: %s\n", mNMTOKEN.c_str() );
		return;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kNUMBERS );
		//printf( "Declared value: %s\n", mNUMBERS.c_str() );
		return;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kNUMBER );
		//printf( "Declared value: %s\n", mNUMBER.c_str() );
		return;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kNUTOKENS );
		//printf( "Declared value: %s\n", mNUTOKENS.c_str() );
		return;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kNUTOKEN );
		//printf( "Declared value: %s\n", mNUTOKEN.c_str() );
		return;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		processNameTokenGroup();
		//printf( "Declared value: name token group\n" );
		return;
	}
	catch( ReadException r )	{
		// Do nothing
	}

	throw ReadException( mDocText->getLineNr(), mDocText->getCharNr(), "Declared value expected" );
	
}

void AttrListDeclParser	::	processDefValue()	{
	
	try	{
		process( mRni );
	}
	catch( ReadException r )	{
		// Must be an attribute value specification
		processAttrValueSpec();
		return;
	}
	try	{
		process( kFIXED );
		processPsPlus();
		processAttrValueSpec();
		return;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kREQUIRED );
		return;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kCURRENT );
		return;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kCONREF );
		return;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kIMPLIED );
		return;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	
	processAttrValueSpec();
	
}
