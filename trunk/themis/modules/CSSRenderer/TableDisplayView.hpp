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

/*	TableDisplayView
	Renders css table display elements
	
	Mark Hellegers (mark@firedisk.net)
	23-02-2012
	
*/

#ifndef TABLEDISPLAYVIEW_HPP
#define TABLEDISPLAYVIEW_HPP

// Standard C++ headers
#include <vector>
#include <string>

// BeOS headers
#include <Handler.h>
#include <Rect.h>
#include <Point.h>

// DOM headers
#include "DOMSupport.h"

// DOM Style headers
#include "CSSStyleSheet.hpp"
#include "CSSStyleDeclaration.hpp"

// CSSRenderer headers
#include "CSSView.hpp"
#include "TextBox.hpp"

// Declarations used
class BFont;
class CSSRendererView;
class CSSStyleContainer;

// Namespaces used
using std::vector;
using std::string;

class TableDisplayView : public CSSView {
	
	public:
		TableDisplayView(CSSRendererView * aBaseView,
						 TNodePtr aNode,
						 CSSStyleContainer * aStyleSheets,
						 CSSStyleDeclarationPtr aStyle,
						 BRect aRect,
						 int32 aSiteId,
						 int32 aUrlId,
						 rgb_color aColor,
						 BFont * aFont = NULL);
		~TableDisplayView();
		virtual void Layout(BRect aRect,
							BPoint aStartingPoint);
};

#endif
