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
#include "CSSValue.hpp"

CSSStyleDeclaration :: CSSStyleDeclaration(CSSRulePtr aParentRule) {

	mParentRule = aParentRule;

}

CSSStyleDeclaration :: ~CSSStyleDeclaration() {

}

TDOMString CSSStyleDeclaration :: getCSSText() {

	return mCSSText;

}

void CSSStyleDeclaration :: setCSSText(const TDOMString aText) {

	mCSSText = aText;

}

TDOMString CSSStyleDeclaration :: getPropertyValue(const TDOMString aName) {

	TDOMString result;

	if (mValues.count(aName) == 0) {
		result = "";
	}
	else {
		CSSValuePtr value = mValues[aName];
		result = value->getCssText();
	}

	return result;

}

CSSValuePtr CSSStyleDeclaration :: getPropertyCSSValue(const TDOMString aName) {

	return mValues[aName];

}

TDOMString CSSStyleDeclaration :: removeProperty(const TDOMString aName) {

	TDOMString result = getPropertyValue(aName);
	mValues.erase(aName);
	mPriorities.erase(aName);

	return "";

}

TDOMString CSSStyleDeclaration :: getPropertyPriority(const TDOMString aName) {

	return mPriorities[aName];

}

void CSSStyleDeclaration :: setProperty(const TDOMString aName,
										const TDOMString aValue,
										const TDOMString aPriority) {
	
/*
	vector<Property>::iterator iter;
	for (iter = mProperties.begin(); iter != mProperties.end(); iter++) {
		if (iter->getName() == aName) {
			iter->setValue(aValue);
			iter->setPriority(aPriority);
			return;
		}
	}

	Property prop(aName, aValue, aPriority);
	mProperties.push_back(prop);
*/

}
															   
unsigned long CSSStyleDeclaration :: getLength() {

	return mValues.size();

}

TDOMString CSSStyleDeclaration :: item(unsigned long aIndex) {

	TDOMString result;

	if (mValues.size() <= aIndex) {
		result = "";
	}
	else {
		unsigned int i = 0;
		map<string, CSSValuePtr>::iterator it = mValues.begin();
		while (it != mValues.end() && i < aIndex) {
			it++;
			i++;
		}
		if (it != mValues.end()) {
			result = (*it).first;
		}
	}

	return result;

}

CSSRulePtr CSSStyleDeclaration :: getParentRule() {

	return mParentRule;

}

void CSSStyleDeclaration :: setPropertyCSSValue(const TDOMString aName,
												const CSSValuePtr aValue,
												const TDOMString aPriority) {

	if (mValues.count(aName) == 0) {
		mValues.insert(
			map<string, CSSValuePtr>::value_type(aName, aValue));
	}
	else {
		mValues[aName] = aValue;
	}

}
