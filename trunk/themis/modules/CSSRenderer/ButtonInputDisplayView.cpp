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
	Class Start Date: March 24, 2012
*/

/*	ButtonInputDisplayView implementation
	See ButtonInputDisplayView.hpp for more information
	
*/

// Standard C headers
#include <stdio.h>

// BeOS headers
#include <Button.h>
#include <Window.h>

// DOM headers
#include "TNode.h"
#include "TElement.h"

// CSS Renderer headers
#include "ButtonInputDisplayView.hpp"
#include "CSSRendererView.hpp"
#include "FormDisplayView.hpp"

ButtonInputDisplayView :: ButtonInputDisplayView(
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
	: CSSView(
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
	mButton = NULL;

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

}

ButtonInputDisplayView :: ~ButtonInputDisplayView() {

	delete mButton;

}

void ButtonInputDisplayView :: Layout(
	BRect aRect,
	BPoint aStartingPoint) {

	if (mButton) {
		mButton->MoveTo(aStartingPoint);
	}
	else {
		TElementPtr element = shared_static_cast<TElement>(mNode);
		string buttonLabel = "";
		if (element->hasAttribute("VALUE")) {
			buttonLabel = element->getAttribute("VALUE");
		}
		string buttonName = "";
		if (element->hasAttribute("NAME")) {
			buttonName = element->getAttribute("NAME");
		}
		
		BPoint rightBottom(aStartingPoint.x, aStartingPoint.y);
		BRect buttonRect(aStartingPoint, rightBottom);
		BMessage * message = new BMessage(INPUT_BUTTON_CLICKED);
		message->AddString("name", buttonName.c_str());
		message->AddString("value", buttonLabel.c_str());
		mButton =
			new BButton(
				buttonRect,
				"ThemisButton",
				buttonLabel.c_str(),
				message);
		mButton->ResizeToPreferred();
		mRect = mButton->Frame();
		mEndPoint = mRect.RightTop();
		mBaseView->AddChild(mButton);
	}

}

void ButtonInputDisplayView :: AttachedToWindow() {

	CSSView::AttachedToWindow();
	mButton->SetTarget(mForm);

}
