/*
	Copyright (c) 2012 Mark Hellegers. All Rights Reserved.
	
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
	Class Start Date: March 25, 2012
*/

/*	TextInputDisplayView implementation
	See TextInputDisplayView.hpp for more information
	
*/

// Standard C headers
#include <stdio.h>

// BeOS headers
#include <TextControl.h>
#include <String.h>

// DOM headers
#include "TNode.h"
#include "TElement.h"

// CSS Renderer headers
#include "TextInputDisplayView.hpp"
#include "CSSRendererView.hpp"
#include "FormDisplayView.hpp"

TextInputDisplayView :: TextInputDisplayView(
	CSSRendererView * aBaseView,
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
	: InputDisplayView(
		aBaseView,
		aNode,
		aStyleSheets,
		aStyle,
		aRect,
		aSiteId,
		aUrlId,
		aColor,
		aFont,
		aWhiteSpace,
		aForm) {

	mDisplay = true;
	mBlock = false;
	mTextInput = NULL;

	CreateChildren(
		aBaseView,
		aNode,
		aStyleSheets,
		aStyle,
		aRect,
		aSiteId,
		aUrlId,
		aColor,
		aFont,
		aWhiteSpace,
		aForm);

	if (aForm) {
		FormDisplayView * formView = dynamic_cast<FormDisplayView *> (aForm);
		formView->AddInputChild(this);
	}

}

TextInputDisplayView :: ~TextInputDisplayView() {

	delete mTextInput;

}

void TextInputDisplayView :: Layout(
	BRect aRect,
	BPoint aStartingPoint) {

	if (mTextInput) {
		mTextInput->MoveTo(aStartingPoint);
	}
	else {
		TElementPtr element = shared_static_cast<TElement>(mNode);
		if (element->hasAttribute("NAME")) {
			mInputName = element->getAttribute("NAME");
		}
		string sizeString = "";
		if (element->hasAttribute("SIZE")) {
			sizeString = element->getAttribute("SIZE");
		}
		int size = atoi(sizeString.c_str());
		if (size == 0) {
			// Set a default of 10 characters
			size = 10;
		}
		// Get the "biggest" character width and multiply by the size
		string bigChar = "W";
		if (mFont == NULL) {
			mFont = new BFont(be_plain_font);
			mInheritedFont = false;
		}
		float inputWidth = mFont->StringWidth(bigChar.c_str()) * size;
		// Get the height of the current font
		font_height height;
		mFont->GetHeight(&height);
		float lineHeight = height.ascent + height.descent + height.leading;
		
		BPoint rightBottom(aStartingPoint.x + inputWidth, aStartingPoint.y + lineHeight);
		BRect textRect(aStartingPoint, rightBottom);
		mTextInput =
			new BTextControl(
				textRect,
				"ThemisTextInput",
				NULL,
				"",
				NULL);
//		mButton->ResizeToPreferred();
		mRect = mTextInput->Frame();
		mEndPoint = mRect.RightTop();
		mBaseView->AddChild(mTextInput);
	}

}

string TextInputDisplayView :: GetValue() const {
	
	string result = "";
	
	if (mTextInput) {
		BString inputText = mTextInput->Text();
		inputText.ReplaceAll(" ", "+");
		result = inputText.String();
	}
	
	return result;
	
}
