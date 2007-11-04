/*
	Copyright (c) 2006 Mark Hellegers. All Rights Reserved.
	
	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or
	sell copies of the Software, and to permit persons to whom
	the Software is furnished to do so, subject to the following
	conditions:
	
	   The above copyright notice and this permission notice
	   shall be included in all copies or substantial portions
	   of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
	KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
	WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
	OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
	OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
	OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
	
	Original Author: 	Mark Hellegers (mark@firedisk.net)
	Project Start Date: October 18, 2000
	Class Start Date: June 12, 2006
*/

/*
	Schema implementation
	See TSchema.hpp for some more information
*/

// SGMLParser headers
#include "TSchema.hpp"
#include "TElementDeclaration.hpp"
#include "ElementDeclException.hpp"

// DOM headers
#include "TNode.h"
#include "TNodeList.h"
#include "TElement.h"

// Standard C headers
#include <stdio.h>

TSchema::	TSchema()
		:	TDocument()	{

}

TSchema	::	~TSchema()	{
	
}

void TSchema	::	setup()	{

	// Element to store parameter entities
	mParEntities = createElement( "parEntities" );
	appendChild(mParEntities);

	// Element to store character entities
	mCharEntities = createElement( "charEntities" );
	appendChild(mCharEntities);

	// Element to store character entities
	mElements = createElement( "elements" );
	appendChild(mElements);
	
	// Element to store attribute lists.
	mAttrLists = createElement( "attrLists" );
	appendChild(mAttrLists);
	
}

TElementDeclarationPtr	TSchema	::	createElementDeclaration()	{

	TDocumentPtr thisDocument = shared_static_cast<TDocument>( shared_from_this() );
	TElementDeclarationPtr result( new TElementDeclaration( thisDocument ) );

	return result;

}

TElementPtr	TSchema	::	getElements() {

	return mElements;
}

TElementPtr	TSchema	::	getAttrLists() {

	return mAttrLists;
}

TElementPtr	TSchema	::	getCharEntities() {

	return mCharEntities;
}

TElementPtr	TSchema	::	getParEntities() {

	return mParEntities;
}
