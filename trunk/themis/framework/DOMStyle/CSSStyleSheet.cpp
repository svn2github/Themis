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
	CSSStyleSheet implementation.
	See CSSStyleSheet.hpp for more information.
*/

// Standard C headers
#include <stdio.h>

// DOM Style headers
#include "CSSStyleSheet.hpp"
#include "CSSRule.hpp"
#include "CSSRuleList.hpp"

// DOM Core headers
#include "TDOMException.h"

CSSStyleSheet :: CSSStyleSheet(CSSRulePtr aOwnerRule)
			  : StyleSheet("text/css",
			  			   TNodePtr(),
			  			   StyleSheetPtr(),
			  			   "",
			  			   "",
						   MediaListPtr()) {

	printf( "Creating CSSStyleSheet\n" );

	mOwnerRule = aOwnerRule;

	mCssRuleList = vector<CSSRulePtr>();
	mCssRules = CSSRuleListPtr(new CSSRuleList(&mCssRuleList));

}

CSSStyleSheet :: ~CSSStyleSheet() {

}

CSSRulePtr CSSStyleSheet :: getOwnerRule() const {

	return make_shared(mOwnerRule);

}

CSSRuleListPtr CSSStyleSheet :: getCSSRules() const {

	return mCssRules;

}

unsigned long CSSStyleSheet :: insertRule(const CSSRulePtr aRule,
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

unsigned long CSSStyleSheet :: insertRule(const TDOMString aRule,
										  unsigned long aIndex) {
	// Don't like this function. Leaving it alone.
	return 0;

}

void CSSStyleSheet :: deleteRule(unsigned long aIndex) {

	if (aIndex > mCssRuleList.size()) {
		throw TDOMException(INDEX_SIZE_ERR);
	}

	mCssRuleList.erase(&mCssRuleList[aIndex]);

}
