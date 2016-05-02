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

/*	HTMLParserPrefsView implementation
	See HTMLParserPrefsView.hpp for more information
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


// HTMLParser headers
#include "HTMLParserPrefsView.hpp"

HTMLParserPrefsView :: HTMLParserPrefsView(BRect aFrame,
										   const char* aName,
										   BHandler * aPlugin)
					: BasePrefsView(aFrame,
									aName) {

	mPlugin = aPlugin;

	/* DTD selection */
	mPopUpMenu = new BPopUpMenu(
		"No DTD selected or available!",
		true,
		true,
		B_ITEMS_IN_COLUMN);
					
	/* find a DTD */
	AppSettings->FindString(kPrefsSettingsDirectory, &mDTDDir);
	mDTDDir.Append("/dtd/");
	printf("DTD dir: %s\n", mDTDDir.String());
			
	BDirectory dir(mDTDDir.String());
	if(dir.InitCheck() != B_OK) {
		printf("DTD directory (%s) not found!\n", mDTDDir.String());
		printf("Setting DTDToUsePath to \"none\"\n");
		AppSettings->AddString(kPrefsActiveDTDPath, kNoDTDFoundString);
	}
	else {
		BString activeDTD;
		AppSettings->FindString(kPrefsActiveDTDPath, &activeDTD);
		
		BEntry entry;
		while(dir.GetNextEntry(&entry, false) != B_ENTRY_NOT_FOUND) {
			BPath path;
			entry.GetPath(&path);
			char name[B_FILE_NAME_LENGTH];
			entry.GetName(name);
				
			BString nstring(name);
			printf("----------------\n");
			printf("found file: %s\n", nstring.String());
			if(nstring.IFindFirst("DTD", nstring.Length() - 3) != B_ERROR) {
				printf("found DTD file: %s\n", nstring.String());
								
				/* add the file to the popupmenu */
				BMessage* msg = new BMessage(DTD_CHANGED_PARSER);
				msg->AddString("DTDFileString", path.Path());
				BMenuItem* item = new BMenuItem(name, msg, 0, 0);
				item->SetTarget(mPlugin);
				mPopUpMenu->AddItem(item);
								
				// if the path of the current file equals the one of the settings,
				// mark the item
				if(strcmp(activeDTD.String(), path.Path()) == 0) {
					printf("DTD from settings found -> SetMarked( true )\n");
					(mPopUpMenu->ItemAt(mPopUpMenu->CountItems() - 1))->SetMarked(true);
				}
			}
		} // while
						
	}
	// end: find a DTD
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
					
	BMenuField* dtdmenufield = new BMenuField(
		rect,
		"DTDFIELD", "Document Type Definition:",
		mPopUpMenu,
		true,
		B_FOLLOW_TOP,
		B_WILL_DRAW);

#ifdef __HAIKU__
	// We can use the normal way to resize the BMenuField to the right size
	// on Haiku
	dtdmenufield->ResizeToPreferred();
#endif	
	dtdmenufield->SetDivider(be_plain_font->StringWidth("Document Type Definition:") + kItemSpacing);
	mMainBox->AddChild(dtdmenufield);

}

void HTMLParserPrefsView :: AttachedToWindow() {

	// if we found some DTDs, but still no DTD is saved in the prefs,
	// or no DTD is selected: 
	// set the last found DTD in the prefs. we save it to the prefs,
	// because the user might not reselect a DTD in the list, which
	// would save the DTD.
	BMessage msg(DTD_CHANGED_PARSER);
	BMessenger msgr(mPlugin);
	if(mPopUpMenu->CountItems() > 0) {
		if(mPopUpMenu->FindMarked() == NULL) {
			printf("no marked item found\n");
			BMenuItem* item = mPopUpMenu->ItemAt(mPopUpMenu->CountItems() - 1);
			item->SetMarked(true);
			// as we cannot invoke the item here, send the DTD_SELECTED message here
			BString dtdstring(mDTDDir.String());
			dtdstring.Append(item->Label());
			msg.AddString("DTDFileString", dtdstring.String());
			msgr.SendMessage(&msg);
		}
	}
	else {
		msg.AddString("DTDFileString", kNoDTDFoundString);
		msgr.SendMessage(&msg);
	}
}
