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
#include "TNodeList.h"

AttrListDeclParser	::	AttrListDeclParser( SGMLTextPtr aDocText,
															TDocumentShared aDTD,
															TElementShared aParEntities,
															TElementShared aCharEntities )
							:	DeclarationParser( aDocText, aDTD, aParEntities, aCharEntities )	{

	//printf( "Constructing AttrListDeclParser\n" );
	mAttrLists = mDTD->createElement( "attrlists" );
	mDTD->appendChild( mAttrLists );
	
}

AttrListDeclParser	::	~AttrListDeclParser()	{

	//printf( "Destroying AttrListDeclParser\n" );
	
}

void AttrListDeclParser	::	processDeclaration()	{

	// Define an element to store the attribute list declaration
	TElementShared attrList = mDTD->createElement( "attrList" );
	
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
		TElementShared assElementType = processAssElementType();
		attrList->appendChild( assElementType );
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
		TElementShared attrDefList = processAttrDefList();
		attrList->appendChild( attrDefList );
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

TElementShared AttrListDeclParser	::	processAssElementType()	{
	
	TElementShared assElementType = mDTD->createElement( "assElementType" );

	try	{
		string name = processGI();
		TElementShared elementName = mDTD->createElement( name );
		assElementType->appendChild( elementName );
		//printf( "Element of attribute list: %s\n", name.c_str() );
	}
	catch( ReadException r )	{
		// Not a generic identifier. Must be a name group
		TElementShared nameGroup = processNameGroup();
		TNodeListShared children = nameGroup->getChildNodes();
		for ( unsigned int i = 0; i < children->getLength(); i++ )	{
			TNodeShared child = make_shared( children->item( i ) );
			TElementShared element = shared_static_cast<TElement>( child );
			assElementType->appendChild( element );
		}
	}
	
	return assElementType;
	
}

TElementShared AttrListDeclParser	::	processAttrDefList()	{

	TElementShared attrDefList = mDTD->createElement( "attrDefList" );

	TElementShared attrDef = processAttrDef();
	attrDefList->appendChild( attrDef );
	
	bool adFound = true;
	while ( adFound )	{
		try	{
			processPsPlus();
			attrDef = processAttrDef();
			attrDefList->appendChild( attrDef );
		}
		catch( ReadException r )	{
			if ( r.isFatal() )	{
				throw r;
			}
			adFound = false;
		}
	}
	
	return attrDefList;
	
}

TElementShared AttrListDeclParser	::	processAttrDef()	{

	string name = processAttrName();
	TElementShared attrDef = mDTD->createElement( name );

	try	{
		processPsPlus();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}

	try	{	
		string declValue = processDeclValue();
		attrDef->setAttribute( "declValue", declValue );
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
		string defValue = processDefValue();
		attrDef->setAttribute( "defValue", defValue );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
	return attrDef;
	
}

string AttrListDeclParser	::	processAttrName()	{

	// Attribute name and name are equivalent
	return processName();
	
}

string AttrListDeclParser	::	processDeclValue()	{

	try	{
		process( kCDATA );
		//printf( "Declared value: %s\n", mCDATA.c_str() );
		return kCDATA;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kENTITY );
		//printf( "Declared value: %s\n", kENTITY.c_str() );
		return kENTITY;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kENTITIES );
		//printf( "Declared value: %s\n", kENTITIES.c_str() );
		return kENTITIES;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kIDREFS );
		//printf( "Declared value: %s\n", kIDREFS.c_str() );
		return kIDREFS;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kIDREF );
		//printf( "Declared value: %s\n", kIDREF.c_str() );
		return kIDREF;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kID );
		//printf( "Declared value: %s\n", kID.c_str() );
		return kID;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kNAMES );
		//printf( "Declared value: %s\n", mNAMES.c_str() );
		return kNAMES;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kNAME );
		//printf( "Declared value: %s\n", mNAME.c_str() );
		return kNAME;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kNMTOKENS );
		//printf( "Declared value: %s\n", mNMTOKENS.c_str() );
		return kNMTOKENS;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kNMTOKEN );
		//printf( "Declared value: %s\n", mNMTOKEN.c_str() );
		return kNMTOKEN;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kNUMBERS );
		//printf( "Declared value: %s\n", mNUMBERS.c_str() );
		return kNUMBERS;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kNUMBER );
		//printf( "Declared value: %s\n", mNUMBER.c_str() );
		return kNUMBER;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kNUTOKENS );
		//printf( "Declared value: %s\n", mNUTOKENS.c_str() );
		return kNUTOKENS;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kNUTOKEN );
		//printf( "Declared value: %s\n", mNUTOKEN.c_str() );
		return kNUTOKEN;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		processNameTokenGroup();
		//printf( "Declared value: name token group\n" );
		return "";
	}
	catch( ReadException r )	{
		// Do nothing
	}

	throw ReadException( mDocText->getLineNr(), mDocText->getCharNr(), "Declared value expected" );
	
}

string AttrListDeclParser	::	processDefValue()	{
	
	try	{
		process( mRni );
	}
	catch( ReadException r )	{
		// Must be an attribute value specification
		processAttrValueSpec();
		return "";
	}
	
	try	{
		process( kFIXED );
		processPsPlus();
		processAttrValueSpec();
		return "";
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kREQUIRED );
		return kREQUIRED;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kCURRENT );
		return kCURRENT;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kCONREF );
		return kCONREF;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		process( kIMPLIED );
		return kIMPLIED;
	}
	catch( ReadException r )	{
		// Do nothing
	}

	processAttrValueSpec();
	
	return "";
	
}
