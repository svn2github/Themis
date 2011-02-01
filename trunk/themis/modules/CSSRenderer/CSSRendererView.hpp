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

/*	CSSRendererView
	Renders an HTML file through css
	
	Mark Hellegers (mark@firedisk.net)
	28-02-2010
	
*/

#ifndef CSSRENDERERVIEW_HPP
#define CSSRENDERERVIEW_HPP

// BeOS headers
#include <View.h>
#include <Point.h>

// DOM headers
#include "DOMSupport.h"

// DOM Style headers
#include "CSSStyleSheet.hpp"

// Declarations used
class CSSView;

class CSSRendererView : public BView {
	
	private:
		CSSView * mView;
		TDOMString mTitle;
		TDocumentPtr mDocument;
		BRect mCSSViewSize;
		CSSStyleSheetPtr mStyleSheet;
	
	public:
		CSSRendererView(BRect aFrame,
						TDocumentPtr aDocument,
						CSSStyleSheetPtr aStyleSheet);
		~CSSRendererView();
		virtual void MouseDown(BPoint aPoint);
		virtual void Draw(BRect aRect);
		void AttachedToWindow();
		virtual void FrameResized(float aWidth, float aHeight);
		void SetTitle(TDOMString aTitle);
		TDOMString GetTitle();
		BRect CSSViewSize();
		TDOMString GetDocumentURI() const;
};

#endif
