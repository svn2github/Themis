/*
	Copyright (c) 2002 Mark Hellegers. All Rights Reserved.
	
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
	
	Original Author: 	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	Project Start Date: October 18, 2000
	Class Start Date: February 14, 2002
*/

/*
	Attr implementation
	See TAttr.h for some more information
*/

// Standard C++ headers
#include <string>

// DOM headers
#include "TAttr.h"
#include "TElement.h"

// Namespaces used
using namespace std;

TAttr	::	TAttr( const TDOMString aName,
					  const TDocumentPtr aOwnerDocument,
					  const bool aSpecified,
					  const TDOMString aValue,
					  TElementPtr aOwnerElement )	:
					  		TNode( ATTRIBUTE_NODE, aOwnerDocument, aName, aValue )	{

	mName = aName;
	// Code doesn't fit with the current core specification anymore. Have to check.
	if ( aOwnerElement.get() == NULL )	{
		mSpecified = true;
	}
	else	{
		mSpecified = aSpecified;
	}
	mValue = aValue;
	mDefaultValue = aValue;
	mOwnerElement = aOwnerElement;
	
}

TAttr	::	~TAttr()	{
	
}

TDOMString TAttr	::	getName()	const	{
	
	return mName;
	
}

bool TAttr	::	getSpecified() const	{
	
	return mSpecified;
	
}

TDOMString TAttr	::	getValue() const	{
	
	return mValue;
	
}

void TAttr	::	setValue( const TDOMString aValue )	{
	
	// Partly vague stuff in the DOM spec. Got this part from the mailing list.
	// Add a text node as a child to the attr node. Still to implement.
	
	mValue = aValue;
	setNodeName( aValue );
	mSpecified = true;
	
}


TElementPtr TAttr	::	getOwnerElement()	 const	{
	
	return make_shared( mOwnerElement );
	
}

TDOMString TAttr	::	getDefaultValue() const	{
	
	return mDefaultValue;
	
}
