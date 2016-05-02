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
	Class Start Date: November 03, 2010
*/

/*	CSSParserPrefsView implementation
	See CSSParserPrefsView.hpp for more information
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
#include "commondefs.h"
#include "PrefsDefs.h"
#include "plugclass.h"


// CSSParser headers
#include "CSSParserPrefsView.hpp"

CSSParserPrefsView :: CSSParserPrefsView(BRect aFrame,
										 const char* aName,
										 BHandler * aPlugin)
				   : BasePrefsView(aFrame,
									aName) {

	mPlugin = aPlugin;

	/* CSS file selection */
	mPopUpMenu = new BPopUpMenu(
		"No CSS selected or available!",
		true,
		true,
		B_ITEMS_IN_COLUMN);
					
	/* find a CSS file */
	AppSettings->FindString(kPrefsSettingsDirectory, &mCSSDir);
	mCSSDir.Append("/css/");
	printf("CSS dir: %s\n", mCSSDir.String());
			
	BDirectory dir(mCSSDir.String());
	if(dir.InitCheck() != B_OK) {
		printf("CSS directory (%s) not found!\n", mCSSDir.String());
		printf("Setting CSSToUsePath to \"none\"\n");
		AppSettings->AddString(kPrefsActiveCSSPath, kNoCSSFoundString);
	}
	else {
		BString activeCSS;
		AppSettings->FindString(kPrefsActiveCSSPath, &activeCSS);
		
		BEntry entry;
		while(dir.GetNextEntry(&entry, false) != B_ENTRY_NOT_FOUND) {
			BPath path;
			entry.GetPath(&path);
			char name[B_FILE_NAME_LENGTH];
			entry.GetName(name);
				
			BString nstring(name);
			printf("----------------\n");
			printf("found CSS file: %s\n", nstring.String());
							
			/* add the file to the popupmenu */
			BMessage* msg = new BMessage(CSS_CHANGED_PARSER);
			msg->AddString("CSSFileString", path.Path());
			BMenuItem* item = new BMenuItem(name, msg, 0, 0);
			item->SetTarget(mPlugin);
			mPopUpMenu->AddItem(item);
							
			// if the path of the current file equals the one of the settings,
			// mark the item
			if(strcmp(activeCSS.String(), path.Path() ) == 0) {
				printf("CSS from settings found -> SetMarked( true )\n");
				(mPopUpMenu->ItemAt(mPopUpMenu->CountItems() - 1))->SetMarked(true);
			}
		} // while
						
	}
	// end: find a CSS file
		
	BRect rect = mMainBox->Bounds();
	rect.InsetBy(kItemSpacing, kItemSpacing);
	rect.top += kBBoxExtraInset;
					
#ifndef __HAIKU__
	// ResizeToPreferred of a BMenuField is broken on BeOS,
	// so we do something ourselves
	font_height height;
	GetFontHeight(&height);
	// Set the menu height to the font height + a number of pixels
	// around the text.
	float menuHeight = height.ascent + height.descent + height.leading + 6;
	rect.bottom = rect.top + menuHeight;
#endif	

	BMenuField* cssmenufield = new BMenuField(
		rect,
		"CSSFIELD", "Cascading Style Sheet:",
		mPopUpMenu,
		true,
		B_FOLLOW_TOP,
		B_WILL_DRAW);
#ifdef __HAIKU__
	// We can use the normal way to resize the BMenuField to the right size
	// on Haiku
	cssmenufield->ResizeToPreferred();
#endif	
	cssmenufield->SetDivider(be_plain_font->StringWidth("Cascading Style Sheet:") + kItemSpacing);
	mMainBox->AddChild(cssmenufield);

}

void CSSParserPrefsView :: AttachedToWindow() {

	// if we found some CSS files, but still no CSS file is saved in the prefs,
	// or no CSS file is selected: 
	// set the last found CSS file in the prefs. we save it to the prefs,
	// because the user might not reselect a CSS file in the list, which
	// would save the CSS file.
	BMessage msg(CSS_CHANGED_PARSER);
	BMessenger msgr(mPlugin);
	if (mPopUpMenu->CountItems() > 0) {
		if (mPopUpMenu->FindMarked() == NULL) {
			printf("no marked item found\n");
			BMenuItem* item = mPopUpMenu->ItemAt(mPopUpMenu->CountItems() - 1);
			item->SetMarked(true);
			// as we cannot invoke the item here, send the CSS_SELECTED message here
			BString cssstring(mCSSDir.String());
			cssstring.Append(item->Label());
			msg.AddString("CSSFileString", cssstring.String());
			msgr.SendMessage(&msg);
		}
	}
	else {
		msg.AddString("CSSFileString", kNoCSSFoundString);
		msgr.SendMessage(&msg);
	}

}
