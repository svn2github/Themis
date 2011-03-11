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
#include <Messenger.h>

// DOM headers
#include "TNode.h"
#include "TNodeList.h"
#include "TElement.h"

// DOM Style headers
#include "CSSRuleList.hpp"
#include "CSSRule.hpp"
#include "CSSStyleRule.hpp"
#include "CSSValue.hpp"
#include "CSSPrimitiveValue.hpp"
#include "RGBColor.hpp"

// Themis headers
#include "commondefs.h"

// CSS Renderer headers
#include "CSSView.hpp"
#include "CSSRendererView.hpp"

// Constants used
const char cSpace = ' ';

CSSView :: CSSView(CSSRendererView * aBaseView,
				   TNodePtr aNode,
				   CSSStyleSheetPtr aStyleSheet,
				   BRect aRect,
				   rgb_color aColor,
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
	mColor = aColor;
	mClickable = false;
	mLineHeight = 0;
	mListStyleType = "none";
	mName = mNode->getNodeName();
	
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
			if (mNode->getNodeName() == "A") {
				// Only A can have a href for a hyperlink, so it is hardcoded here.
				if (element->hasAttribute("HREF")) {
					mHref = element->getAttribute("HREF");
					mClickable = true;
				}
			}
			CSSStyleDeclarationPtr style = GetComputedStyle(element);
			if (style.get()) {
				CSSValuePtr value = style->getPropertyCSSValue("display");
				if (value.get()) {
					CSSPrimitiveValuePtr primitiveValue = shared_static_cast<CSSPrimitiveValue>(value);
					TDOMString valueString = primitiveValue->getStringValue();
//					printf("Display property value: %s\n", valueString.c_str());
					if (valueString == "none") {
						mDisplay = false;
						mRect = BRect(0, 0, 0, 0);
					}
					else if (valueString == "inline") {
						mBlock = false;
					}
				}
				value = style->getPropertyCSSValue("font-size");
				if (value.get()) {
					CSSPrimitiveValuePtr primitiveValue = shared_static_cast<CSSPrimitiveValue>(value);
					if (primitiveValue.get()) {
						if (primitiveValue->getPrimitiveType() == CSSPrimitiveValue::CSS_EMS) {
							float floatValue = primitiveValue->getFloatValue(CSSPrimitiveValue::CSS_EMS);
							if (mFont == NULL) {
								mFont = new BFont(be_plain_font);
								mInheritedFont = false;
							}
							mFont->SetSize(floatValue * mFont->Size());
						}
					}
				}
				value = style->getPropertyCSSValue("font-style");
				if (value.get()) {
					CSSPrimitiveValuePtr primitiveValue = shared_static_cast<CSSPrimitiveValue>(value);
					if (primitiveValue.get()) {
						TDOMString valueString = primitiveValue->getStringValue();
						if (valueString == "italic") {
							if (mFont == NULL) {
								mFont = new BFont(be_plain_font);
								mInheritedFont = false;
							}
							mFont->SetFace(B_ITALIC_FACE);
						}
					}
				}
				value = style->getPropertyCSSValue("font-weight");
				if (value.get()) {
					CSSPrimitiveValuePtr primitiveValue = shared_static_cast<CSSPrimitiveValue>(value);
					if (primitiveValue.get()) {
						TDOMString valueString = primitiveValue->getStringValue();
						if (valueString == "bold") {
							if (mFont == NULL) {
								mFont = new BFont(be_plain_font);
								mInheritedFont = false;
							}
							mFont->SetFace(B_BOLD_FACE);
						}
					}
				}
				value = style->getPropertyCSSValue("margin-bottom");
				if (value.get()) {
					CSSPrimitiveValuePtr primitiveValue = shared_static_cast<CSSPrimitiveValue>(value);
					if (primitiveValue.get()) {
						if (primitiveValue->getPrimitiveType() == CSSPrimitiveValue::CSS_PX) {
							float floatValue = primitiveValue->getFloatValue(CSSPrimitiveValue::CSS_PX);
							mMarginBottom = floatValue;
						}
					}
				}
				value = style->getPropertyCSSValue("color");
				if (value.get()) {
					CSSPrimitiveValuePtr primitiveValue = shared_static_cast<CSSPrimitiveValue>(value);
					if (primitiveValue.get()) {
						if (primitiveValue->getPrimitiveType() == CSSPrimitiveValue::CSS_RGBCOLOR) {
							RGBColorPtr color = primitiveValue->getRGBColorValue();
							CSSPrimitiveValuePtr redValue = color->getRed();
							CSSPrimitiveValuePtr greenValue = color->getGreen();
							CSSPrimitiveValuePtr blueValue = color->getBlue();
							if (redValue->getPrimitiveType() == CSSPrimitiveValue::CSS_NUMBER &&
								greenValue->getPrimitiveType() == CSSPrimitiveValue::CSS_NUMBER &&
								blueValue->getPrimitiveType() == CSSPrimitiveValue::CSS_NUMBER) {
								mColor.red = (uint8)redValue->getFloatValue(CSSPrimitiveValue::CSS_NUMBER);
								mColor.green = (uint8)greenValue->getFloatValue(CSSPrimitiveValue::CSS_NUMBER);
								mColor.blue = (uint8)blueValue->getFloatValue(CSSPrimitiveValue::CSS_NUMBER);
							}
						}
					}
				}
				value = style->getPropertyCSSValue("list-style-type");
				if (value.get()) {
					CSSPrimitiveValuePtr primitiveValue = shared_static_cast<CSSPrimitiveValue>(value);
					if (primitiveValue.get()) {
						mListStyleType = primitiveValue->getStringValue();
					}
				}
			}
		}

		for (unsigned int i = 0; i < length; i++) {
			TNodePtr child = children->item(i);
			CSSView * childView = new CSSView(aBaseView,
											  child,
											  mStyleSheet,
											  mRect,
											  mColor,
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

void CSSView :: RetrieveLink() {

	if (mHref.size() > 0) {
		// Send a message to the window.
		// The window will do all the actual work of requesting a new page.
		string urlString = "";
		unsigned int position = mHref.find("://");
		if (position == string::npos) {
			urlString = mBaseView->GetDocumentURI();
			if (mHref[0] == '/') {
				// Get past the :// in the base uri, so we can find the right /
				position = urlString.find("://");
				if (position != string::npos) {
					position += 3;
				}
				else {
					position = 0;
				}
				// Find domain and use that as the base of the url.
				position = urlString.find_first_of("/", position);
				urlString = urlString.substr(0, position);
				
			}
			else {
				// Find last subdirectory and use that as the base of the url.
				position = urlString.find_last_of("/");
				urlString = urlString.substr(0, position + 1);
			}
		}
		urlString += mHref;
		BMessage message(URL_OPEN);
		message.AddString("url_to_open", urlString.c_str());
		BMessenger messenger(NULL, mBaseView->Window());
		messenger.SendMessage(&message);
	}

}

void CSSView :: MouseDown(BPoint aPoint) {
	
	if (mClickable) {
		printf("MouseDown for href %s\n", mHref.c_str());
		RetrieveLink();
	}
	else {
		unsigned int length = mChildren.size();
		unsigned int i = 0;
		bool found = false;
		while (i < length && !found) {
			if (mChildren[i]->Contains(aPoint)) {
				found = true;
				mChildren[i]->MouseDown(aPoint);
			}
			else {
				i++;
			}
		}
	}
	

}

void CSSView :: Draw() {
	
//	printf("Drawing %s\n", mNode->getNodeName().c_str());
//	mRect.PrintToStream();
	if (mDisplay) {
		if (mNode->getNodeType() == TEXT_NODE) {
			mBaseView->SetHighColor(mColor);
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
		else if (mListStyleType == "square") {
			mBaseView->SetHighColor(mColor);
			mBaseView->FillRect(mListStyleRect);
		}
		
		unsigned int length = mChildren.size();
		for (unsigned int i = 0; i < length; i++) {
			mChildren[i]->Draw();
		}
	}

}

bool CSSView :: Contains(BPoint aPoint) {
	
	bool result = false;
	
	if (mRect.Contains(aPoint)) {
		if (mEndPoint.x != 0 && mEndPoint.y != 0) {
			if (aPoint.y < mRect.top + mEndPoint.y) {
				result = true;
			}
			else if (aPoint.x < mRect.left + mEndPoint.x) {
				result = true;
			}
		}
		else {
			result = true;
		}
	}
	
	return result;
	
}

bool CSSView :: IsDisplayed() {

	return mDisplay;

}

bool CSSView :: IsBlock() {

	return mBlock;

}

bool CSSView :: IsClickable() {

	return mClickable;

}

BRect CSSView :: Bounds() {
	
	return mRect;
	
}

BPoint CSSView :: GetEndPoint() {

	return mEndPoint;

}

void CSSView :: Layout(BRect aRect,
					   BPoint aStartingPoint) {

//	printf("Doing layout for %s\n", mName.c_str());
	if (mDisplay) {
		mRect = aRect;
		// Always set the top of the rect to the one from the starting point as that definitely correct.
		mRect.top = aStartingPoint.y;
		BRect restRect = mRect;
		if (mNode->getNodeType() == TEXT_NODE) {
			// Text should normally fit inside the parent.
			// Store the width of the view, so we know how much fits on one line.
			float viewWidth = mRect.Width();
			// Indicates how much of a line is used at the moment.
			float lineWidth = 0;
			// Used to store the width of a textbox. Just for convenience.
			float boxWidth = 0;
			
			if (aStartingPoint.x > mRect.left) {
				// Not starting at the upper-left corner of the rectangle.
				// There is already some text in the rect from another element.
//				printf("Setting starting point to the right\n");
				// Set the current in use width of text to that already used by the other element.
				lineWidth = aStartingPoint.x - mRect.left;
				// Push the y starting point down, so we don't overwrite what is already there.
				mRect.top = aStartingPoint.y;
				// Also push the y starting point of the rest down, as we need it later.
				restRect.top = mRect.top;
			}

			// Create a rect that starts at the leftmost part and is one line high, but no pixels wide.
			// This is the minimum amount of space taken by this text view.
			BRect rect = BRect(mRect.left + lineWidth, mRect.top, mRect.left + lineWidth, mRect.top + mLineHeight);
			TDOMString text = mNode->getNodeValue();
			//printf("Doing layout for %s\n", text.c_str());
			// Push the top of the rest down by the lineheight. This is the height a single line of text will take.
			restRect.top += mLineHeight;
			// Set the initial right of the views rect to the left. It will get bigger again if text is added.
			mRect.right = mRect.left;
			
			// Loop through the text boxes calculated earlier in SplitText and fit them in the rect.
			// One TextBox is one bit of text (always one word currently).
			// Variable to check if the last textbox ends with a space, so we can add it at the end.
			bool endsWithSpace = false;
			unsigned int length = mTextBoxes.size();
			for (unsigned int i = 0; i < length; i++) {
				TextBox box = mTextBoxes[i];
				endsWithSpace = box.endsWithSpace();
				// How many pixels wide is the box?
				boxWidth = box.getPixelWidth();
				if (box.startsWithSpace()) {
					boxWidth += mSpaceWidth;
				}
				if (lineWidth + boxWidth > viewWidth) {
					// TextBox doesn't fit on the current line.
					// Start a new line.
					rect.left = mRect.left;
					rect.top += mLineHeight;
					rect.bottom += mLineHeight;
					restRect.top += mLineHeight;
					// Nothing in use yet.
					lineWidth = 0;
				}
				else {
					// Still fits inside the view.
					// We have to move the rect to the right of the previous text box.
					rect.left = rect.right;
				}
				// The right side of our rect is now moved by the size of the box width.
				rect.right = rect.left + boxWidth;
				if (box.startsWithSpace()) {
					rect.left += mSpaceWidth;
				}
				if (rect.right > mRect.right) {
					mRect.right = rect.right;
				}
				// Also adjust the current in use width of the line.
				lineWidth += boxWidth;
				// Store the rect for this box.
				box.setRect(rect);
				mTextBoxes[i] = box;
			}
			// In case the last textbox ends with a space, add a space now.
			if (endsWithSpace) {
				rect.right += mSpaceWidth;
			}
			// Mark the endpoint of the text in the rect.
			mEndPoint.Set(rect.right, rect.top);
//			printf("Set endpoint for text node to: ");
//			mEndPoint.PrintToStream();
		}
		else {
			//mEndPoint.Set(restRect.left, restRect.top);
			// Set the endpoint to the starting point, as that is the minimum endpoint
			mEndPoint = aStartingPoint;
			// Assume we don't need any horizontal space. The children will determine the space needed.
//			mRect.right = mRect.left;
			// In case we need to draw something before drawing any children, move the children.
			if (mListStyleType == "square") {
				mListStyleRect.left = restRect.left + 2;
				mListStyleRect.right = restRect.left + 7;
				mListStyleRect.top = restRect.top + 2;
				mListStyleRect.bottom = restRect.top + 7;
				restRect.left += 12;
				mEndPoint.x += 12;
			}
			// Layout the children.
			unsigned int length = mChildren.size();
			BPoint startingPoint = mEndPoint;
			float maxRight = 0;
			for (unsigned int i = 0; i < length; i++) {
				CSSView * childView = mChildren[i];
				// Only layout those children that are actually displayed.
				if (childView->IsDisplayed()) {
					if (childView->IsBlock()) {
						// Blocks have their own rect, so start at the left of the parent rect and just below the previous block.
						startingPoint = BPoint(restRect.left, restRect.top);
					//	previousChildIsBlock = true;
					//	startingPoint.PrintToStream();
					}
					// Do the layout for the child.
					childView->Layout(restRect, startingPoint);
					BRect rect2 = childView->Bounds();
					mEndPoint = childView->GetEndPoint();
					// Set the top of the remaining rect to the bottom of the child,
					// because the space above is already taken by the child.
					restRect.top = rect2.bottom;
					if (rect2.right > restRect.right) {
						// The child used more space than was available. We can use that space now as well
						// for any remaining children.
						restRect.right = rect2.right;
						mRect.right = rect2.right;
					}
					// What is the maximum horizontal space being taken up by one of the children?
					if (rect2.right > maxRight) {
						maxRight = rect2.right;
					}
					
					if (childView->IsBlock()) {
						// The next child will have to start at the left of the parent rect and just below this block.
						startingPoint = BPoint(restRect.left, restRect.top);
					//	previousChildIsBlock = true;
					//	startingPoint.PrintToStream();
					}
					else {
						// We can start where we left off.
						startingPoint = mEndPoint;
					//	previousChildIsBlock = false;
//						printf("Starting at: ");
//						mEndPoint.PrintToStream();
					}
				}
			}
			mRect.right = maxRight;
		}
		
		// Add any margins
		mEndPoint.Set(mEndPoint.x, mEndPoint.y + mMarginBottom);
		
		mRect.bottom = restRect.top + mMarginBottom;
		//mRect.right = restRect.right;
	}
	else {
		// Only in case we are the first element, otherwise this is never called.
		mRect = BRect(0, 0, 0, 0);
	}
	
//	printf("Layout done for %s\n", mName.c_str());
//	mRect.PrintToStream();

}

void CSSView :: SplitText() {
	
	// Split the text into words
	TDOMString text = mNode->getNodeValue();
	const char * textPointer = text.c_str();
	unsigned int length = text.size();
	unsigned int start = 0;
	unsigned int end = 0;
	bool textFound = false;
	bool spaceFound = false;
	char c = '\0';
	float pixelWidth = 0;
	for (unsigned int i = 0; i < length; i++) {
		c = text[i];
		if ((isspace(c) || iscntrl(c))) {
			if (textFound) {
				pixelWidth = mFont->StringWidth(textPointer + start, end - start + 1);
				TextBox box = TextBox(start, end, pixelWidth, spaceFound, true);
				mTextBoxes.push_back(box);
				textFound = false;
			}
			spaceFound = true;
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
		TextBox box = TextBox(start, end, pixelWidth, spaceFound);
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
