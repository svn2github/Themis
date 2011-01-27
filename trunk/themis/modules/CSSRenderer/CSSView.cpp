/*
	Copyright (c) 2010 Mark Hellegers. All Rights Reserved.
	
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
	Class Start Date: Februari 28, 2010
*/

/*	CSSView implementation
	See CSSView.hpp for more information
	
*/

// Standard C headers
#include <stdio.h>

// BeOS headers
#include <Font.h>
#include <Point.h>
#include <Window.h>

// DOM headers
#include "TNode.h"
#include "TNodeList.h"
#include "TElement.h"

// DOM Style headers
#include "CSSRuleList.hpp"
#include "CSSRule.hpp"
#include "CSSStyleRule.hpp"

// CSS Renderer headers
#include "CSSView.hpp"
#include "CSSRendererView.hpp"

// Constants used
const char cSpace = ' ';

CSSView :: CSSView(CSSRendererView * aBaseView,
				   TNodePtr aNode,
				   CSSStyleSheetPtr aStyleSheet,
				   BRect aRect,
				   BFont * aFont)
		: BHandler("CSSView") {

	mBaseView = aBaseView;
	mNode = aNode;
	mStyleSheet = aStyleSheet;
	mRect = aRect;
	mDisplay = true;
	mBlock = true;
	mFont = aFont;
	mInheritedFont = true;
	mMarginBottom = 0;
	
	if (mNode->hasChildNodes()) {
		TNodeListPtr children = mNode->getChildNodes();
		unsigned int length = children->getLength();
     	if ((mNode->getNodeType() == ELEMENT_NODE) &&
				 (mNode->getNodeName() == "TITLE") &&
				 (length > 0)) {
			printf("Found TITLE node\n");
			TNodePtr textChild = children->item(0);
			TDOMString titleText = textChild->getNodeValue();
			TDOMString strippedTitle = "";
			unsigned int titleLength = titleText.size();
			for (unsigned int i = 0; i < titleLength; i++) {
				if (!iscntrl(titleText[i])) {
					strippedTitle += titleText[i];
				}
			}
			if (strippedTitle != "") {
				printf("Setting title of window to: %s\n", strippedTitle.c_str());
				aBaseView->SetTitle(strippedTitle);
			}
		}
		else if (mNode->getNodeType() == ELEMENT_NODE) {
			TElementPtr element = shared_static_cast<TElement>(mNode);
			CSSStyleDeclarationPtr style = GetComputedStyle(element);
			if (style.get()) {
				TDOMString value = style->getPropertyValue("display");
//				printf("Display property value: %s\n", value.c_str());
				if (value == "none") {
					mDisplay = false;
				}
				else if (value == "inline") {
					mBlock = false;
				}
				value = style->getPropertyValue("font-size");
				if (value != "") {
//					printf("Found font size of %s\n", value.c_str());
					size_t endPos = value.find_last_not_of("em");
					//printf("endPos: %u\n", endPos);
					if (string::npos != endPos) {
						value = value.substr(0, endPos + 1);
					}
//					printf("Setting font size to %s\n", value.c_str());
					if (mFont == NULL) {
						mFont = new BFont(be_plain_font);
						mInheritedFont = false;
					}
//					printf("Setting font size as integer: %lu\n", strtoul(value.c_str(), NULL, 10));
					mFont->SetSize(strtod(value.c_str(), NULL) * mFont->Size());
				}
				value = style->getPropertyValue("margin-bottom");
				if (value != "") {
					size_t endPos = value.find_last_not_of("px");
					if (string::npos != endPos) {
						value = value.substr(0, endPos + 1);
					}
					mMarginBottom = strtod(value.c_str(), NULL);
				}
			}
		}

		for (unsigned int i = 0; i < length; i++) {
			TNodePtr child = children->item(i);
			CSSView * childView = new CSSView(aBaseView,
											  child,
											  mStyleSheet,
											  mRect,
											  mFont);
			mChildren.push_back(childView);
		}
	}
	else if (mNode->getNodeType() == TEXT_NODE) {
		if (mFont == NULL) {
			mFont = new BFont(be_plain_font);
			mInheritedFont = false;
		}
		font_height height;
		mFont->GetHeight(&height);
		mLineHeight = height.ascent + height.descent + height.leading;
		mBottomMargin = height.descent;
		mSpaceWidth = mFont->StringWidth(&cSpace, 1);
		mBlock = false;

		SplitText();
	}

}

CSSView :: ~CSSView() {

	unsigned int length = mChildren.size();
	for (unsigned int i = 0; i < length; i++) {
		delete mChildren[i];
	}

/*
	if (!mInheritedFont) {
		delete mFont;
	}
*/

}

void CSSView :: Draw() {
	
//	printf("Drawing %s\n", mNode->getNodeName().c_str());
//	mRect.PrintToStream();
	if (mDisplay) {
		if (mNode->getNodeType() == TEXT_NODE) {
			BPoint drawPoint;
			string text = mNode->getNodeValue();
	
			unsigned int length = mTextBoxes.size();
			for (unsigned int i = 0; i < length; i++) {
				TextBox box = mTextBoxes[i];
				unsigned int start = 0;
				unsigned int end = 0;
				box.getRange(start, end);
				BRect rect = box.getRect();
	//			printf("Drawing string: %s for %u at %f and %f\n", (text.c_str()) + start, end - start, rect.left, rect.top);
				drawPoint.Set(rect.left, rect.top + mLineHeight - mBottomMargin);
				mBaseView->SetFont(mFont);
				mBaseView->DrawString((text.c_str()) + start, end - start + 1, drawPoint);
			}
		}
		
		unsigned int length = mChildren.size();
		for (unsigned int i = 0; i < length; i++) {
			mChildren[i]->Draw();
		}
	}

}

bool CSSView :: IsDisplayed() {

	return mDisplay;

}

bool CSSView :: IsBlock() {

	return mBlock;

}

BRect CSSView :: Bounds() {
	
	return mRect;
	
}

BPoint CSSView :: GetEndPoint() {

	return mEndPoint;

}

void CSSView :: Layout(BRect aRect,
					   BPoint aStartingPoint) {

	if (mDisplay) {
		mRect = aRect;
		BRect restRect = mRect;
		if (mNode->getNodeType() == TEXT_NODE) {
			float viewWidth = mRect.Width();
			float lineWidth = 0;
			float boxWidth = 0;
			
			if (aStartingPoint.x > mRect.left) {
//				printf("Setting starting point to the right\n");
				lineWidth = aStartingPoint.x - mRect.left;
				mRect.top = aStartingPoint.y;
				restRect.top = mRect.top;
			}
	
			BRect rect = BRect(mRect.left + lineWidth, mRect.top, mRect.left + lineWidth, mRect.top + mLineHeight);
			TDOMString text = mNode->getNodeValue();
			restRect.top += mLineHeight;
			
			unsigned int length = mTextBoxes.size();
			for (unsigned int i = 0; i < length; i++) {
				TextBox box = mTextBoxes[i];
				boxWidth = box.getPixelWidth();
				if (lineWidth + boxWidth > viewWidth) {
					// TextBox doesn't fit on the current line.
					// Start a new line.
					rect.left = mRect.left;
					rect.top += mLineHeight;
					rect.bottom += mLineHeight;
					restRect.top += mLineHeight;
					lineWidth = 0;
				}
				else {
					rect.left = rect.right;
					if (rect.left != mRect.left) {
						rect.left += mSpaceWidth;
						lineWidth += mSpaceWidth;
					}
				}
				rect.right = rect.left + boxWidth;
	//			rect.PrintToStream();
				box.setRect(rect);
				mTextBoxes[i] = box;
				lineWidth += boxWidth;
			}
			mEndPoint.Set(rect.right, rect.top);
//			printf("Set endpoint for text node to: ");
//			mEndPoint.PrintToStream();
		}
		else {
			//mEndPoint.Set(restRect.left, restRect.top);
			mEndPoint = aStartingPoint;
			unsigned int length = mChildren.size();
			for (unsigned int i = 0; i < length; i++) {
				CSSView * childView = mChildren[i];
				if (childView->IsDisplayed()) {
					BPoint startingPoint;
					if (childView->IsBlock()) {
						startingPoint = BPoint(restRect.left, restRect.top);
					//	startingPoint.PrintToStream();
					}
					else {
						startingPoint = mEndPoint;
//						printf("Starting at: ");
//						mEndPoint.PrintToStream();
					}
					childView->Layout(restRect, startingPoint);
					BRect rect2 = childView->Bounds();
					mEndPoint = childView->GetEndPoint();
					restRect.top = rect2.bottom;
					if (rect2.right > restRect.right) {
						restRect.right = rect2.right;
					}
				}
			}
		}
		
		mEndPoint.Set(mEndPoint.x, mEndPoint.y + mMarginBottom);
		
		mRect.bottom = restRect.top + mMarginBottom;
		mRect.right = restRect.right;
	}
	else {
		mRect = BRect(0, 0, 0, 0);
	}

}

void CSSView :: SplitText() {
	
	// Split the text into words
	TDOMString text = mNode->getNodeValue();
	const char * textPointer = text.c_str();
	unsigned int length = text.size();
	unsigned int start = 0;
	unsigned int end = 0;
	bool textFound = false;
	char c = '\0';
	float pixelWidth = 0;
	for (unsigned int i = 0; i < length; i++) {
		c = text[i];
		if ((isspace(c) || iscntrl(c))) {
			if (textFound) {
				pixelWidth = mFont->StringWidth(textPointer + start, end - start + 1);
				TextBox box = TextBox(start, end, pixelWidth);
				mTextBoxes.push_back(box);
				textFound = false;
			}
			start = i;
			end = i;
		}
		else {
			if (!textFound) {
				start = i;
				textFound = true;
			}
			end = i;
		}
	}
	
	if (textFound) {
		pixelWidth = mFont->StringWidth(textPointer + start, end - start + 1);
		TextBox box = TextBox(start, end, pixelWidth);
		mTextBoxes.push_back(box);
	}

}

CSSStyleDeclarationPtr CSSView :: GetComputedStyle(TElementPtr aElement) {

	CSSStyleDeclarationPtr result;
	CSSRuleListPtr rules = mStyleSheet->getCSSRules();
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
//					printf("Found a match for %s\n", aElement->getTagName().c_str());
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
	
	return result;

}
