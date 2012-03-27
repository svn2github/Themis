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
	Class Start Date: Februari 23, 2012
*/

/*	TableRowDisplayView implementation
	See TableRowDisplayView.hpp for more information
	
*/

// Standard C headers
#include <stdio.h>

// CSS Renderer headers
#include "TableRowDisplayView.hpp"

TableRowDisplayView :: TableRowDisplayView(CSSRendererView * aBaseView,
										   TNodePtr aNode,
										   CSSStyleContainer * aStyleSheets,
										   CSSStyleDeclarationPtr aStyle,
										   BRect aRect,
										   int32 aSiteId,
										   int32 aUrlId,
										   rgb_color aColor,
								 		   BFont * aFont,
										   WhiteSpaceType aWhiteSpace = NORMAL,
										   BHandler * aForm)
				 	: CSSView(aBaseView,
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

TableRowDisplayView :: ~TableRowDisplayView() {

}

void TableRowDisplayView :: Layout(BRect aRect,
								   BPoint aStartingPoint) {

	mRects.clear();
	mRect = aRect;
	// Always set the top of the rect to the one from the starting point as that is definitely correct.
	mRect.top = aStartingPoint.y;
	BRect restRect = mRect;
	// If we are drawing a border around this element, make sure there is less space for the content.
	restRect.InsetBy(mBorderWidth, mBorderWidth);
	// Set the endpoint to the starting point, as that is the minimum endpoint
	mEndPoint = aStartingPoint;
	// Assume we don't need any horizontal space. The children will determine the space needed.
//	mRect.right = mRect.left;
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

	BRect restRowRect = restRect;
	float maxBottom = restRowRect.top;
	for (unsigned int i = 0; i < length; i++) {
		CSSView * childView = mChildren[i];
		// Only layout those children that are actually displayed.
		if (childView->IsDisplayed()) {
			// Don't care if the child is a block or not, children will be positioned next to each other.
			startingPoint = BPoint(restRowRect.left, restRowRect.top);
			// Do the layout for the child.
			childView->Layout(restRowRect, startingPoint);
			BRect rect2 = childView->Bounds();
			mEndPoint = childView->GetEndPoint();
			if (rect2.right > restRect.right) {
				// The child used more space than was available. We can use that space now as well
				// for any remaining children.
				restRect.right = rect2.right;
				mRect.right = rect2.right;
				restRowRect.right = rect2.right;
			}
			// What is the maximum horizontal space being taken up by one of the children?
			if (rect2.right > maxRight) {
				maxRight = rect2.right;
			}
			// What is the maximum vertical space being taken up by one of the children?
			if (rect2.bottom > maxBottom) {
				maxBottom = rect2.bottom;
			}
			// Make sure the next child starts to the right of this one.
			restRowRect.left = rect2.right;
		}
	}
	if (maxBottom > 0) {
		// Only set it in case we found something.
		restRect.top = maxBottom;
	}

	if (maxRight > 0) {
		// Only set it in case we found something.
		mRect.right = maxRight;
	}
	else {
		mRect.right = mRect.left;
	}
	mRect.bottom = restRect.top;
//	mRect.PrintToStream();

	// Calculating the cell heights.
	float maxHeight = 0;
	float childHeight = 0;
	for (unsigned int i = 0; i < length; i++) {
		CSSView * childView = mChildren[i];
		BRect childRect = childView->Bounds();
		childHeight = childRect.Height();
		if (maxHeight < childHeight) {
			maxHeight = childHeight;
		}
	}
	
	// Only setting them to the max height.
	// Counting on the row group parent to force a recalculation of the layout for now.
	for (unsigned int i = 0; i < length; i++) {
		CSSView * childView = mChildren[i];
		childView->SetHeight(maxHeight);
	}

	// Add any margins
	mRect.bottom += mMarginBottom;
	mRect.right += mMarginRight;
	mEndPoint.Set(mRect.right, mRect.bottom);
	//mRect.right = restRect.right;
	mRects.push_back(mRect);

}
