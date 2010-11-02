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
	Class Start Date: October 31, 2010
*/

/*	HTMLParserPrefsView
	Contains the preferences view for the HTML parser addon.

	Mark Hellegers (mark@firedisk.net)
	31-10-2010
*/

#ifndef HTMLPARSERPREFSVIEW_HPP
#define HTMLPARSERPREFSVIEW_HPP

// BeOS headers
#include <String.h>

// Themis headers
#include "BasePrefsView.hpp"

// Declarations used
class BPopUpMenu;
class BHandler;

/// Class to view the preferences of the HTML parser addon.

/**
	This class is a view of the preferences of the HTML parser addon.
*/

class HTMLParserPrefsView : public BasePrefsView {

	private:
		BPopUpMenu * mPopUpMenu;
		BString mDTDDir;
		BHandler * mPlugin;

	public:
		HTMLParserPrefsView(BRect aFrame,
							const char* aName,
							BHandler * aPlugin);
		virtual void AttachedToWindow();

};

#endif

