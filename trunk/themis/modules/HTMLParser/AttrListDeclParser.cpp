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

// BeOS headers (TEMPORARY)
//#include <be/kernel/OS.h>

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

//	bigtime_t start_t, end_t;
	
//	start_t = system_time();

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
	
//	end_t = system_time();
	
//	bigtime_t diff_t = end_t - start_t;
//	printf( "Time taken processDeclaration: %d\n", (int) diff_t );
	
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

//	bigtime_t start_t, end_t;
	
//	start_t = system_time();

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

//	end_t = system_time();
	
//	bigtime_t diff_t = end_t - start_t;
//	printf( "Time taken processAttrDefList: %d\n", (int) diff_t );
	
	return attrDefList;
	
}

TElementShared AttrListDeclParser	::	processAttrDef()	{

//	bigtime_t start_t, end_t;
	
//	start_t = system_time();

//	bigtime_t startval_t, endval_t;
	
//	startval_t = system_time();

	string name = processAttrName();
	TElementShared attrDef = mDTD->createElement( name );

//	endval_t = system_time();

//	bigtime_t diffval_t = endval_t - startval_t;
//	printf( "Time taken processAttrName: %d\n", (int) diffval_t );

//	startval_t = system_time();

	try	{
		processPsPlus();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}

//	endval_t = system_time();
	
//	diffval_t = endval_t - startval_t;
//	printf( "Time taken processPsPlus: %d\n", (int) diffval_t );

//	startval_t = system_time();

	try	{	
		string declValue = processDeclValue();
		attrDef->setAttribute( "declValue", declValue );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}

//	endval_t = system_time();
	
//	diffval_t = endval_t - startval_t;
//	printf( "Time taken processDeclValue: %d\n", (int) diffval_t );
	
	try	{
		processPsPlus();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
//	startval_t = system_time();

	try	{	
		string defValue = processDefValue();
		attrDef->setAttribute( "defValue", defValue );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}

//	endval_t = system_time();
	
//	diffval_t = endval_t - startval_t;
//	printf( "Time taken processDefValue: %d\n", (int) diffval_t );
	
//	end_t = system_time();
	
//	bigtime_t diff_t = end_t - start_t;
//	printf( "Time taken processAttrDef: %d\n", (int) diff_t );

	return attrDef;
	
}

string AttrListDeclParser	::	processAttrName()	{

	// Attribute name and name are equivalent
	return processName();
	
}

string AttrListDeclParser	::	processDeclValue()	{

	switch( mDocText->getChar() )	{
		case 'C':	{
			try	{
				process( kCDATA );
				//printf( "Declared value: %s\n", mCDATA.c_str() );
				return kCDATA;
			}
			catch( ReadException r )	{
				break;
			}
		}
		case 'E':	{
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
				break;
			}
		}
		case 'I':	{
			try	{
				process( kID );
				//printf( "Declared value: %s\n", kID.c_str() );
				try	{
					process( "REF" );
					//printf( "Declared value: %s\n", kIDREF.c_str() );
					try	{
						process( "S" );
						//printf( "Declared value: %s\n", kIDREFS.c_str() );
						return kIDREFS;
					}
					catch( ReadException r )	{
						return kIDREF;
					}
				}
				catch( ReadException r )	{
					return kID;
				}
			}
			catch( ReadException r )	{
				break;
			}
		}
		case 'N':	{
			try	{
				process( kNAME );
				//printf( "Declared value: %s\n", mNAME.c_str() );
				try	{
					process( "S" );
					//printf( "Declared value: %s\n", mNAMES.c_str() );
					return kNAMES;
				}
				catch( ReadException r )	{
					return kNAME;
				}
			}
			catch( ReadException r )	{
				// Do nothing
			}
			try	{
				process( kNMTOKEN );
				//printf( "Declared value: %s\n", mNMTOKEN.c_str() );
				try	{
					process( "S" );
					//printf( "Declared value: %s\n", mNMTOKENS.c_str() );
					return kNMTOKENS;
				}
				catch( ReadException r )	{
					return kNMTOKEN;
				}
			}
			catch( ReadException r )	{
				// Do nothing
			}
			try	{
				process( kNUMBER );
				//printf( "Declared value: %s\n", mNUMBER.c_str() );
				try	{
					process( "S" );
					//printf( "Declared value: %s\n", mNUMBERS.c_str() );
					return kNUMBERS;
				}
				catch( ReadException r )	{
					return kNUMBER;
				}
			}
			catch( ReadException r )	{
				// Do nothing
			}
			try	{
				process( kNUTOKEN );
				//printf( "Declared value: %s\n", mNUTOKEN.c_str() );
				try	{
					process( "S" );
					//printf( "Declared value: %s\n", mNUTOKENS.c_str() );
					return kNUTOKENS;
				}
				catch( ReadException r )	{
					return kNUTOKEN;
				}
			}
			catch( ReadException r )	{
				break;
			}
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

/*
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
*/

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
	
	switch( mDocText->getChar() )	{
		case 'F':	{
			try	{
				process( kFIXED );
				processPsPlus();
				processAttrValueSpec();
				return "";
			}
			catch( ReadException r )	{
				break;
			}
		}
		case 'R':	{
			try	{
				process( kREQUIRED );
				return kREQUIRED;
			}
			catch( ReadException r )	{
				// Do nothing
			}
		}
		case 'C':	{
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
				break;
			}
		}
		case 'I':	{
			try	{
				process( kIMPLIED );
				return kIMPLIED;
			}
			catch( ReadException r )	{
				break;
			}
		}
	}

/*
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
*/

	processAttrValueSpec();
	
	return "";
	
}
