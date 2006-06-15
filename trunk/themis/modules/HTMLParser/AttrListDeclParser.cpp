/*	AttrListDeclParser implementation
	See AttrListDeclParser.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// AttrListDeclParser headers
#include "AttrListDeclParser.hpp"
#include "ReadException.hpp"
#include "SGMLSupport.hpp"
#include "TSchema.hpp"

// DOM headers
#include "TElement.h"
#include "TNodeList.h"

AttrListDeclParser	::	AttrListDeclParser( SGMLTextPtr aDocText,
															TSchemaPtr aDTD )
							:	DeclarationParser( aDocText, aDTD )	{

	mAttrLists = mDTD->createElement( "attrLists" );
	mDTD->appendChild( mAttrLists );
	
}

AttrListDeclParser	::	~AttrListDeclParser()	{

}

bool AttrListDeclParser	::	processDeclaration()	{

	// Define an element to store the attribute list declaration
	TElementPtr declaration = mDTD->createElement( "declaration" );
	TElementPtr element;
	
	//process( mMdo );
	if ( ! process( kATTLIST, false ) )	{
		return false;
	}

	processPsPlus();
	element = processAssElementType();
	if ( element.get() == NULL )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(),
										"Attribute element type expected",
										GENERIC, true );
	}
	processPsPlus();
	TElementPtr attrDefList = processAttrDefList();
	declaration->appendChild( attrDefList );
	processPsStar();
	if ( ! process( mMdc, false ) )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(),
										"AttrList not closed correctly",
										GENERIC, true );
	}
		

	if ( element->hasChildNodes() )	{
		// Name group.
		declaration->appendChild( element );
	}
	else	{
		TElementPtr elements = mDTD->createElement( "elements" );
		declaration->appendChild( elements );
		elements->appendChild( element );
	}

	mAttrLists->appendChild( declaration );

	return true;

}

TElementPtr AttrListDeclParser	::	processAssElementType()	{

	string name = processGI( false );
	if ( name == "" )	{
		TElementPtr group = processNameGroup();
		if ( group.get() != NULL )	{
			return group;
		}
	}
	else	{
		TElementPtr element = mDTD->createElement( name );
		return element;
	}
	
	return TElementPtr();
		
}

TElementPtr AttrListDeclParser	::	processAttrDefList()	{

	TElementPtr attrDefList = mDTD->createElement( "attributes" );

	TElementPtr attrDef = processAttrDef();
	attrDefList->appendChild( attrDef );
	
	bool adFound = true;
	while ( adFound )	{
		processPsPlus();
		attrDef = processAttrDef();
		if ( attrDef.get() == NULL )	{
			adFound = false;
		}
		else	{
			attrDefList->appendChild( attrDef );
		}
	}

	return attrDefList;
	
}

TElementPtr AttrListDeclParser	::	processAttrDef()	{

	string name = processAttrName();
	if ( name == "" )	{
		return TElementPtr();
	}

	try	{
		processPsPlus();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}

	string declValue = processDeclValue();
	if ( declValue == "" )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(),
										"Declared value expected",
										GENERIC, true );
	}
	TElementPtr attrDef = mDTD->createElement( name );
	attrDef->setAttribute( "declValue", declValue );

	try	{
		processPsPlus();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
	string defValue = processDefValue();
	if ( defValue == "" )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(),
										"Default value expected",
										GENERIC, true );
	}
	attrDef->setAttribute( "defValue", defValue );

	return attrDef;
	
}

string AttrListDeclParser	::	processAttrName()	{

	// Attribute name and name are equivalent
	return processName( false );
	
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
	
	if ( processNameTokenGroup() != "" )	{
		//printf( "Declared value: name token group\n" );
		return "nameTokenGroup";
	}

	return "";
	
}

string AttrListDeclParser	::	processDefValue()	{
	
	if ( ! process( mRni, false ) )	{
		// Must be an attribute value specification
		return processAttrValueSpec( false );
	}
	
	if ( process( kFIXED, false ) )	{
		processPsPlus();
		return processAttrValueSpec( false );
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

	return processAttrValueSpec( false );
	
}
