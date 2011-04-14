/*
	Copyright (c) 2011 Mark Hellegers. All Rights Reserved.
	
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
	Class Start Date: April 10, 2011
*/

/*	CSSStyleContainer implementation
	See CSSStyleContainer.hpp for more information
	
*/

// Standard C headers
#include <stdio.h>

// DOM Style headers
#include "CSSRuleList.hpp"
#include "CSSStyleRule.hpp"

// CSS Renderer headers
#include "CSSStyleContainer.hpp"

CSSStyleContainer :: CSSStyleContainer() {
}

CSSStyleContainer :: ~CSSStyleContainer() {
}

void CSSStyleContainer :: addStyleSheet(const CSSStyleSheetPtr aStyleSheet) {

	mStyleSheetList.push_back(aStyleSheet);

}

CSSStyleDeclarationPtr CSSStyleContainer :: getComputedStyle(const TElementPtr aElement) {

	CSSStyleDeclarationPtr result;
	unsigned int styleSheetLength = mStyleSheetList.size();

	for (unsigned int j = 0; j < styleSheetLength; j++) {
		CSSStyleSheetPtr styleSheet = mStyleSheetList[j];
		CSSRuleListPtr rules = styleSheet->getCSSRules();
		unsigned long length = rules->getLength();
		bool found = false;
		unsigned long i = 0;
		while (i < length && !found) {
			CSSRulePtr rule = rules->item(i);
			switch (rule->getType()) {
				case CSSRule::STYLE_RULE: {
					CSSStyleRulePtr styleRule = shared_static_cast<CSSStyleRule>(rule);
					// Trim trailing spaces before comparing.
					string styleText = styleRule->getSelectorText();
					size_t endPos = styleText.find_last_not_of(" ");
					if (string::npos != endPos) {
						styleText = styleText.substr(0, endPos + 1);
					}
					if (styleText == aElement->getTagName()) {
						result = styleRule->getStyle();
						found = true;
					}
					break;
				}
				default: {
					break;
				}
			}
			i++;
		}
	}

/*	
	if (found) {
		printf("Found a match for %s\n", aElement->getTagName().c_str());
	}
	else {
		printf("Didn't find a match for %s\n", aElement->getTagName().c_str());
	}
*/	
	return result;
}
