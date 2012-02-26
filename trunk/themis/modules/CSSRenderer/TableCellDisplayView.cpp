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
								 			 BFont * aFont)
				 	  : CSSView(aBaseView,
				 				aNode,
					 			aStyleSheets,
					 			aStyle,
				 				aRect,
				 				aSiteId,
				 				aUrlId,
				 				aColor,
				 				aFont) {

	mDisplay = true;
	mBlock = false;
	mRequestedWidth = -1;

}

TableCellDisplayView :: ~TableCellDisplayView() {

}

void TableCellDisplayView :: Layout(BRect aRect,
									BPoint aStartingPoint) {

	CSSView::Layout(aRect, aStartingPoint);

	if (mRequestedWidth > -1) {
		mRect.right = mRect.left + mRequestedWidth;
		mEndPoint.x = mRect.right;
	}

}
