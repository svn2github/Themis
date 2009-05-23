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

CSSPrimitiveValue	::	CSSPrimitiveValue( const TDOMString aCssText,
															unsigned short aPrimitiveType )
							:	CSSValue( aCssText, CSS_PRIMITIVE )	{

	printf( "Creating CSSPrimitiveValue\n" );

	mPrimitiveType = aPrimitiveType;

}

CSSPrimitiveValue	::	~CSSPrimitiveValue()	{
	
}

unsigned short CSSPrimitiveValue	::	getPrimitiveType() const	{

	return mPrimitiveType;

}

void CSSPrimitiveValue	::	setFloatValue( unsigned short aUnitType,
															  float aFloatValue )	{

	// Keeping it simple for now.
	if ( mPrimitiveType == aUnitType )	{
		mFloatValue = aFloatValue;
	}
	else	{
		throw TDOMException( INVALID_ACCESS_ERR );
	}

}

float CSSPrimitiveValue	::	getFloatValue( unsigned short aUnitType ) const	{
	
	// Keeping it simple for now.
	if ( mPrimitiveType != aUnitType )	{
		throw TDOMException( INVALID_ACCESS_ERR );
	}
	
	return mFloatValue;
	
}

void CSSPrimitiveValue	::	setStringValue( unsigned short aStringType,
															   TDOMString aStringValue )	{

	// Keeping it simple for now.
	if ( mPrimitiveType == aStringType )	{
		mStringValue = aStringValue;
	}
	else	{
		throw TDOMException( INVALID_ACCESS_ERR );
	}

}

TDOMString CSSPrimitiveValue	:: getStringValue()	{

	// Keeping it simple for now.
	if ( mPrimitiveType != CSS_STRING )	{
		throw TDOMException( INVALID_ACCESS_ERR );
	}

	return mStringValue;

}

CounterPtr CSSPrimitiveValue	::	getCounterValue()	{

	// Keeping it simple for now.
	if ( mPrimitiveType != CSS_COUNTER )	{
		throw TDOMException( INVALID_ACCESS_ERR );
	}

	return mCounterValue;

}

RectPtr CSSPrimitiveValue	::	getRectValue()	{

	// Keeping it simple for now.
	if ( mPrimitiveType != CSS_RECT )	{
		throw TDOMException( INVALID_ACCESS_ERR );
	}

	return mRectValue;
	
}

RGBColorPtr CSSPrimitiveValue	::	getRGBColorValue()	{

	// Keeping it simple for now.
	if ( mPrimitiveType != CSS_RGBCOLOR )	{
		throw TDOMException( INVALID_ACCESS_ERR );
	}

	return mRGBColorValue;
	
}
