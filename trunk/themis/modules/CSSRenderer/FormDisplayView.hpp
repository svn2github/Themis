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

/*	ButtonDisplayView
	Renders button elements
	
	Mark Hellegers (mark@firedisk.net)
	25-03-2012
	
*/

#ifndef FORMDISPLAYVIEW_HPP
#define FORMDISPLAYVIEW_HPP

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
class InputDisplayView;

// Namespaces used
using std::vector;
using std::string;

// Constants used
#define INPUT_BUTTON_CLICKED 'ibcl'

class FormDisplayView : public CSSView {

	private:
		vector<InputDisplayView *> mInputChildren;

	public:
		FormDisplayView(
			CSSRendererView * aBaseView,
			TNodePtr aNode,
			CSSStyleContainer * aStyleSheets,
			CSSStyleDeclarationPtr aStyle,
			BRect aRect,
			int32 aSiteId,
			int32 aUrlId,
			rgb_color aColor,
			BFont * aFont = NULL,
			WhiteSpaceType aWhiteSpace = NORMAL,
			BHandler * aForm = NULL);
		~FormDisplayView();
		virtual void MessageReceived(BMessage * aMessage);
			
		virtual void Layout(
			BRect aRect,
			BPoint aStartingPoint);
		void AddInputChild(InputDisplayView * aView);
};

#endif
