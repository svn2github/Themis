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

	try	{
		processPsPlus();
		element = processAssElementType();
		processPsPlus();
		TElementPtr attrDefList = processAttrDefList();
		declaration->appendChild( attrDefList );
		processPsStar();
		process( mMdc );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
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
	
	try	{
		string name = processGI();
		TElementPtr element = mDTD->createElement( name );
		return element;
	}
	catch( ReadException r )	{
		if ( r.isFatal() )	{
			throw r;
		}
	}

	try	{
		TElementPtr group = processNameGroup();
		return group;
	}
	catch( ReadException r )	{
		if ( r.isFatal() )	{
			throw r;
		}
	}
	
	throw ReadException( mDocText->getLineNr(),
									mDocText->getCharNr(),
									"Ass Element type expected" );
	
}

TElementPtr AttrListDeclParser	::	processAttrDefList()	{

	TElementPtr attrDefList = mDTD->createElement( "attributes" );

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
