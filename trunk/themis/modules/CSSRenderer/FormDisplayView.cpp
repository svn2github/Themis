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

	mRects.clear();
	if (mDisplay) {
		float maxRight = 0;
		float maxBottom = 0;
		mRect = aRect;
		// Always set the top and left of the rect to the one from the starting point
		// as that is definitely correct.
		mRect.top = aStartingPoint.y;
		mRect.left = aStartingPoint.x;
		BRect restRect = aRect;
		// Add the left margin.
		mRect.left += mMarginLeft;
		// Set the endpoint to the starting point, as that is the minimum endpoint
		mEndPoint = aStartingPoint;
		// Enforce the requested width if set
		if (mRequestedWidth > -1) {
			mRect.right = mRect.left + mRequestedWidth;
			maxRight = mRect.right;
			if (maxRight > restRect.right) {
				restRect.right = maxRight;
			}
			mEndPoint.x = mRect.right + 1;
		}
		// Enforce the requested height if set
		if (mRequestedHeight > -1) {
			mRect.bottom = mRect.top + mRequestedHeight;
			maxBottom = mRect.bottom;
			if (maxBottom > restRect.bottom) {
				restRect.bottom = maxBottom;
			}
		}
		// If we are drawing a border around this element, make sure there is less space for the content.
		restRect.InsetBy(mBorderWidth, mBorderWidth);
		//mEndPoint.Set(restRect.left, restRect.top);
		// In case we need to draw something before drawing any children, move the children.
		if (mListStyleType == "square") {
			mListStyleRect.left = restRect.left + 2;
			mListStyleRect.right = restRect.left + 7;
			mListStyleRect.top = restRect.top + 2;
			mListStyleRect.bottom = restRect.top + 7;
			restRect.left += 12;
			mEndPoint.x += 12;
			maxRight = mListStyleRect.right;
			maxBottom = mListStyleRect.bottom;
		}
		
		// Layout the children.
		unsigned int length = mChildren.size();
		BPoint startingPoint = mEndPoint;
		for (unsigned int i = 0; i < length; i++) {
			CSSView * childView = mChildren[i];
			// Only layout those children that are actually displayed.
			if (childView->IsDisplayed()) {
				// Do the layout for the child.
				childView->Layout(restRect, startingPoint);
				BRect rect2 = childView->Bounds();
				mEndPoint = childView->GetEndPoint();
				// Set the top of the remaining rect to the endpoint of the child,
				// because the space above is already taken by the child.
				if (restRect.top < mEndPoint.y) {
					restRect.top = mEndPoint.y;
				}
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
				// What is the maximum vertical space being taken up by one of the children?
				if (rect2.bottom > maxBottom) {
					maxBottom = rect2.bottom;
				}
				// We can start where we left off.
				startingPoint = mEndPoint;
				//	previousChildIsBlock = false;
//				printf("Starting at: ");
//				mEndPoint.PrintToStream();
			}
		}
		if (maxRight > 0) {
			if ((mRequestedWidth == -1) || (mRect.left + mRequestedWidth < maxRight)) {
				// Only set it in case we found something
				// and it is larger than the requested width.
				mRect.right = maxRight;
				//mEndPoint.x = maxRight;
			}
		}
		else {
			mRect.right = aStartingPoint.x;
			mEndPoint.x = aStartingPoint.x;
		}

		if (maxBottom > 0) {
			if ((mRequestedHeight == -1) ||
				(mRect.top + mRequestedHeight < maxBottom)) {
				// Only set it in case we found something
				// and it is larger than the requested width.
				mRect.bottom = maxBottom;
			}
		}
		else {
			mRect.bottom = aStartingPoint.y;
			mEndPoint.y = aStartingPoint.y;
		}
//			mRect.PrintToStream();

		// Add any margins
		mEndPoint.Set(mEndPoint.x + mMarginRight, mEndPoint.y + mMarginBottom);
		
		mRect.bottom += mMarginBottom;
		mRect.right += mMarginRight;

		//mRect.right = restRect.right;
		mRects.push_back(mRect);
	}
	else {
		// Only in case we are the first element, otherwise this is never called.
		mRect = BRect(0, 0, 0, 0);
	}

}

void FormDisplayView :: AddInputChild(InputDisplayView * aView) {
	
	mInputChildren.push_back(aView);
	
}
