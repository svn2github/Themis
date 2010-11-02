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

/*	BasePrefsView implementation
	See BasePrefsView.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// BeOS headers
#include <interface/Window.h>
#include <interface/Box.h>
#include <interface/PopUpMenu.h>
#include <interface/MenuItem.h>
#include <interface/MenuField.h>
#include <storage/Directory.h>
#include <storage/Entry.h>
#include <storage/Path.h>

// Themis headers
#include "BasePrefsView.hpp"
#include "commondefs.h"
#include "PrefsDefs.h"

BasePrefsView :: BasePrefsView(BRect aFrame,
							   const char* aName)
			  : BView(aFrame,
					  aName,
					  B_FOLLOW_ALL,
					  0) {

	mMainBox = new BBox(
		Bounds(),
		"MainBox",
		B_FOLLOW_ALL);
	mMainBox->SetLabel(Name());
	AddChild(mMainBox);

	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

}
