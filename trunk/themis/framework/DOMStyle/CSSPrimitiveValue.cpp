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
	Class Implementation Start Date: March 07, 2004
*/

/*
	CSSPrimitiveValue implementation.
	See CSSPrimitiveValue.hpp for more information.
*/

// Standard C headers
#include <stdio.h>

// DOM Style headers
#include "CSSPrimitiveValue.hpp"
#include "Counter.hpp"
#include "Rect.hpp"
#include "RGBColor.hpp"

// DOM Core headers
#include "TDOMException.h"

CSSPrimitiveValue :: CSSPrimitiveValue(const TDOMString aValue,
									   unsigned short aPrimitiveType )
				  : CSSValue(aValue, CSS_PRIMITIVE) {

	mPrimitiveType = aPrimitiveType;
	mStringValue = aValue;

}

CSSPrimitiveValue :: CSSPrimitiveValue(float aValue,
									   unsigned short aUnitType)
				  : CSSValue(computeCssString(aValue, aUnitType), CSS_PRIMITIVE) {


	mFloatValue = aValue;
	mPrimitiveType = aUnitType;

}

CSSPrimitiveValue :: CSSPrimitiveValue(CounterPtr aValue)
				  : CSSValue("Counter", CSS_PRIMITIVE) {


	mCounterValue = aValue;
	mPrimitiveType = CSS_COUNTER;

}

CSSPrimitiveValue :: CSSPrimitiveValue(RectPtr aValue)
				  : CSSValue("Rect", CSS_PRIMITIVE) {


	mRectValue = aValue;
	mPrimitiveType = CSS_RECT;

}

CSSPrimitiveValue :: CSSPrimitiveValue(RGBColorPtr aValue)
				  : CSSValue("RGBColor", CSS_PRIMITIVE) {


	mRGBColorValue = aValue;
	mPrimitiveType = CSS_RGBCOLOR;

}

CSSPrimitiveValue	::	~CSSPrimitiveValue()	{

}

TDOMString CSSPrimitiveValue :: computeCssString(float aValue, unsigned short aUnitType) {

	TDOMString result = "";
	char floatString[10];
	snprintf(floatString, sizeof(floatString), "%g", aValue);
	result = floatString;
	
	switch(aUnitType) {
		case CSS_NUMBER: {
			// nothing to be done
			break;
		}
		case CSS_PERCENTAGE: {
			result += "%";
			break;
		}
		case CSS_EMS: {
			result += "em";
			break;
		}
		case CSS_EXS: {
			result += "ex";
			break;
		}
		case CSS_PX: {
			result += "px";
			break;
		}
		case CSS_CM: {
			result += "cm";
			break;
		}
		case CSS_MM: {
			result += "mm";
			break;
		}
		case CSS_IN: {
			result += "in";
			break;
		}
		case CSS_PT: {
			result += "pt";
			break;
		}
		case CSS_PC: {
			result += "pc";
			break;
		}
		case CSS_DEG: {
			result += "deg";
			break;
		}
		case CSS_RAD: {
			result += "rad";
			break;
		}
		case CSS_GRAD: {
			result += "grad";
			break;
		}
		case CSS_MS: {
			result += "ms";
			break;
		}
		case CSS_S: {
			result += "s";
			break;
		}
		case CSS_HZ: {
			result += "hz";
			break;
		}
		case CSS_KHZ: {
			result += "khz";
			break;
		}
	}

	return result;
	
}

unsigned short CSSPrimitiveValue :: getPrimitiveType() const {

	return mPrimitiveType;

}

void CSSPrimitiveValue :: setFloatValue(unsigned short aUnitType,
										float aFloatValue ) {

	// Keeping it simple for now.
	if (mPrimitiveType == aUnitType) {
		mFloatValue = aFloatValue;
	}
	else {
		throw TDOMException(INVALID_ACCESS_ERR);
	}

}

float CSSPrimitiveValue	::	getFloatValue(unsigned short aUnitType) const {
	
	// Keeping it simple for now.
	if (mPrimitiveType != aUnitType) {
		throw TDOMException(INVALID_ACCESS_ERR);
	}

	return mFloatValue;

}

void CSSPrimitiveValue :: setStringValue(unsigned short aStringType,
										 TDOMString aStringValue) {

	// Keeping it simple for now.
	if (mPrimitiveType == aStringType) {
		mStringValue = aStringValue;
	}
	else {
		throw TDOMException(INVALID_ACCESS_ERR);
	}

}

TDOMString CSSPrimitiveValue :: getStringValue() {

	// Keeping it simple for now.
	if (mPrimitiveType != CSS_STRING &&
		mPrimitiveType != CSS_URI &&
		mPrimitiveType != CSS_IDENT &&
		mPrimitiveType != CSS_ATTR) {
		throw TDOMException(INVALID_ACCESS_ERR);
	}

	return mStringValue;

}

CounterPtr CSSPrimitiveValue :: getCounterValue() {

	// Keeping it simple for now.
	if (mPrimitiveType != CSS_COUNTER) {
		throw TDOMException(INVALID_ACCESS_ERR);
	}

	return mCounterValue;

}

RectPtr CSSPrimitiveValue :: getRectValue() {

	// Keeping it simple for now.
	if (mPrimitiveType != CSS_RECT) {
		throw TDOMException(INVALID_ACCESS_ERR);
	}

	return mRectValue;
	
}

RGBColorPtr CSSPrimitiveValue :: getRGBColorValue() {

	// Keeping it simple for now.
	if (mPrimitiveType != CSS_RGBCOLOR) {
		throw TDOMException(INVALID_ACCESS_ERR);
	}

	return mRGBColorValue;
	
}
