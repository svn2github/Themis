/*
 * PrefsViews.cpp
 *
 * author: Michael Weirauch
 */


#include <interface/Box.h>
#include <interface/Button.h>
#include <interface/CheckBox.h>
#include <interface/MenuField.h>
#include <interface/MenuItem.h>
#include <interface/PopUpMenu.h>
#include <interface/RadioButton.h>
#include <interface/TextControl.h>
#include <interface/Window.h>
#include <storage/Directory.h>
#include <storage/Entry.h>
#include <storage/Path.h>

#include <stdio.h>

#include "commondefs.h"
#include "PrefsDefs.h"
#include "PrefsViews.h"


/*
 * BasePrefsView
 */


BasePrefsView :: BasePrefsView(BRect frame,
							   const char* name)
			  : BView(frame,
					  name,
					  B_FOLLOW_ALL,
					  0) {

	fMainBox = new BBox(
		Bounds(),
		"MainBox",
		B_FOLLOW_ALL);
	fMainBox->SetLabel(Name());
	AddChild(fMainBox);

	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

}

/*
 * WindowPrefsView
 */


WindowPrefsView :: WindowPrefsView(BRect frame,
								 const char* name)
				: BasePrefsView(frame,
								name) {

	/* homepage */
	BString str;
	AppSettings->FindString(kPrefsHomePage, &str);
	
	BRect rect = Bounds();
	rect.InsetBy(kItemSpacing, kItemSpacing);
	rect.top += kBBoxExtraInset;
	
	BTextControl* homepage = new BTextControl(
		rect,
		"HOMEPAGE",
		"Home Page:",
		"",
		NULL);
	homepage->SetModificationMessage(new BMessage(HOMEPAGE_CHANGED));
	homepage->SetDivider(be_plain_font->StringWidth("Home Page:") + kDividerSpacing);
	homepage->SetText(str.String());
	fMainBox->AddChild(homepage);
	
	/* blank button */
	rect.top = homepage->Frame().bottom + kItemSpacing;
	BMessage* blankmsg = new BMessage(SET_BLANK_PAGE);
	blankmsg->AddPointer("homepagecontrol", homepage);
	BButton* blankbtn = new BButton(
		rect,
		"BLANKBUTTON",
		"Use Blank Page",
		blankmsg,
		B_FOLLOW_ALL,
		B_WILL_DRAW);
	blankbtn->ResizeToPreferred();
	blankbtn->MoveTo(
		homepage->Frame().right - blankbtn->Bounds().Width(),
		blankbtn->Frame().top );
	fMainBox->AddChild(blankbtn);

	/* intelligent zoom */
	rect.left = Bounds().left + kItemSpacing;
	rect.top = blankbtn->Frame().bottom + kItemSpacing;
	BCheckBox* IZcbox = new BCheckBox(
		rect,
		"IZCBOX",
		"Intelligent Zoom",
		new BMessage(IZ_CHECKBOX));
	IZcbox->ResizeToPreferred();
	bool IntelligentZoom = false;
	AppSettings->FindBool( kPrefsIntelligentZoom, &IntelligentZoom );
	if(IntelligentZoom == false)
		IZcbox->SetValue(0);
	else
		IZcbox->SetValue(1);
	fMainBox->AddChild(IZcbox);
	
	/* type ahead */
	rect.top = IZcbox->Frame().bottom + kItemSpacing;
	BCheckBox* ShowTypeAheadbox = new BCheckBox(
		rect,
		"SHOWTYPEAHEADBOX",
		"Show Type Ahead Window",
		new BMessage(SHOWTYPEAHEAD_CHECKBOX));
	ShowTypeAheadbox->ResizeToPreferred();
	bool ShowTypeAhead = false;
	AppSettings->FindBool(kPrefsShowTypeAheadWindow, &ShowTypeAhead);
	ShowTypeAhead ? ShowTypeAheadbox->SetValue(1) : ShowTypeAheadbox->SetValue(0);
	fMainBox->AddChild(ShowTypeAheadbox);
	
	/* show tabs */
	rect.top = ShowTypeAheadbox->Frame().bottom + kItemSpacing;
	BCheckBox* ShowTabscbox = new BCheckBox(
		rect,
		"SHOWTABSCBOX",
		"Show Tabs at Startup",
		new BMessage(SHOWTABS_CHECKBOX));
	ShowTabscbox->ResizeToPreferred();
	bool ShowTabs = false;
	AppSettings->FindBool(kPrefsShowTabsAtStartup, &ShowTabs);
	ShowTabs ? ShowTabscbox->SetValue(1) : ShowTabscbox->SetValue(0);
	fMainBox->AddChild(ShowTabscbox);
					
	/* open tabs in background */
	rect.top = ShowTabscbox->Frame().bottom + kItemSpacing;
	BCheckBox* OpenTabsInBackgroundcbox = new BCheckBox(
		rect,
		"OPENTABSINBACKGROUNDCBOX",
		"Open new Tabs in Background",
		new BMessage(TABSBACKGROUND_CHECKBOX));
	OpenTabsInBackgroundcbox->ResizeToPreferred();
	bool OpenInBackground = false;
	AppSettings->FindBool(kPrefsOpenTabsInBackground, &OpenInBackground);
	OpenInBackground ? OpenTabsInBackgroundcbox->SetValue(1) : OpenTabsInBackgroundcbox->SetValue(0);
	fMainBox->AddChild( OpenTabsInBackgroundcbox );
	
	/* open blank targets in tab (instead of new window) */
	rect.top = OpenTabsInBackgroundcbox->Frame().bottom + kItemSpacing;
	BCheckBox* OpenBlankTargetInTab_CBox = new BCheckBox(
		rect,
		"OPENBLANKTARGETINTAB",
		"Open \"_blank\" target in new Tab",
		new BMessage(TABSBLANK_CHECKBOX));
	OpenBlankTargetInTab_CBox->ResizeToPreferred();
	bool OpenBlankInTab = false;
	AppSettings->FindBool(kPrefsOpenBlankTargetInTab, &OpenBlankInTab);
	OpenBlankInTab ? OpenBlankTargetInTab_CBox->SetValue(1) : OpenBlankTargetInTab_CBox->SetValue(0);
	fMainBox->AddChild(OpenBlankTargetInTab_CBox);
	
	/* new windows open with ... */
	rect = Bounds();
	rect.InsetBy(kItemSpacing, 0);
	rect.top = OpenBlankTargetInTab_CBox->Frame().bottom + kItemSpacing;
	rect.bottom = rect.top + 100;
	BBox * newInPageBox = new BBox(rect, "NEWWINPAGEBOX");
	newInPageBox->SetLabel("New Windows open with ...");
	fMainBox->AddChild(newInPageBox);
	
	/* blank page */
	rect = newInPageBox->Bounds();
	rect.InsetBy(kItemSpacing, kItemSpacing);
	rect.top += kBBoxExtraInset;
	BMessage* nwspmsg1 = new BMessage(NEW_WINDOW_START_PAGE);
	nwspmsg1->AddInt8("newpagemode", 0);
	BRadioButton* rbtn1 = new BRadioButton(rect, "RBTN1", "Blank Page", nwspmsg1);
	rbtn1->ResizeToPreferred();
	newInPageBox->AddChild(rbtn1);
	/* homepage */
	rect.top = rbtn1->Frame().bottom + kItemSpacing;
	BMessage* nwspmsg2 = new BMessage(NEW_WINDOW_START_PAGE);
	nwspmsg2->AddInt8("newpagemode", 1);
	BRadioButton* rbtn2 = new BRadioButton(rect, "RBTN2", "Home Page", nwspmsg2);
	rbtn2->ResizeToPreferred();
	newInPageBox->AddChild(rbtn2);
	/* current page */
	rect.top = rbtn2->Frame().bottom + kItemSpacing;
	BMessage* nwspmsg3 = new BMessage(NEW_WINDOW_START_PAGE);
	nwspmsg3->AddInt8("newpagemode", 2);
	BRadioButton* rbtn3 = new BRadioButton(rect, "RBTN3", "Current Page", nwspmsg3);
	rbtn3->ResizeToPreferred();
	newInPageBox->AddChild(rbtn3);
	
	newInPageBox->ResizeTo(
		newInPageBox->Bounds().Width(),
		rect.top + rbtn3->Bounds().Height() + kItemSpacing);

	int8 NewPageMode = 0;
	AppSettings->FindInt8(kPrefsNewWindowStartPage, &NewPageMode);
	/* select the correct radiobutton */
	switch(NewPageMode) {
		case 0: {
			rbtn1->SetValue(B_CONTROL_ON);
			break;
		}
		case 1: {
			rbtn2->SetValue(B_CONTROL_ON);
			break;
		}
		case 2: {
			rbtn3->SetValue(B_CONTROL_ON);
			break;
		}
	}
}

/*
 * NetworkPrefsView
 */


NetworkPrefsView :: NetworkPrefsView(BRect frame,
									 const char* name)
				 : BasePrefsView(frame,
								 name) {
}


/*
 * PrivacyPrefsView
 */


PrivacyPrefsView :: PrivacyPrefsView(BRect frame,
									 const char* name)
				 : BasePrefsView(frame,
								 name) {

	BRect rect = fMainBox->Bounds();
	
	rect.InsetBy(kItemSpacing, kItemSpacing);
	rect.top += kBBoxExtraInset;
	
	/* history box */	
	BBox * historyBox = new BBox(
		rect,
		"historyBox");
	historyBox->SetLabel("History");
	fMainBox->AddChild(historyBox);
	
	/* history depth */
	rect = historyBox->Bounds();
	rect.InsetBy(kItemSpacing, kItemSpacing);
	rect.top += kBBoxExtraInset;
	BTextControl* ghistdepth = new BTextControl(
		rect,
		"GLOBALHISTDEPTH",
		"History Depth [in Days, 1-127]:",
		"",
		NULL);
	BMessage* chmsg1 = new BMessage(HISTORY_CHANGED);
	chmsg1->AddInt8("which", 0);
	ghistdepth->SetModificationMessage(chmsg1);
	char string[5];
	int8 days = 0;
	AppSettings->FindInt8(kPrefsGlobalHistoryDepthInDays, &days);
	sprintf(string, "%d", days);
	ghistdepth->SetText(string);
	ghistdepth->TextView()->AddFilter(new DigitOnlyMessageFilter());
	historyBox->AddChild(ghistdepth);
	
	/* free urls */
	rect.top = ghistdepth->Frame().bottom + kItemSpacing;
	BTextControl* freeurlcount = new BTextControl(
		rect,
		"FREEURLCOUNT",
		"Number of free URLs [1-127]:",
		"",
		NULL);
	BMessage* chmsg2 = new BMessage(HISTORY_CHANGED);
	chmsg2->AddInt8("which", 1);
	freeurlcount->SetModificationMessage(chmsg2);
	int8 count = 0;
	AppSettings->FindInt8(kPrefsGlobalHistoryFreeURLCount, &count);
	sprintf(string, "%d", count);
	freeurlcount->SetText(string);
	freeurlcount->TextView()->AddFilter(new DigitOnlyMessageFilter());
	historyBox->AddChild(freeurlcount);
	
	/* tab history */
	rect.top = freeurlcount->Frame().bottom + kItemSpacing;
	BTextControl* tabHistDepth = new BTextControl(
		rect,
		"TABHISTDEPTH",
		"Tab History Depth [1-127]:",
		"",
		NULL);
	BMessage* chmsg3 = new BMessage(HISTORY_CHANGED);
	chmsg3->AddInt8("which", 2);
	tabHistDepth->SetModificationMessage(chmsg3);
	AppSettings->FindInt8(kPrefsTabHistoryDepth, &count);
	sprintf(string, "%d", count);
	tabHistDepth->SetText(string);
	tabHistDepth->TextView()->AddFilter(new DigitOnlyMessageFilter());
	historyBox->AddChild(tabHistDepth);

	/* set the dividers, and resize the controls to be smaller */
	float maxwidth = be_plain_font->StringWidth("History Depth [in Days, 1-127]:");
	float width2 = be_plain_font->StringWidth("Number of free URLs [1-127]:");
	float width3 = be_plain_font->StringWidth("Tab History Depth [1-127]:");
	if(maxwidth < width2)
		maxwidth = width2;
	if(maxwidth < width3)
		maxwidth = width3;
	maxwidth += kDividerSpacing;
		
	ghistdepth->SetDivider(maxwidth);
	freeurlcount->SetDivider(maxwidth);
	tabHistDepth->SetDivider(maxwidth);
	
	ghistdepth->ResizeTo(
		ghistdepth->Divider() + 50,
		ghistdepth->Bounds().Height());
	freeurlcount->ResizeTo(
		freeurlcount->Divider() + 50,
		freeurlcount->Bounds().Height());
	tabHistDepth->ResizeTo(
		tabHistDepth->Divider() + 50,
		tabHistDepth->Bounds().Height());

	historyBox->ResizeTo(
		historyBox->Bounds().Width(),
		rect.top + tabHistDepth->Bounds().Height() + kItemSpacing);

	/* clear button */
	BButton* clearHistbtn = new BButton(
		rect,
		"CLEARGHBUTTON",
		"Clear History",
		new BMessage(HISTORY_CLEAR),
		B_FOLLOW_RIGHT | B_FOLLOW_TOP,
		B_WILL_DRAW);
	clearHistbtn->ResizeToPreferred();
	historyBox->AddChild(clearHistbtn);
	
	/* move button to right center */
	clearHistbtn->MoveTo(
		historyBox->Bounds().right - (clearHistbtn->Bounds().Width() + kItemSpacing),
		historyBox->Bounds().Height() / 2 - clearHistbtn->Bounds().Height() / 2);
	
	/* cache box */
	rect = historyBox->Frame();
	rect.top = rect.bottom + kItemSpacing;
	rect.bottom = rect.top + 100;
	BBox* CacheBox = new BBox(rect, "CACHEBOX");
	CacheBox->SetLabel("Cache");
	fMainBox->AddChild(CacheBox);

	/* cookie box */
	rect = CacheBox->Frame();
	rect.top = rect.bottom + kItemSpacing;
	rect.bottom = rect.top + 100;
	BBox* CookieBox = new BBox(rect, "COOKIEBOX");
	CookieBox->SetLabel("Cookies");
	fMainBox->AddChild(CookieBox);

}


/*
 * HTMLParserPrefsView
 */


HTMLParserPrefsView :: HTMLParserPrefsView(BRect frame,
										   const char* name)
					: BasePrefsView(frame,
									name) {

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
				BMessage* msg = new BMessage(DTD_SELECTED);
				msg->AddString("DTDFileString", path.Path());
				BMenuItem* item = new BMenuItem(name, msg, 0, 0);
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
	BRect rect = fMainBox->Bounds();
	rect.InsetBy(kItemSpacing, kItemSpacing);
	rect.top += kBBoxExtraInset;
					
	BMenuField* dtdmenufield = new BMenuField(
		rect,
		"DTDFIELD", "Document Type Definition:",
		mPopUpMenu,
		true,
		B_FOLLOW_TOP,
		B_WILL_DRAW);
	dtdmenufield->SetDivider(be_plain_font->StringWidth("Document Type Definition:") + kItemSpacing);
	fMainBox->AddChild(dtdmenufield);

}

void HTMLParserPrefsView :: AttachedToWindow() {

	// if we found some DTDs, but still no DTD is saved in the prefs,
	// or no DTD is selected: 
	// set the last found DTD in the prefs. we save it to the prefs,
	// because the user might not reselect a DTD in the list, which
	// would save the DTD.
	BMessage imsg(DTD_SELECTED);
	BMessenger msgr(NULL, Window()->Looper());
	if(mPopUpMenu->CountItems() > 0) {
		if(mPopUpMenu->FindMarked() == NULL) {
			printf("no marked item found\n");
			BMenuItem* item = mPopUpMenu->ItemAt(mPopUpMenu->CountItems() - 1);
			item->SetMarked(true);
			// as we cannot invoke the item here, send the DTD_SELECTED message here
			BString dtdstring(mDTDDir.String());
			dtdstring.Append(item->Label());
			imsg.AddString("DTDFileString", dtdstring.String());
			msgr.SendMessage(&imsg);
		}
	}
	else {
		imsg.AddString("DTDFileString", kNoDTDFoundString);
		msgr.SendMessage(&imsg);
	}
}

/*
 * CSSParserPrefsView
 */


CSSParserPrefsView :: CSSParserPrefsView(BRect frame,
										 const char* name)
				   : BasePrefsView(frame,
								   name) {

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
			BMessage* msg = new BMessage(CSS_SELECTED);
			msg->AddString("CSSFileString", path.Path());
			BMenuItem* item = new BMenuItem(name, msg, 0, 0);
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
		
	BRect rect = fMainBox->Bounds();
	rect.InsetBy(kItemSpacing, kItemSpacing);
	rect.top += kBBoxExtraInset;
					
	BMenuField* cssmenufield = new BMenuField(
		rect,
		"CSSFIELD", "Cascading Style Sheet:",
		mPopUpMenu,
		true,
		B_FOLLOW_TOP,
		B_WILL_DRAW);
	cssmenufield->SetDivider(be_plain_font->StringWidth("Cascading Style Sheet:") + kItemSpacing);
	fMainBox->AddChild(cssmenufield);
}

void CSSParserPrefsView :: AttachedToWindow() {

		// if we found some CSS files, but still no CSS file is saved in the prefs,
		// or no CSS file is selected: 
		// set the last found CSS file in the prefs. we save it to the prefs,
		// because the user might not reselect a CSS file in the list, which
		// would save the CSS file.
		BMessage imsg(CSS_SELECTED);
		BMessenger msgr(NULL, Window()->Looper());
		if (mPopUpMenu->CountItems() > 0) {
			if (mPopUpMenu->FindMarked() == NULL) {
				printf("no marked item found\n");
				BMenuItem* item = mPopUpMenu->ItemAt(mPopUpMenu->CountItems() - 1);
				item->SetMarked(true);
				// as we cannot invoke the item here, send the CSS_SELECTED message here
				BString cssstring(mCSSDir.String());
				cssstring.Append(item->Label());
				imsg.AddString("CSSFileString", cssstring.String());
				msgr.SendMessage(&imsg);
			}
		}
		else {
			imsg.AddString("CSSFileString", kNoCSSFoundString);
			msgr.SendMessage(&imsg);
		}
}

/*
 * RendererPrefsView
 */


RendererPrefsView :: RendererPrefsView(BRect frame,
									   const char* name)
				  : BasePrefsView(frame,
								  name) {
}

