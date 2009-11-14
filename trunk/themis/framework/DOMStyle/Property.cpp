/*
	Copyright (c) 2004 Mark Hellegers. All Rights Reserved.
	
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
	Class Start Date: March 18, 2004
*/

/*
	Property implementation.
	See Property.hpp for more information.
*/

// Standard C headers
#include <stdio.h>

// DOM Style headers
#include "Property.hpp"

Property :: Property(TDOMString aName,TDOMString aValue, TDOMString aPriority) {

	mName = aName;
	mValue = aValue;
	mPriority = aPriority;

}

Property :: ~Property() {

}

TDOMString Property :: getName() const {

	return mName;

}

TDOMString Property :: getValue() const {

	return mValue;

}

TDOMString Property :: getPriority() const {

	return mPriority;

}

void Property :: setValue(TDOMString aValue) {

	mValue = aValue;

}

void Property :: setPriority(TDOMString aPriority) {

	mPriority = aPriority;

}
