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

CSSStyleDeclaration	::	CSSStyleDeclaration( CSSRulePtr aParentRule )	{

	printf( "Creating CSSPrimitiveValue\n" );
	
	mParentRule = aParentRule;
	
}

CSSStyleDeclaration	::	~CSSStyleDeclaration()	{
	
}

TDOMString CSSStyleDeclaration	::	getCssText()	{
	
	return mCssText;
	
}

void CSSStyleDeclaration	::	setCssText( const TDOMString aText )	{

	mCssText = aText;
	
}

TDOMString CSSStyleDeclaration	::	getPropertyValue( const TDOMString aName )	{
	
	vector<Property>::iterator iter;
	for ( iter = mProperties.begin(); iter != mProperties.end(); iter++ )	{
		if ( iter->getName() == aName )	{
				return iter->getValue();
		}
	}
	
	return "";
	
}

CSSValuePtr CSSStyleDeclaration	::	getPropertyCSSValue( const TDOMString aName )	{
	
	return CSSValuePtr();
	
}

TDOMString CSSStyleDeclaration	::	removeProperty( const TDOMString aName )	{
	
	vector<Property>::iterator iter;
	for ( iter = mProperties.begin(); iter != mProperties.end(); iter++ )	{
		if ( iter->getName() == aName )	{
				TDOMString value = iter->getValue();
				mProperties.erase( iter );
				return value;
		}
	}
	
	return "";
	
}

TDOMString CSSStyleDeclaration	::	getPropertyPriority( const TDOMString aName )	{

	vector<Property>::iterator iter;
	for ( iter = mProperties.begin(); iter != mProperties.end(); iter++ )	{
		if ( iter->getName() == aName )	{
				return iter->getPriority();
		}
	}
	
	return "";
	
}

void CSSStyleDeclaration	::	setProperty( const TDOMString aName,
															   const TDOMString aValue,
															   const TDOMString aPriority )	{

	vector<Property>::iterator iter;
	for ( iter = mProperties.begin(); iter != mProperties.end(); iter++ )	{
		if ( iter->getName() == aName )	{
				iter->setValue( aValue );
				iter->setPriority( aPriority );
				return;
		}
	}
	
	Property prop( aName, aValue, aPriority );
	mProperties.push_back( prop );

}
															   
unsigned long CSSStyleDeclaration	::	getLength()	{
	
	return mProperties.size();
	
}

TDOMString CSSStyleDeclaration	::	item( unsigned long aIndex )	{
	
	if ( mProperties.size() <= aIndex )	{
		return "";
	}
	
	return mProperties[ aIndex ].getName();
	
}

CSSRulePtr CSSStyleDeclaration	::	getParentRule()	{
	
	return mParentRule;
	
}
