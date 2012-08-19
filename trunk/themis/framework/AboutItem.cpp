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
	Class Start Date: August 19, 2012
*/

/*	AboutItem implementation
	See AboutItem.hpp for more information
	
*/

// Standard C headers
#include <stdio.h>

// BeOS headers
#include <GraphicsDefs.h>
#include <InterfaceDefs.h>
#include <View.h>

// Themis headers
#include "AboutItem.hpp"

AboutItem :: AboutItem(const char * aAboutText, AboutType aAboutType)
		: BListItem() {

	mAboutText = BString(aAboutText);
	mAboutType = aAboutType;

}

void AboutItem :: DrawItem(BView * aOwner,
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
	aOwner->MovePenTo(aFrame.left + 4 + 19, aFrame.bottom - 2);
	aOwner->DrawString(mAboutText.String());

}

const char * AboutItem :: Text() const {
	
	return mAboutText.String();

}

AboutType AboutItem :: Type() const {
	
	return mAboutType;
	
}
