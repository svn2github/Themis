/*
	Copyright (c) 2011 Mark Hellegers. All Rights Reserved.
	
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
	Class Start Date: January 16, 2011
*/

/*	CSSScrolledRendererView implementation
	See CSSScrolledRendererView.hpp for more information
	
*/

// Standard C headers
#include <stdio.h>

// BeOS headers
#include <ScrollView.h>

// DOM headersa
#include "TNode.h"
#include "TDocument.h"

// CSS Renderer headers
#include "CSSScrolledRendererView.hpp"
#include "CSSRendererView.hpp"

CSSScrolledRendererView :: CSSScrolledRendererView(TDocumentPtr aDocument,
												   CSSStyleSheetPtr aStyleSheet,
												   float aWidth,
												   float aHeight)
				  		: BView(BRect(0, 0, aWidth, aHeight),
							"CSSScrolledRendererView",
							B_FOLLOW_ALL_SIDES,
							B_WILL_DRAW | B_FRAME_EVENTS | B_FULL_UPDATE_ON_RESIZE) {

	printf("Creating CSS Scrolled Renderer View\n");
	mStyleSheet = aStyleSheet;
	mView = NULL;
	mDocument = aDocument;
	BRect rect = Bounds();
	rect.bottom -= B_H_SCROLL_BAR_HEIGHT;
	rect.right -= B_V_SCROLL_BAR_WIDTH;
	mView = new CSSRendererView(rect, mDocument, mStyleSheet);
	mScrollView = new BScrollView("CSS Scroll view",
								  mView,
								  B_FOLLOW_ALL_SIDES,
								  0,
								  true,
								  true);
	mScrollView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	AddChild(mScrollView);

	SetScrollbars();

}
				
CSSScrolledRendererView :: ~CSSScrolledRendererView() {

	printf("Destroying CSSScrolledRendererView\n");
/*
	if (mScrollView) {
		RemoveChild(mScrollView);
		delete mScrollView;
		delete mView;
	}
*/
	printf("Destroyed CSSScrolledRendererView\n");

}

void CSSScrolledRendererView :: SetScrollbars() {

	BRect rect = Bounds();
	rect.bottom -= B_H_SCROLL_BAR_HEIGHT;
	rect.right -= B_V_SCROLL_BAR_WIDTH;

	BScrollBar * bar = mScrollView->ScrollBar(B_VERTICAL);
	BRect newRect = mView->CSSViewSize();
//	printf("Vertical: Old height: %f, new height: %f\n", rect.Height(), newRect.Height());
	if (newRect.Height() > rect.Height()) {
		bar->SetRange(0, newRect.Height() - rect.bottom);
		bar->SetProportion(rect.Height() / newRect.Height());
	}
	else {
		bar->SetRange(0, 0);
	}
	bar = mScrollView->ScrollBar(B_HORIZONTAL);
//	printf("Horizontal: Old width: %f, new width: %f\n", rect.Width(), newRect.Width());
	if (newRect.Width() > rect.Width()) {
		bar->SetRange(0, newRect.Width() - rect.right);
		bar->SetProportion(rect.Width() / newRect.Width());
	}
	else {
		bar->SetRange(0, 0);
	}
}

void CSSScrolledRendererView :: FrameResized(float aWidth, float aHeight) {

	SetScrollbars();
	
}
