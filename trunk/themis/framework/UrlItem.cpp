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
	Class Start Date: June 4, 2011
*/

/*	UrlItem implementation
	See UrlItem.hpp for more information
	
*/

// BeOS headers
#include "be/interface/GraphicsDefs.h"
#include "be/interface/InterfaceDefs.h"
#include "be/interface/View.h"
#include "be/support/String.h"

// Themis headers
#include "UrlItem.hpp"

UrlItem :: UrlItem(const char * aUrl)
		: BStringItem(aUrl) {

}

void UrlItem :: DrawItem(BView * aOwner,
						 BRect aFrame,
						 bool aComplete) {

	rgb_color color;
	if (IsSelected() || aComplete) {
		if (IsSelected()) {
			color = ui_color(B_MENU_BACKGROUND_COLOR);
		}
		else {
			color = aOwner->ViewColor();
		}
		aOwner->SetHighColor(color);
		aOwner->FillRect(aFrame);
	}

	color = ui_color(B_MENU_ITEM_TEXT_COLOR);
	aOwner->SetHighColor(color);
	BString url(Text());
	aOwner->TruncateString(
		&url,
		B_TRUNCATE_END,
		aFrame.right - 10);
	
	aOwner->MovePenTo(aFrame.left + 4, aFrame.bottom - 2);
	aOwner->DrawString(url.String());

}
