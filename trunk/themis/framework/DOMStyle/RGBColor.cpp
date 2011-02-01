/*
	Copyright (c) 2001 Mark Hellegers. All Rights Reserved.
	
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
	Class Implementation Start Date: March 09, 2004
*/

/*
	RGBColor implementation
	See RGBColor.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// DOM Style headers
#include "RGBColor.hpp"
#include "CSSPrimitiveValue.hpp"

RGBColor :: RGBColor(CSSPrimitiveValuePtr aRed,
					 CSSPrimitiveValuePtr aGreen,
					 CSSPrimitiveValuePtr aBlue) {

	mRed = aRed;
	mGreen = aGreen;
	mBlue = aBlue;
	
}

RGBColor :: RGBColor(TDOMString aColorName) {
	
	mColorName = aColorName;
	
	if (aColorName == "black") {
		mRed = CSSPrimitiveValuePtr(new CSSPrimitiveValue(0, CSSPrimitiveValue::CSS_NUMBER));
		mGreen = CSSPrimitiveValuePtr(new CSSPrimitiveValue(0, CSSPrimitiveValue::CSS_NUMBER));
		mBlue = CSSPrimitiveValuePtr(new CSSPrimitiveValue(0, CSSPrimitiveValue::CSS_NUMBER));
	}
	else if (aColorName == "silver") {
		mRed = CSSPrimitiveValuePtr(new CSSPrimitiveValue(192, CSSPrimitiveValue::CSS_NUMBER));
		mGreen = CSSPrimitiveValuePtr(new CSSPrimitiveValue(192, CSSPrimitiveValue::CSS_NUMBER));
		mBlue = CSSPrimitiveValuePtr(new CSSPrimitiveValue(192, CSSPrimitiveValue::CSS_NUMBER));
	}
	else if (aColorName == "gray") {
		mRed = CSSPrimitiveValuePtr(new CSSPrimitiveValue(80, CSSPrimitiveValue::CSS_NUMBER));
		mGreen = CSSPrimitiveValuePtr(new CSSPrimitiveValue(80, CSSPrimitiveValue::CSS_NUMBER));
		mBlue = CSSPrimitiveValuePtr(new CSSPrimitiveValue(80, CSSPrimitiveValue::CSS_NUMBER));
	}
	else if (aColorName == "white") {
		mRed = CSSPrimitiveValuePtr(new CSSPrimitiveValue(255, CSSPrimitiveValue::CSS_NUMBER));
		mGreen = CSSPrimitiveValuePtr(new CSSPrimitiveValue(255, CSSPrimitiveValue::CSS_NUMBER));
		mBlue = CSSPrimitiveValuePtr(new CSSPrimitiveValue(255, CSSPrimitiveValue::CSS_NUMBER));
	}
	else if (aColorName == "maroon") {
		mRed = CSSPrimitiveValuePtr(new CSSPrimitiveValue(128, CSSPrimitiveValue::CSS_NUMBER));
		mGreen = CSSPrimitiveValuePtr(new CSSPrimitiveValue(0, CSSPrimitiveValue::CSS_NUMBER));
		mBlue = CSSPrimitiveValuePtr(new CSSPrimitiveValue(0, CSSPrimitiveValue::CSS_NUMBER));
	}
	else if (aColorName == "red") {
		mRed = CSSPrimitiveValuePtr(new CSSPrimitiveValue(255, CSSPrimitiveValue::CSS_NUMBER));
		mGreen = CSSPrimitiveValuePtr(new CSSPrimitiveValue(0, CSSPrimitiveValue::CSS_NUMBER));
		mBlue = CSSPrimitiveValuePtr(new CSSPrimitiveValue(0, CSSPrimitiveValue::CSS_NUMBER));
	}
	else if (aColorName == "purple") {
		mRed = CSSPrimitiveValuePtr(new CSSPrimitiveValue(128, CSSPrimitiveValue::CSS_NUMBER));
		mGreen = CSSPrimitiveValuePtr(new CSSPrimitiveValue(0, CSSPrimitiveValue::CSS_NUMBER));
		mBlue = CSSPrimitiveValuePtr(new CSSPrimitiveValue(128, CSSPrimitiveValue::CSS_NUMBER));
	}
	else if (aColorName == "fuchsia") {
		mRed = CSSPrimitiveValuePtr(new CSSPrimitiveValue(255, CSSPrimitiveValue::CSS_NUMBER));
		mGreen = CSSPrimitiveValuePtr(new CSSPrimitiveValue(0, CSSPrimitiveValue::CSS_NUMBER));
		mBlue = CSSPrimitiveValuePtr(new CSSPrimitiveValue(255, CSSPrimitiveValue::CSS_NUMBER));
	}
	else if (aColorName == "green") {
		mRed = CSSPrimitiveValuePtr(new CSSPrimitiveValue(0, CSSPrimitiveValue::CSS_NUMBER));
		mGreen = CSSPrimitiveValuePtr(new CSSPrimitiveValue(128, CSSPrimitiveValue::CSS_NUMBER));
		mBlue = CSSPrimitiveValuePtr(new CSSPrimitiveValue(0, CSSPrimitiveValue::CSS_NUMBER));
	}
	else if (aColorName == "lime") {
		mRed = CSSPrimitiveValuePtr(new CSSPrimitiveValue(0, CSSPrimitiveValue::CSS_NUMBER));
		mGreen = CSSPrimitiveValuePtr(new CSSPrimitiveValue(255, CSSPrimitiveValue::CSS_NUMBER));
		mBlue = CSSPrimitiveValuePtr(new CSSPrimitiveValue(0, CSSPrimitiveValue::CSS_NUMBER));
	}
	else if (aColorName == "olive") {
		mRed = CSSPrimitiveValuePtr(new CSSPrimitiveValue(128, CSSPrimitiveValue::CSS_NUMBER));
		mGreen = CSSPrimitiveValuePtr(new CSSPrimitiveValue(128, CSSPrimitiveValue::CSS_NUMBER));
		mBlue = CSSPrimitiveValuePtr(new CSSPrimitiveValue(0, CSSPrimitiveValue::CSS_NUMBER));
	}
	else if (aColorName == "yellow") {
		mRed = CSSPrimitiveValuePtr(new CSSPrimitiveValue(255, CSSPrimitiveValue::CSS_NUMBER));
		mGreen = CSSPrimitiveValuePtr(new CSSPrimitiveValue(255, CSSPrimitiveValue::CSS_NUMBER));
		mBlue = CSSPrimitiveValuePtr(new CSSPrimitiveValue(0, CSSPrimitiveValue::CSS_NUMBER));
	}
	else if (aColorName == "navy") {
		mRed = CSSPrimitiveValuePtr(new CSSPrimitiveValue(0, CSSPrimitiveValue::CSS_NUMBER));
		mGreen = CSSPrimitiveValuePtr(new CSSPrimitiveValue(0, CSSPrimitiveValue::CSS_NUMBER));
		mBlue = CSSPrimitiveValuePtr(new CSSPrimitiveValue(128, CSSPrimitiveValue::CSS_NUMBER));
	}
	else if (aColorName == "blue") {
		mRed = CSSPrimitiveValuePtr(new CSSPrimitiveValue(0, CSSPrimitiveValue::CSS_NUMBER));
		mGreen = CSSPrimitiveValuePtr(new CSSPrimitiveValue(0, CSSPrimitiveValue::CSS_NUMBER));
		mBlue = CSSPrimitiveValuePtr(new CSSPrimitiveValue(255, CSSPrimitiveValue::CSS_NUMBER));
	}
	else if (aColorName == "teal") {
		mRed = CSSPrimitiveValuePtr(new CSSPrimitiveValue(0, CSSPrimitiveValue::CSS_NUMBER));
		mGreen = CSSPrimitiveValuePtr(new CSSPrimitiveValue(128, CSSPrimitiveValue::CSS_NUMBER));
		mBlue = CSSPrimitiveValuePtr(new CSSPrimitiveValue(128, CSSPrimitiveValue::CSS_NUMBER));
	}
	else if (aColorName == "aqua") {
		mRed = CSSPrimitiveValuePtr(new CSSPrimitiveValue(0, CSSPrimitiveValue::CSS_NUMBER));
		mGreen = CSSPrimitiveValuePtr(new CSSPrimitiveValue(255, CSSPrimitiveValue::CSS_NUMBER));
		mBlue = CSSPrimitiveValuePtr(new CSSPrimitiveValue(255, CSSPrimitiveValue::CSS_NUMBER));
	}

}

RGBColor	::	~RGBColor()	{

}

CSSPrimitiveValuePtr RGBColor	::	getRed() const	{

	return mRed;
	
}

CSSPrimitiveValuePtr RGBColor	::	getGreen() const	{

	return mGreen;
	
}

CSSPrimitiveValuePtr RGBColor	::	getBlue() const	{

	return mBlue;
	
}
