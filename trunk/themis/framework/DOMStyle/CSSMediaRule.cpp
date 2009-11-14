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
	Class Implementation Start Date: March 20, 2004
*/

/*
	CSSMediaRule implementation.
	See CSSMediaRule.hpp for more information.
*/

// Standard C headers
#include <stdio.h>

// DOM Style headers
#include "CSSMediaRule.hpp"
#include "CSSRuleList.hpp"
#include "DOMStyleSupport.hpp"

// DOM Core headers
#include "TDOMException.h"

CSSMediaRule :: CSSMediaRule(CSSStyleSheetPtr aParentStyleSheet,
							 CSSRulePtr aParentRule,
							 MediaListPtr aMedia)
			 : CSSRule(aParentStyleSheet, aParentRule, MEDIA_RULE, "") {

	printf("Creating CSSMediaRule\n");

	mCssRuleList = vector<CSSRulePtr>();
	mCssRules = CSSRuleListPtr(new CSSRuleList(&mCssRuleList));

	mMedia = aMedia;
}

CSSMediaRule :: ~CSSMediaRule() {

}

MediaListPtr CSSMediaRule :: getMedia() const {

	return mMedia;

}

CSSRuleListPtr CSSMediaRule :: getCssRules() const {

	return mCssRules;

}

unsigned long CSSMediaRule :: insertRule(const CSSRulePtr aRule,
										 unsigned long aIndex) {

	if (aIndex > mCssRuleList.size()) {
		throw TDOMException(INDEX_SIZE_ERR);
	}
	
	if (aIndex == mCssRuleList.size()) {
		mCssRuleList.push_back(aRule);
	}
	else {
		// Please check this.
		mCssRuleList.insert(&mCssRuleList[aIndex], aRule);
	}

}

unsigned long CSSMediaRule :: insertRule(const TDOMString aRule,
										 unsigned long aIndex) {
	// Don't like this function. Leaving it alone.
	return 0;

}

void CSSMediaRule :: deleteRule(unsigned long aIndex) {

	if (aIndex > mCssRuleList.size()) {
		throw TDOMException(INDEX_SIZE_ERR);
	}

	mCssRuleList.erase(&mCssRuleList[aIndex]);

}
