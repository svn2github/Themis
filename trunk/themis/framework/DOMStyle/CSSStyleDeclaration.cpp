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
	
	Original Author: 	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	Project Start Date: October 18, 2000
	Class Implementation Start Date: April 07, 2004
*/

/*
	CSSStyleDeclaration implementation.
	See CSSStyleDeclaration.hpp for more information.
*/

// Standard C headers
#include <stdio.h>

// DOM Style headers
#include "CSSStyleDeclaration.hpp"

CSSStyleDeclaration	::	CSSStyleDeclaration()	{

	printf( "Creating CSSPrimitiveValue\n" );
	
}

CSSStyleDeclaration	::	~CSSStyleDeclaration()	{
	
}

TDOMString CSSStyleDeclaration	::	getCssText()	{
	
	return mCssText;
	
}

void CSSStyleDeclaration	::	setCssText( const TDOMString aText )	{

	mCssText = aText;
	
}

TDOMString CSSStyleDeclaration	::	getPropertyValue( const TDOMString aPropertyName )	{
	
	return "";
	
}

CSSValuePtr CSSStyleDeclaration	::	getPropertyCSSValue( const TDOMString aPropertyName )	{
	
	return CSSValuePtr();
	
}

TDOMString CSSStyleDeclaration	::	removeProperty( const TDOMString aPropertyName )	{
	
	return "";
	
}

TDOMString CSSStyleDeclaration	::	getPropertyPriority( const TDOMString aPropertyName )	{

	return "";
	
}

void CSSStyleDeclaration	::	setProperty( const TDOMString aPropertyName,
															   const TDOMString aPropertyValue,
															   const TDOMString aPriority )	{


}
															   
unsigned long CSSStyleDeclaration	::	getLength()	{
	
	return 0;
	
}

TDOMString CSSStyleDeclaration	::	item( unsigned long aIndex )	{
	
	return "";
	
}

CSSRulePtr CSSStyleDeclaration	::	getParentRule()	{
	
	return CSSRulePtr();
	
}
