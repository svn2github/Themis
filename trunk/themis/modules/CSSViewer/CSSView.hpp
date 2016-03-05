/*
	Copyright (c) 2001 Mark Hellegers. All Rights Reserved.
	
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
	Class Start Date: November 22, 2009
*/

#ifndef CSSVIEW_HPP
#define CSSVIEW_HPP

// Standard C++ headers
#include <string>

// BeOS headers
#include <SupportDefs.h>
#include <Handler.h>
#include <Window.h>

// DOM Style headers
#include "CSSStyleSheet.hpp"

// Declarations of Themis classes
class TTextView;
class BColumnListView;
class TOutlineListView;

class CSSView : public BWindow {

	private:
		TOutlineListView * mTree;
		CSSStyleSheetPtr mStyleSheet;
		BStringItem * mMediaItem;
		BStringItem * mGeneralItem;
		TTextView * mSelectorText;
		BColumnListView * mPropertiesView;
		
		void ShowStyleSheet();
		void AddRules(CSSRuleListPtr aList, BStringItem * aParent);
		void CreatePropertiesListView(BRect aRect);
		void SetPropertiesOfSelection(TDOMString aSelectorText,
									  unsigned short aRuleType,
									  TDOMString aMedium = "");
	
	public:
		CSSView(CSSStyleSheetPtr aDocument);
		~CSSView();
		void MessageReceived(BMessage * aMessage);
		bool QuitRequested();
		void SetStyleSheet(CSSStyleSheetPtr aStyleSheet);
		
	
};

#endif
