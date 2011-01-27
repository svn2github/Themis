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

/*	CSSScrolledRendererWindow
	Renders an HTML file through css in a scrollable view
	
	Mark Hellegers (mark@firedisk.net)
	16-01-2011
	
*/

#ifndef CSSSCROLLEDRENDERERVIEW_HPP
#define CSSSCROLLEDRENDERERVIEW_HPP

// BeOS headers
#include <View.h>

// DOM headers
#include "DOMSupport.h"

// DOM Style headers
#include "CSSStyleSheet.hpp"

// Declarations used
class CSSRendererView;
class BScrollView;


class CSSScrolledRendererView : public BView {
	
	private:
		BScrollView * mScrollView;
		CSSRendererView * mView;
		CSSStyleSheetPtr mStyleSheet;
		TDocumentPtr mDocument;
		void SetScrollbars();
	
	public:
		CSSScrolledRendererView(TDocumentPtr aDocument,
								CSSStyleSheetPtr aStyleSheet,
								float aWidth,
								float aHeight);
		~CSSScrolledRendererView();
		void FrameResized(float aWidth, float aHeight);

};

#endif
