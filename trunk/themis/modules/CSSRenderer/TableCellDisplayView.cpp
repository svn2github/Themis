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

/*	TableCellDisplayView implementation
	See TableCellDisplayView.hpp for more information
	
*/

// Standard C headers
#include <stdio.h>

// CSS Renderer headers
#include "TableCellDisplayView.hpp"

TableCellDisplayView :: TableCellDisplayView(CSSRendererView * aBaseView,
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
	mRequestedWidth = -1;

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

TableCellDisplayView :: ~TableCellDisplayView() {

}

void TableCellDisplayView :: Layout(BRect aRect,
									BPoint aStartingPoint) {

	mRect = aRect;
	mRect.left += mMarginLeft;
	// Enforce the requested width if set
	if (mRequestedWidth > -1) {
		mRect.right = mRect.left + mRequestedWidth;
	}
	// Always set the top of the rect to the one from the starting point as that is definitely correct.
	mRect.top = aStartingPoint.y;
	mRect.left = aStartingPoint.x;
	BRect restRect = mRect;
	// If we are drawing a border around this element, make sure there is less space for the content.
	restRect.InsetBy(mBorderWidth, mBorderWidth);
	//mEndPoint.Set(restRect.left, restRect.top);
	// Set the endpoint to the starting point, as that is the minimum endpoint
	mEndPoint.x = restRect.left;
	mEndPoint.y = restRect.top;
	// Assume we don't need any horizontal space. The children will determine the space needed.
//			mRect.right = mRect.left;
	float maxRight = 0;
	// In case we need to draw something before drawing any children, move the children.
	if (mListStyleType == "square") {
		mListStyleRect.left = restRect.left + 2;
		mListStyleRect.right = restRect.left + 7;
		mListStyleRect.top = restRect.top + 2;
		mListStyleRect.bottom = restRect.top + 7;
		restRect.left += 12;
		mEndPoint.x += 12;
		maxRight = mListStyleRect.right;
	}
	BPoint startingPoint = mEndPoint;
	// Layout the children.
	unsigned int length = mChildren.size();
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
			if (restRect.top < rect2.bottom) {
				restRect.top = rect2.bottom;
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

	mRect.bottom = restRect.top + 1;
//			mRect.PrintToStream();

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
	mRects.push_back(mRect);

}
