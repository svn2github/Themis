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
	Class Start Date: March 27, 2012
*/

/*	InputDisplayView implementation
	See InputDisplayView.hpp for more information
	
*/

// Standard C headers
#include <stdio.h>

// DOM headers
#include "TNode.h"
#include "TElement.h"

// CSS Renderer headers
#include "HiddenInputDisplayView.hpp"
#include "CSSRendererView.hpp"
#include "FormDisplayView.hpp"

HiddenInputDisplayView :: HiddenInputDisplayView(
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

	mDisplay = false;
	mBlock = false;

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

	TElementPtr element = shared_static_cast<TElement>(mNode);
	if (element->hasAttribute("NAME")) {
		mInputName = element->getAttribute("NAME");
	}

}

HiddenInputDisplayView :: ~HiddenInputDisplayView() {

}

void HiddenInputDisplayView :: Layout(
	BRect aRect,
	BPoint aStartingPoint) {

	CSSView::Layout(aRect, aStartingPoint);

}

string HiddenInputDisplayView :: GetValue() const {
	
	string result = "";
	
	TElementPtr element = shared_static_cast<TElement>(mNode);
	if (element->hasAttribute("VALUE")) {
		result = element->getAttribute("VALUE");
	}

	return result;
	
}
