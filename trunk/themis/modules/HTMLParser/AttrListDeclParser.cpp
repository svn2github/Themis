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
															TDocumentPtr aDTD )
							:	DeclarationParser( aDocText, aDTD )	{

	//printf( "Constructing AttrListDeclParser\n" );
	mAttrLists = mDTD->createElement( "attrlists" );
	mDTD->appendChild( mAttrLists );
	
}

AttrListDeclParser	::	~AttrListDeclParser()	{

	//printf( "Destroying AttrListDeclParser\n" );
	
}

void AttrListDeclParser	::	processDeclaration()	{

	// Define an element to store the attribute list declaration
	TElementPtr attrList = mDTD->createElement( "attrList" );
	
	//process( mMdo );
	process( kATTLIST );

	try	{
		processPsPlus();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
	try	{
		TElementPtr assElementType = processAssElementType();
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
		TElementPtr attrDefList = processAttrDefList();
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

TElementPtr AttrListDeclParser	::	processAssElementType()	{
	
	TElementPtr assElementType = mDTD->createElement( "assElementType" );

	try	{
		string name = processGI();
		TElementPtr elementName = mDTD->createElement( name );
		assElementType->appendChild( elementName );
		//printf( "Element of attribute list: %s\n", name.c_str() );
	}
	catch( ReadException r )	{
		// Not a generic identifier. Must be a name group
		TElementPtr nameGroup = processNameGroup();
		TNodeListPtr children = nameGroup->getChildNodes();
		for ( unsigned int i = 0; i < children->getLength(); i++ )	{
			TNodePtr child = children->item( i );
			TElementPtr element = shared_static_cast<TElement>( child );
			assElementType->appendChild( element );
		}
	}
	
	return assElementType;
	
}

TElementPtr AttrListDeclParser	::	processAttrDefList()	{

	TElementPtr attrDefList = mDTD->createElement( "attrDefList" );

	TElementPtr attrDef = processAttrDef();
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

TElementPtr AttrListDeclParser	::	processAttrDef()	{

	string name = processAttrName();
	TElementPtr attrDef = mDTD->createElement( name );

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

	if ( process( kCDATA, false ) )	{
		return kCDATA;
	}
	if ( process( kENTITY, false ) )	{
		return kENTITY;
	}
	if ( process( kENTITIES, false ) )	{
		return kENTITIES;
	}
	if ( process( kID, false ) )	{
		if ( process( "REF", false ) )	{
			if ( process( "S", false ) )	{
				return kIDREFS;
			}
			else	{
				return kIDREF;
			}
		}
		else	{
			return kID;
		}
	}
	if ( process( kNAME, false ) )	{
		if ( process( "S", false ) )	{
			return kNAMES;
		}
		else	{
			return kNAME;
		}
	}
	if ( process( kNMTOKEN, false ) )	{
		if ( process( "S", false ) )	{
			return kNMTOKENS;
		}
		else	{
			return kNMTOKEN;
		}
	}
	if ( process( kNUMBER, false ) )	{
		if ( process( "S", false ) )	{
			return kNUMBERS;
		}
		else	{
			return kNUMBER;
		}
	}
	if ( process( kNUTOKEN, false ) )	{
		if ( process( "S", false ) )	{
			return kNUTOKENS;
		}
		else	{
			return kNUTOKEN;
		}
	}
	
	try	{
		processNameTokenGroup();
		//printf( "Declared value: name token group\n" );
		return "";
	}
	catch( ReadException r )	{
		// Do nothing
	}

	throw ReadException( mDocText->getLineNr(),
									mDocText->getCharNr(),
									"Declared value expected" );
	
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
	
	if ( process( kFIXED, false ) )	{
		processPsPlus();
		processAttrValueSpec();
		return "";
	}
	if ( process( kREQUIRED, false ) )	{
		return kREQUIRED;
	}
	if ( process( kCURRENT, false ) )	{
		return kCURRENT;
	}
	if ( process( kCONREF, false ) )	{
		return kCONREF;
	}
	if ( process( kIMPLIED, false ) )	{
		return kIMPLIED;
	}

	processAttrValueSpec();
	
	return "";
	
}
