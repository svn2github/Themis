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

/*	FormDisplayView implementation
	See FormDisplayView.hpp for more information
	
*/

// Standard C headers
#include <stdio.h>

// BeOS headers
#include <Message.h>
#include <String.h>

// DOM headers
#include "TNode.h"
#include "TElement.h"

// CSS Renderer headers
#include "FormDisplayView.hpp"
#include "CSSRendererView.hpp"
#include "InputDisplayView.hpp"

FormDisplayView :: FormDisplayView(
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
		this) {

	mDisplay = true;
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

}

FormDisplayView :: ~FormDisplayView() {

}

void FormDisplayView :: MessageReceived(BMessage * aMessage) {

	switch(aMessage->what) {
		case INPUT_BUTTON_CLICKED: {
			aMessage->PrintToStream();
			BString formAction = "";
			TElementPtr element = shared_static_cast<TElement>(mNode);
			if (element->hasAttribute("ACTION")) {
				formAction = element->getAttribute("ACTION").c_str();
				BString name = "";
				BString value = "";
				aMessage->FindString("name", &name);
				aMessage->FindString("value", &value);
				name.ReplaceAll(" ", "+");
				value.ReplaceAll(" ", "+");
				formAction += "?";
				formAction += name;
				formAction += "=";
				formAction += value;
				unsigned int length = mInputChildren.size();
				for (unsigned int i = 0; i < length; i++) {
					formAction += "&";
					formAction += mInputChildren[i]->GetName().c_str();
					formAction += "=";
					formAction += mInputChildren[i]->GetValue().c_str();
				}
				printf("URL: %s\n", formAction.String());
				mHref = formAction.String();
				RetrieveLink();
				
			}
			break;
		}
		default: {
			CSSView::MessageReceived(aMessage);
		}
	}
	
}

void FormDisplayView :: Layout(
	BRect aRect,
	BPoint aStartingPoint) {

	CSSView::Layout(aRect, aStartingPoint);

}

void FormDisplayView :: AddInputChild(InputDisplayView * aView) {
	
	mInputChildren.push_back(aView);
	
}
