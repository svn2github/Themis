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
	Class Implementation Start Date: March 14, 2004
*/

/*
	CSSRule implementation.
	See CSSRule.hpp for more information.
*/

// Standard C headers
#include <stdio.h>

// DOM Style headers
#include "CSSRule.hpp"
#include "CSSStyleSheet.hpp"

CSSRule :: CSSRule(CSSStyleSheetPtr aParentStyleSheet,
				   CSSRulePtr aParentRule,
				   unsigned short aType,
				   const TDOMString aCSSText) {

	mParentStyleSheet = aParentStyleSheet;
	mParentRule = aParentRule;
	mType = aType;
	setCSSText(aCSSText);

}

CSSRule :: ~CSSRule() {

}

TDOMString CSSRule :: getCSSText() const {

	return mCSSText;

}

void CSSRule :: setCSSText(const TDOMString aCSSText) {

	mCSSText = aCSSText;

}

CSSStyleSheetPtr CSSRule :: getParentStyleSheet() const {

	return make_shared(mParentStyleSheet);

}

CSSRulePtr CSSRule :: getParentRule() const {

	return make_shared(mParentRule);

}
