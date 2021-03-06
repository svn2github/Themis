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

// Standard C++ headers
#include <string>

// BeOS headers
#include <Font.h>
#include <Point.h>
#include <Window.h>
#include <Messenger.h>
#include <TranslationUtils.h>
#include <Bitmap.h>

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
#include "CSSStyleContainer.hpp"
#include "InlineDisplayView.hpp"
#include "BlockDisplayView.hpp"
#include "TableDisplayView.hpp"
#include "TableRowGroupDisplayView.hpp"
#include "TableHeaderGroupDisplayView.hpp"
#include "TableFooterGroupDisplayView.hpp"
#include "TableRowDisplayView.hpp"
#include "TableCellDisplayView.hpp"
#include "NoneDisplayView.hpp"
#include "ButtonInputDisplayView.hpp"
#include "TextInputDisplayView.hpp"
#include "FormDisplayView.hpp"
#include "HiddenInputDisplayView.hpp"

// Namespaces used
using std::string;

// Constants used
const char cSpace = ' ';

CSSView :: CSSView(CSSRendererView * aBaseView,
				   TNodePtr aNode,
				   CSSStyleContainer * aStyleSheets,
				   CSSStyleDeclarationPtr aStyle,
				   BRect aRect,
				   int32 aSiteId,
				   int32 aUrlId,
				   rgb_color aColor,
				   BFont * aFont,
				   WhiteSpaceType aWhiteSpace,
				   BHandler * aForm)
		: BHandler("CSSView") {

	mBaseView = aBaseView;
	mNode = aNode;
	mStyleSheets = aStyleSheets;
	mRect = aRect;
	mSiteId = aSiteId;
	mUrlId = aUrlId;
	mFont = aFont;
	mInheritedFont = true;
	mMarginBottom = 0;
	mMarginRight = 0;
	mMarginLeft = 0;
	mBorderWidth = 0;
	mColor = aColor;
	mClickable = false;
	mLineHeight = 0;
	mListStyleType = "none";
	mName = mNode->getNodeName();
	mRequestedWidth = -1;
	mRequestedHeight = -1;
	mWhiteSpace = aWhiteSpace;
	mForm = aForm;
	mImage = NULL;

	if (mNode->getNodeType() == ELEMENT_NODE) {
		TElementPtr element = shared_static_cast<TElement>(mNode);
		if (mName == "A") {
			// Only A can have a href for a hyperlink, so it is hardcoded here.
			if (element->hasAttribute("HREF")) {
				mHref = element->getAttribute("HREF");
				mClickable = true;
			}
		}
		else if (mName == "IMG") {
			// Images have the image source in the src attribute.
			if (element->hasAttribute("SRC")) {
				mHref = element->getAttribute("SRC");
			}
			if (element->hasAttribute("WIDTH")) {
				mRequestedWidth = atoi(element->getAttribute("WIDTH").c_str());
			}
			if (element->hasAttribute("HEIGHT")) {
				mRequestedHeight = atoi(element->getAttribute("HEIGHT").c_str());
			}
		}
		ApplyStyle(element, aStyle);
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
		mDisplay = true;
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

string CSSView :: ConstructURL(string aUrl) {

	// FIXME. Look at http://en.wikipedia.org/wiki/URI_scheme for the general syntax.
	// This is not taken into account with the code below.
	string urlString = "";
	if (aUrl.size() > 0) {
		unsigned int position = aUrl.find("://");
		if (position == string::npos || aUrl[0] == '/') {
			urlString = mBaseView->GetDocumentURI();
			printf("Document URI: %s\n", urlString.c_str());
			if (aUrl[0] == '/') {
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
		urlString += aUrl;
	}

	return urlString;

}

void CSSView :: RetrieveLink(bool aVisible) {

	// FIXME. Look at http://en.wikipedia.org/wiki/URI_scheme for the general syntax.
	// This is not taken into account with the code below.
	string urlString = ConstructURL(mHref);
	if (urlString.size() > 0) {
		// Send a message to the window.
		// The window will do all the actual work of requesting a new page.
		BMessage message(URL_OPEN);
		message.AddInt32("site_id", mSiteId);
		message.AddInt32("url_id", mUrlId);
		message.AddString("url_to_open", urlString.c_str());
		message.AddBool("visible", aVisible);
		BMessenger messenger(NULL, mBaseView->Window());
		messenger.SendMessage(&message);
	}

}

void CSSView :: CreateChildren(
	CSSRendererView * aBaseView,
	TNodePtr aNode,
	CSSStyleContainer * aStyleSheets,
	CSSStyleDeclarationPtr aStyle,
	BRect aRect,
	int32 aSiteId,
	int32 aUrlId,
	rgb_color aColor,
	BFont * aFont = NULL,
	WhiteSpaceType aWhiteSpace = NORMAL,
	BHandler * aForm = NULL) {

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

		for (unsigned int i = 0; i < length; i++) {
			TNodePtr child = children->item(i);
			if (child->getNodeType() == ELEMENT_NODE) {
				CSSView * childView = NULL;
				TElementPtr element = shared_static_cast<TElement>(child);
				CSSStyleDeclarationPtr style = mStyleSheets->getComputedStyle(element);
				if (child->getNodeName() == "INPUT") {
					if (element->hasAttribute("TYPE")) {
						string inputType = element->getAttribute("TYPE");
						if (inputType == "text" || inputType == "") {
							childView = new TextInputDisplayView(
								aBaseView,
								child,
								mStyleSheets,
								style,
								mRect,
								mSiteId,
								mUrlId,
								mColor,
								mFont,
								mWhiteSpace,
								mForm);
						}
						else if (inputType == "submit") {
							childView = new ButtonInputDisplayView(
								aBaseView,
								child,
								mStyleSheets,
								style,
								mRect,
								mSiteId,
								mUrlId,
								mColor,
								mFont,
								mWhiteSpace,
								mForm);
						}
						else if (inputType == "hidden") {
							childView = new HiddenInputDisplayView(
								aBaseView,
								child,
								mStyleSheets,
								style,
								mRect,
								mSiteId,
								mUrlId,
								mColor,
								mFont,
								mWhiteSpace,
								mForm);
						}
					}
					else {
						// Default is text
						childView = new TextInputDisplayView(
							aBaseView,
							child,
							mStyleSheets,
							style,
							mRect,
							mSiteId,
							mUrlId,
							mColor,
							mFont,
							mWhiteSpace,
							mForm);
					}
				}
				else if (child->getNodeName() == "FORM") {
					childView = new FormDisplayView(
						aBaseView,
						child,
						mStyleSheets,
						style,
						mRect,
						mSiteId,
						mUrlId,
						mColor,
						mFont,
						mWhiteSpace,
						NULL);
				}
				if (childView == NULL) {
					// No input element found.
					if (style.get()) {
						CSSValuePtr value = style->getPropertyCSSValue("display");
						// Default to inline in case it doesn't exist.
						TDOMString valueString = "inline";
						if (value.get()) {
							CSSPrimitiveValuePtr primitiveValue = shared_static_cast<CSSPrimitiveValue>(value);
							valueString = primitiveValue->getStringValue();
						}
				//		printf("Display property value: %s\n", valueString.c_str());
						if (valueString == "inline") {
							childView = new InlineDisplayView(
								aBaseView,
								child,
								mStyleSheets,
								style,
								mRect,
								mSiteId,
								mUrlId,
								mColor,
								mFont,
								mWhiteSpace,
								mForm);
						}
						else if (valueString == "block") {
							childView = new BlockDisplayView(
								aBaseView,
								child,
								mStyleSheets,
								style,
								mRect,
								mSiteId,
								mUrlId,
								mColor,
								mFont,
								mWhiteSpace,
								mForm);
						}
						else if (valueString == "table") {
							childView = new TableDisplayView(
								aBaseView,
								child,
								mStyleSheets,
								style,
								mRect,
								mSiteId,
								mUrlId,
								mColor,
								mFont,
								mWhiteSpace,
								mForm);
						}
						else if (valueString == "table-row-group") {
							childView = new TableRowGroupDisplayView(
								aBaseView,
								child,
								mStyleSheets,
								style,
								mRect,
								mSiteId,
								mUrlId,
								mColor,
								mFont,
								mWhiteSpace,
								mForm);
						}
						else if (valueString == "table-header-group") {
							childView = new TableHeaderGroupDisplayView(
								aBaseView,
								child,
								mStyleSheets,
								style,
								mRect,
								mSiteId,
								mUrlId,
								mColor,
								mFont,
								mWhiteSpace,
								mForm);
						}
						else if (valueString == "table-footer-group") {
							childView = new TableFooterGroupDisplayView(
								aBaseView,
								child,
								mStyleSheets,
								style,
								mRect,
								mSiteId,
								mUrlId,
								mColor,
								mFont,
								mWhiteSpace,
								mForm);
						}
						else if (valueString == "table-row") {
							childView = new TableRowDisplayView(
								aBaseView,
								child,
								mStyleSheets,
								style,
								mRect,
								mSiteId,
								mUrlId,
								mColor,
								mFont,
								mWhiteSpace,
								mForm);
						}
						else if (valueString == "table-cell") {
							childView = new TableCellDisplayView(
								aBaseView,
								child,
								mStyleSheets,
								style,
								mRect,
								mSiteId,
								mUrlId,
								mColor,
								mFont,
								mWhiteSpace,
								mForm);
						}
						else if (valueString == "none") {
							childView = new NoneDisplayView(
								aBaseView,
								child,
								mStyleSheets,
								style,
								mRect,
								mSiteId,
								mUrlId,
								mColor,
								mFont,
								mWhiteSpace,
								mForm);
						}
						else {
							// The default is an inline element
							printf("Currently unsupported display type: %s. Defaulting to inline...\n", valueString.c_str());
							childView = new InlineDisplayView(aBaseView,
								child,
								mStyleSheets,
								style,
								mRect,
								mSiteId,
								mUrlId,
								mColor,
								mFont,
								mWhiteSpace,
								mForm);
						}
					}
					else {
						printf("No style specified for %s. Defaulting to inline...\n", element->getTagName().c_str());
						childView = new InlineDisplayView(aBaseView,
							child,
							mStyleSheets,
							style,
							mRect,
							mSiteId,
							mUrlId,
							mColor,
							mFont,
							mWhiteSpace,
							mForm);
					}
				}
				mChildren.push_back(childView);
			}
			else if (child->getNodeType() == TEXT_NODE) {
				// Quick fix. Take a look at what would be elegant.
				CSSStyleDeclarationPtr style;
				CSSView * childView = new CSSView(
					aBaseView,
					child,
					mStyleSheets,
					style,
					mRect,
					mSiteId,
					mUrlId,
					mColor,
					mFont,
					mWhiteSpace,
					mForm);
				mChildren.push_back(childView);
			}
		}
	}

}

void CSSView :: ApplyStyle(const TElementPtr aElement,
						   const CSSStyleDeclarationPtr aStyle) {
	
	if (aStyle.get()) {
		unsigned long length = aStyle->getLength();
		for (unsigned long i = 0; i < length; i++) {
			TDOMString propertyName = aStyle->item(i);
			CSSValuePtr value = aStyle->getPropertyCSSValue(propertyName);
			if (value.get()) {
				if (propertyName == "font-size") {
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
				else if (propertyName == "font-style") {
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
				else if (propertyName == "font-weight") {
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
				else if (propertyName == "margin-bottom") {
					CSSPrimitiveValuePtr primitiveValue = shared_static_cast<CSSPrimitiveValue>(value);
					if (primitiveValue.get()) {
						if (primitiveValue->getPrimitiveType() == CSSPrimitiveValue::CSS_PX) {
							float floatValue = primitiveValue->getFloatValue(CSSPrimitiveValue::CSS_PX);
							mMarginBottom = floatValue;
						}
					}
				}
				else if (propertyName == "margin-right") {
					CSSPrimitiveValuePtr primitiveValue = shared_static_cast<CSSPrimitiveValue>(value);
					if (primitiveValue.get()) {
						if (primitiveValue->getPrimitiveType() == CSSPrimitiveValue::CSS_PX) {
							float floatValue = primitiveValue->getFloatValue(CSSPrimitiveValue::CSS_PX);
							mMarginRight = floatValue;
						}
					}
				}
				else if (propertyName == "margin-left") {
					CSSPrimitiveValuePtr primitiveValue = shared_static_cast<CSSPrimitiveValue>(value);
					if (primitiveValue.get()) {
						if (primitiveValue->getPrimitiveType() == CSSPrimitiveValue::CSS_PX) {
							float floatValue = primitiveValue->getFloatValue(CSSPrimitiveValue::CSS_PX);
							mMarginLeft = floatValue;
						}
					}
				}
				else if (propertyName == "color") {
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
				else if (propertyName == "list-style-type") {
					CSSPrimitiveValuePtr primitiveValue = shared_static_cast<CSSPrimitiveValue>(value);
					if (primitiveValue.get()) {
						mListStyleType = primitiveValue->getStringValue();
					}
				}
				else if (propertyName == "border-style") {
					CSSPrimitiveValuePtr primitiveValue = shared_static_cast<CSSPrimitiveValue>(value);
					if (primitiveValue.get()) {
						mBorderStyle = primitiveValue->getStringValue();
					}
				}
				else if (propertyName == "border-width") {
					CSSPrimitiveValuePtr primitiveValue = shared_static_cast<CSSPrimitiveValue>(value);
					if (primitiveValue.get()) {
						if (primitiveValue->getPrimitiveType() == CSSPrimitiveValue::CSS_PX) {
							float floatValue = primitiveValue->getFloatValue(CSSPrimitiveValue::CSS_PX);
							mBorderWidth = floatValue;
						}
					}
				}
				else if (propertyName == "font-family") {
					CSSPrimitiveValuePtr primitiveValue = shared_static_cast<CSSPrimitiveValue>(value);
					if (primitiveValue.get()) {
						TDOMString valueString = primitiveValue->getStringValue();
						if (valueString == "monospace") {
							if (mFont == NULL) {
								mFont = new BFont(be_fixed_font);
								mInheritedFont = false;
							}
						}
					}
				}
				else if (propertyName == "white-space") {
					CSSPrimitiveValuePtr primitiveValue = shared_static_cast<CSSPrimitiveValue>(value);
					if (primitiveValue.get()) {
						TDOMString valueString = primitiveValue->getStringValue();
						if (valueString == "pre") {
							mWhiteSpace = PRE;
						}
					}
				}
			}
		}
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

	if (mDisplay) {
		if (mNode != NULL && mNode->getNodeType() == TEXT_NODE) {
			mBaseView->SetHighColor(mColor);
			BPoint drawPoint;
			string text = mNode->getNodeValue();
	
			unsigned int length = mTextBoxes.size();
			for (unsigned int i = 0; i < length; i++) {
				TextBox box = mTextBoxes[i];
				unsigned int start = 0;
				unsigned int end = 0;
				box.getRange(start, end);
				// Draw the string, but only when there is something to draw.
				if (!box.emptyBox()) {
					BRect rect = box.getRect();
					//printf("Drawing string: %s for %u at %f and %f\n", (text.c_str()) + start, end - start + 1, rect.left, rect.top + mLineHeight - mBottomMargin);
					drawPoint.Set(rect.left, rect.top + mLineHeight - mBottomMargin);
					mBaseView->SetFont(mFont);
					mBaseView->DrawString((text.c_str()) + start, end - start + 1, drawPoint);
				}
			}
		}
		else if (mListStyleType == "square") {
			mBaseView->SetHighColor(mColor);
			mBaseView->FillRect(mListStyleRect);
		}
		
		if (mBorderStyle == "solid") {
			float savedPenSize = mBaseView->PenSize();
			mBaseView->SetPenSize(mBorderWidth);
			mBaseView->SetHighColor(mColor);
			mBaseView->StrokeRect(mRect);
			mBaseView->SetPenSize(savedPenSize);
		}
		unsigned int length = mChildren.size();
		for (unsigned int i = 0; i < length; i++) {
			mChildren[i]->Draw();
		}
		
		// Quick hack to test image loading
		if ((mName == "IMG") && (mHref != "")) {
			if (!mImage) {
				string urlString = ConstructURL(mHref);
				printf("Attempting to draw img: %s\n", urlString.c_str());
				string protocolString(urlString.substr(0, 7));
				string fileLocation(urlString.substr(7, urlString.size() - 7));
				if (!protocolString.compare("file://")) {
					printf("File protocol found. Getting bitmap of %s\n", fileLocation.c_str());
					mImage = BTranslationUtils::GetBitmapFile(fileLocation.c_str());
				}
			}
			if (mImage) {
				mBaseView->DrawBitmap(mImage, mRect.LeftTop());
			}
		}
	}
}

bool CSSView :: Contains(BPoint aPoint) {
	
	bool result = false;
	unsigned int i = 0;
	unsigned int length = mRects.size();
	while (i < length && !result) {
		result = mRects[i].Contains(aPoint);
		i++;
		
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

void CSSView :: Layout(
	BRect aRect,
	BPoint aStartingPoint) {

//	printf("Doing layout for %s\n", mName.c_str());
	if (mName == "IMG") {
		printf("Starting with rect and startingpoint for image\n");
		aRect.PrintToStream();
		aStartingPoint.PrintToStream();
	}
	mRects.clear();
	if (mDisplay) {
		mRect = aRect;
		mRect.left += mMarginLeft;
		// Enforce the requested width if set
		if (mRequestedWidth > -1) {
			mRect.right = mRect.left + mRequestedWidth;
		}
		// Always set the top of the rect to the one from the starting point as that is definitely correct.
		mRect.top = aStartingPoint.y;
		BRect restRect = mRect;
		// If we are drawing a border around this element, make sure there is less space for the content.
		restRect.InsetBy(mBorderWidth, mBorderWidth);
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
			// Variable to store a container rect, that can contain more than one word,
			// which can later be used to calculate if a point is inside an area.
			BRect containerRect = rect;
			unsigned int length = mTextBoxes.size();
			for (unsigned int i = 0; i < length; i++) {
				TextBox box = mTextBoxes[i];
				endsWithSpace = box.endsWithSpace();
				// How many pixels wide is the box?
				boxWidth = box.getPixelWidth();
				if (box.startsWithSpace()) {
					boxWidth += mSpaceWidth;
				}
				//printf("Linewidth: %f, boxwidth: %f, viewWidth: %f\n", lineWidth, boxWidth, viewWidth);
				if ((lineWidth + boxWidth > viewWidth) || mWhiteSpace == PRE) {
					// TextBox doesn't fit on the current line.
					// Store the current container rect if needed
					if (containerRect.right != containerRect.left) {
						mRects.push_back(containerRect);
					}
					// Start a new line.
					rect.left = mRect.left;
					rect.right = rect.left;
					rect.top += mLineHeight;
					rect.bottom += mLineHeight;
					restRect.top += mLineHeight;
					// Nothing in use yet.
					lineWidth = 0;
					// The container rect is the same as the normal rect.
					containerRect = rect;
				}
				else {
					// Still fits inside the view.
					// We have to move the rect to the right of the previous text box.
					rect.left = rect.right;
				}
				// The right side of our rect is now moved by the size of the box width.
				rect.right = rect.left + boxWidth;
				containerRect.right = rect.right;
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
				containerRect.right = rect.right; 
			}
			if (containerRect.right != containerRect.left) {
				// Still an area that needs to be stored.
				mRects.push_back(containerRect);
			}
			// Mark the endpoint of the text in the rect.
			mEndPoint.Set(rect.right, rect.top);
			mRect.bottom = rect.top + mLineHeight;
//			printf("Set endpoint for text node to: ");
//			mEndPoint.PrintToStream();
		}

		// Enforce the requested height if set
		if (mRequestedHeight > -1) {
			mRect.bottom = mRect.top + mRequestedHeight;
			//mEndPoint.y = mRect.bottom;
		}
		
		// Add any margins
		mEndPoint.Set(mEndPoint.x + mMarginRight, mEndPoint.y + mMarginBottom);
		
		mRect.bottom += mMarginBottom;
		mRect.right += mMarginRight;
		//mRect.right = restRect.right;
	}
	else {
		// Only in case we are the first element, otherwise this is never called.
		mRect = BRect(0, 0, 0, 0);
	}

	if (mName == "IMG") {
		printf("Ending with rect and startingpoint for image\n");
		mRect.PrintToStream();
		mEndPoint.PrintToStream();
	}
	
//	printf("Layout done for %s\n", mName.c_str());
//	mRect.PrintToStream();

}

void CSSView :: AttachedToWindow() {

	if (!Looper()) {
		mBaseView->Window()->LockLooper();
		mBaseView->Window()->AddHandler(this);
		mBaseView->Window()->UnlockLooper();
	}

	unsigned int length = mChildren.size();
	for (unsigned int i = 0; i < length; i++) {
		mChildren[i]->AttachedToWindow();
	}

}

void CSSView :: SplitText() {
	
	TDOMString text = mNode->getNodeValue();
	const char * textPointer = text.c_str();
	unsigned int length = text.size();
	unsigned int start = 0;
	unsigned int end = 0;
	bool textFound = false;
	char c = '\0';
	float pixelWidth = 0;

	if (mWhiteSpace == PRE) {
		// Only split the text at a linebreak
		for (unsigned int i = 0; i < length; i++) {
			c = text[i];
			if (c == '\n') {
				pixelWidth = mFont->StringWidth(textPointer + start, end - start + 1);
				TextBox box = TextBox(start, end, pixelWidth, false, false, !textFound);
				mTextBoxes.push_back(box);
				textFound = false;
				start = i;
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
			TextBox box = TextBox(start, end, pixelWidth, false, false);
			mTextBoxes.push_back(box);
		}
	}
	else {
		// Split the text into words
		bool spaceFound = false;
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

}

void CSSView :: RetrieveResources() {

//	printf("Retrieving resources in %s with href %s\n", mName.c_str(), mHref.c_str());
	if (mHref != "" && !mClickable) {
		// Some resource that must be loaded.
		printf("Retrieving %s\n", mHref.c_str());
		RetrieveLink(false);
	}

	unsigned int length = mChildren.size();
	for (unsigned int i = 0; i < length; i++) {
		CSSView * childView = mChildren[i];
		childView->RetrieveResources();
	}
	
}

void CSSView :: SetWidth(float aWidth) {
	
	mRequestedWidth = aWidth;
	
}

void CSSView :: SetHeight(float aHeight) {
	
	mRequestedHeight = aHeight;
	
}

float CSSView :: GetChildWidth(unsigned int aIndex) {

	float result = 0;

	if (aIndex < mChildren.size()) {
		CSSView * childView = mChildren[aIndex];
		result = childView->Bounds().Width();
	}

	return result;

}

void CSSView :: SetChildWidth(float aWidth, unsigned int aIndex) {

	if (aIndex < mChildren.size()) {
		CSSView * childView = mChildren[aIndex];
		childView->SetWidth(aWidth);
	}

}

unsigned int CSSView :: GetLength() const {
	
	return mChildren.size();

}
