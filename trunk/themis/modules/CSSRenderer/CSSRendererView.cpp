/*
	Copyright (c) 2010 Mark Hellegers. All Rights Reserved.
	
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
	Class Start Date: Februari 28, 2010
*/

/*	CSSRendererView implementation
	See CSSRendererView.hpp for more information
	
*/

// Standard C headers
#include <stdio.h>

// BeOS headers
#include <Window.h>

// DOM headersa
#include "TNode.h"
#include "TDocument.h"

// CSS Renderer headers
#include "CSSRendererView.hpp"
#include "CSSView.hpp"
#include "CSSStyleContainer.hpp"

CSSRendererView :: CSSRendererView(BRect aFrame,
								   TDocumentPtr aDocument,
								   CSSStyleSheetPtr aStyleSheet)
				: BView(aFrame,
						"CSSRendererView",
						B_FOLLOW_ALL_SIDES,
						B_WILL_DRAW | B_FRAME_EVENTS | B_FULL_UPDATE_ON_RESIZE) {

	printf("Creating CSSRendererView\n");
	mStyleSheets = new CSSStyleContainer();
	mStyleSheets->addStyleSheet(aStyleSheet);
	mDocument = aDocument;
	BRect rect = Bounds();
	if (mDocument->hasChildNodes()) {
		rgb_color defaultColor;
		defaultColor.red = 0;
		defaultColor.green = 0;
		defaultColor.blue = 0;
		
				
		TNodePtr root = mDocument->getFirstChild();
		
		mView = new CSSView(this,
							root,
							mStyleSheets,
							rect,
							defaultColor);
		printf("Doing layout\n");
		mView->Layout(rect, BPoint(rect.left, rect.top));
		printf("Layout done\n");
		
		mCSSViewSize = mView->Bounds();
	}
	else {
		mView = NULL;
	}
}

CSSRendererView :: ~CSSRendererView() {

	printf("Destroying CSSRendererView\n");
	delete mView;
	delete mStyleSheets;
	printf("Destroyed CSSRendererView\n");

}

void CSSRendererView :: MouseDown(BPoint aPoint) {
	
	if (mView) {
		mView->MouseDown(aPoint);
	}
	
}

void CSSRendererView :: Draw(BRect aRect) {
	
//	printf("Drawing in view\n");
	FillRect(Bounds(), B_SOLID_LOW);
	if (mView) {
		mView->Draw();
	}
	
}

void CSSRendererView :: AttachedToWindow() {

	TDOMString title = "Themis - ";

	if (mTitle == "") {
		title += mDocument->getDocumentURI();
	}
	else {
		title += mTitle;
	}
	Window()->SetTitle(title.c_str());
	SetViewColor(B_TRANSPARENT_COLOR);

}

void CSSRendererView :: FrameResized(float aWidth, float aHeight) {

	BRect frame = BRect(0, 0, aWidth, aHeight);
	mView->Layout(frame, BPoint(frame.left, frame.top));

}

void CSSRendererView :: SetTitle(TDOMString aTitle) {
	
	mTitle = aTitle;
	if (Window() != NULL) {
		Window()->SetTitle(mTitle.c_str());
	}

}

TDOMString CSSRendererView :: GetTitle() {
	
	return mTitle;

}

BRect CSSRendererView :: CSSViewSize() {

	return mCSSViewSize;

}

TDOMString CSSRendererView :: GetDocumentURI() const {

	return mDocument->getDocumentURI();

}

