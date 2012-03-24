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

/*	TableDisplayView implementation
	See TableDisplayView.hpp for more information
	
*/

// Standard C headers
#include <stdio.h>

// CSS Renderer headers
#include "TableDisplayView.hpp"
#include "TableGroupDisplayView.hpp"

TableDisplayView :: TableDisplayView(CSSRendererView * aBaseView,
									 TNodePtr aNode,
									 CSSStyleContainer * aStyleSheets,
									 CSSStyleDeclarationPtr aStyle,
									 BRect aRect,
									 int32 aSiteId,
									 int32 aUrlId,
									 rgb_color aColor,
								 	 BFont * aFont,
									 WhiteSpaceType aWhiteSpace)
				 : CSSView(aBaseView,
				 		   aNode,
				 		   aStyleSheets,
				 		   aStyle,
				 		   aRect,
				 		   aSiteId,
				 		   aUrlId,
				 		   aColor,
				 		   aFont,
				 		   aWhiteSpace) {

	mDisplay = true;
	mBlock = true;

}

TableDisplayView :: ~TableDisplayView() {

}

void TableDisplayView :: LayoutChildren(
	BRect aRect,
	BPoint aStartingPoint) {

	unsigned int length = mChildren.size();
	BPoint startingPoint = aStartingPoint;
	BRect restRect = aRect;
	float maxRight = 0;

	for (unsigned int i = 0; i < length; i++) {
		CSSView * childView = mChildren[i];
		// Only layout those children that are actually displayed.
		if (childView->IsDisplayed()) {
			// Don't care if the child is a block or not, children will be positioned below to each other.
			startingPoint = BPoint(restRect.left, restRect.top);
			// Do the layout for the child.
			childView->Layout(restRect, startingPoint);
			BRect rect2 = childView->Bounds();
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
		}
	}
	
	if (maxRight > 0) {
		// Only set it in case we found something.
		mRect.right = maxRight;
	}
	else {
		mRect.right = mRect.left;
	}
	mRect.bottom = restRect.top;

}

void TableDisplayView :: Layout(BRect aRect,
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
	LayoutChildren(restRect, mEndPoint);
	unsigned int length = mChildren.size();

	vector<float> widths;
	for (unsigned int i = 0; i < length; i++) {
		CSSView * childView = mChildren[i];
		TableGroupDisplayView * childGroupView = dynamic_cast<TableGroupDisplayView *> (childView);
		if (childGroupView) {
			unsigned int childLength = childGroupView->GetColLength();
			for (unsigned int j = 0; j < childLength; j++) {
				if (j >= widths.size()) {
					widths.push_back(childGroupView->GetColWidth(j));
				}
				else if (childGroupView->GetColWidth(j) > widths[j]) {
					widths[j] = childGroupView->GetColWidth(j);
				}
			}
		}
	}

	for (unsigned int i = 0; i < length; i++) {
		CSSView * childView = mChildren[i];
		TableGroupDisplayView * childGroupView = dynamic_cast<TableGroupDisplayView *> (childView);
		if (childGroupView) {
			unsigned int childLength = childGroupView->GetColLength();
			for (unsigned int j = 0; j < childLength; j++) {
				childGroupView->SetColWidth(widths[j], j);
			}
		}
	}

	// Do another layout the children based on the new col widths.
	LayoutChildren(restRect, mEndPoint);

	// Add any margins
	mRect.bottom += mMarginBottom;
	mRect.right += mMarginRight;
	mEndPoint.Set(mRect.right, mRect.bottom);
	mRects.push_back(mRect);
	//mRect.right = restRect.right;
	
}

