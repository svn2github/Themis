/*
 * PrefsWin.cpp
 *
 * author: Michael Weirauch
 */

// Standard C headers
#include <stdio.h>
#include <stdlib.h>

// BeOS headers
#include <app/Application.h>
#include <interface/CheckBox.h>
#include <interface/TextControl.h>

// Themis headers
#include "app.h"
#include "commondefs.h"
#include "plugclass.h"
#include "GlobalHistory.h"
#include "PrefsDefs.h"
#include "PrefsIcons.h"
#include "PrefsViews.h"
#include "PrefsWin.h"

/* the struct which defines the order of the prefs */
static struct prefs_item_info kPrefsItems [] =
{
	{ "Window", kPrefsIconGeneral, PREFSVIEW_WINDOW },
	{ "Network", kPrefsIconNetwork, PREFSVIEW_NETWORK },
	{ "Privacy", kPrefsIconPrivacy, PREFSVIEW_PRIVACY },
	{ "HTML Parser", kPrefsIconParser, PREFSVIEW_HTMLPARSER },
	{ "CSS Parser", kPrefsIconParser, PREFSVIEW_CSSPARSER },
	{ "Renderer", kPrefsIconRenderer, PREFSVIEW_RENDERER },
	
	{ NULL, NULL }
};


PrefsWin :: PrefsWin(BRect aFrame, BMessage * aPluginList)
		 : BWindow(aFrame,
				   "Preferences",
				   B_TITLED_WINDOW_LOOK,
				   B_NORMAL_WINDOW_FEEL,
				   B_NOT_RESIZABLE | B_ASYNCHRONOUS_CONTROLS | B_NOT_MINIMIZABLE | B_NOT_ZOOMABLE),
		   fCurrentPrefsView(NULL),
		   fLastSelection(-1),
		   fViewFrame(0, 0, 0, 0) {	

	/* set up the background view */
	fBackgroundView = new BView(
		Bounds(),
		"BackgroundView",
		B_FOLLOW_ALL,
		0);
	AddChild(fBackgroundView);
	fBackgroundView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	/* add the listview */
	BRect rect = Bounds();
	rect.InsetBy(kItemSpacing, kItemSpacing);
	rect.right = rect.left + 100;
	
	fPrefsListView = new PrefsListView(rect, aPluginList);
	fBackgroundView->AddChild(fPrefsListView);
	
	/* calculate the view frame */
	rect = Bounds();
	rect.InsetBy(kItemSpacing, kItemSpacing);
	rect.left = fPrefsListView->Frame().right + kItemSpacing;
	fViewFrame = rect;

	CreatePrefViews(aPluginList);

	Show();
}

PrefsWin :: ~PrefsWin() {
	
	if (fCurrentPrefsView) {
		fCurrentPrefsView->RemoveSelf();
		fCurrentPrefsView = NULL;
	}

	printf("Deleting preferences views\n");
	for (unsigned int i = 0; i < mPrefViews.size(); i++) {
		delete mPrefViews[i];
	}
	
}

bool PrefsWin :: GetCheckBoxValue(BMessage * aMessage) {

	BCheckBox * checkBox;
	aMessage->FindPointer("source", (void**)&checkBox);
	bool result = false;
	if(checkBox->Value() == 1)
		result = true;

	return result;

}

void PrefsWin :: CreatePrefViews(BMessage * aPluginList) {

	printf("Creating preferences views\n");
	
	mPrefViews.push_back(
		new WindowPrefsView(fViewFrame,
							kPrefsItems[0].name));
	mPrefViews.push_back(
		new PrivacyPrefsView(fViewFrame,
							 kPrefsItems[2].name));

	int32 countFound;
	type_code typeFound;
	status_t status = aPluginList->GetInfo(
		"plug_name",
		&typeFound,
		&countFound);
	if (status == B_OK) {
		PlugClass * plugin;
		BView * view = NULL;
		for (int32 i = 0; i < countFound; i++) {
			aPluginList->FindPointer("plug_object", i, (void **)&plugin);
			view = plugin->SettingsView(fViewFrame);
			if (view != NULL) {
				mPrefViews.push_back(view);
				  
			}
		}
	}

}

void PrefsWin :: MessageReceived(BMessage* msg) {

	switch(msg->what) {
		case LIST_SELECTION_CHANGED: {
			int32 selection = fPrefsListView->CurrentSelection();
			/*
			 * if the user presses in the empty area, we ignore this,
			 * but the BListView does take away the focus from the curent
			 * selected item. So reselect it again. :/
			 */
			if(selection < 0) {
				fPrefsListView->Select(fLastSelection);
			}
			else {
				if(selection != fLastSelection) {
					fLastSelection = selection;
				
					if (fCurrentPrefsView) {
						fCurrentPrefsView->RemoveSelf();
						fCurrentPrefsView = NULL;
					}
					
					if ((unsigned int)selection < mPrefViews.size()) {
						fCurrentPrefsView = mPrefViews[selection];
						fBackgroundView->AddChild(fCurrentPrefsView);
					}
				}
			}
			
			break;
		}
		case HOMEPAGE_CHANGED: {
			printf("PREFS: HOMEPAGE_CHANGED\n");
			
			BTextControl * ctrl;
			msg->FindPointer("source", (void**)&ctrl);
			if(ctrl) {
				AppSettings->ReplaceString(kPrefsHomePage, ctrl->Text());
			}
			
			SaveAppSettings();

			break;
		}
		case SET_BLANK_PAGE: {
			printf("PREFS: SET_BLANK_PAGE\n");
						
			BTextControl* ctrl;
			msg->FindPointer("homepagecontrol", (void**)&ctrl);
			if(ctrl) {
				ctrl->SetText(kAboutBlankPage);
			}
			/*
			 * We don't need to save the setting here, as HOMEPAGE_CHANGED is
			 * triggered by SetText().
			 */
			
			break;
		}
		case IZ_CHECKBOX: {
			printf("PREFS: IZ_CHECKBOX\n");
			
			bool value = GetCheckBoxValue(msg);
			AppSettings->ReplaceBool(kPrefsIntelligentZoom, value);
			
			SaveAppSettings();
			
			break;
		}
		case SHOWTYPEAHEAD_CHECKBOX :
		{
			printf("PREFS: SHOWTYPEAHEAD_CHECKBOX\n");
			
			bool value = GetCheckBoxValue(msg);
			AppSettings->ReplaceBool(kPrefsShowTypeAheadWindow, value);
			
			SaveAppSettings();
			
			break;
		}		
		case SHOWTABS_CHECKBOX: {
			printf("PREFS: SHOWTABS_CHECKBOX\n");
			
			bool value = GetCheckBoxValue(msg);
			AppSettings->ReplaceBool(kPrefsShowTabsAtStartup, value);
			
			SaveAppSettings();
			
			break;
		}
		case TABSBACKGROUND_CHECKBOX: {
			printf("PREFS: TABSBACKGROUND_CHECKBOX\n");
			
			bool value = GetCheckBoxValue(msg);
			AppSettings->ReplaceBool(kPrefsOpenTabsInBackground, value);
			
			SaveAppSettings();
			
			break;
		}
		case TABSBLANK_CHECKBOX: {
			printf("PREFS: TABSBACKGROUND_CHECKBOX\n");
			
			bool value = GetCheckBoxValue(msg);
			AppSettings->ReplaceBool(kPrefsOpenBlankTargetInTab, value);
			
			SaveAppSettings();
			
			break;
		}
		case NEW_WINDOW_START_PAGE: {
			printf("PREFS: NEW_WINDOW_START_PAGE\n");
			
			int8 value;
			msg->FindInt8("newpagemode", &value);
			AppSettings->ReplaceInt8(kPrefsNewWindowStartPage, value);
			
			SaveAppSettings();
			
			break;
		}
		case HISTORY_CHANGED: {
			printf("PREFS: HISTORY_CHANGED\n");
			
			int8 setdepth = 0;
			
			BTextControl* ctrl;
			msg->FindPointer("source", (void**)&ctrl);
			BString string(ctrl->Text());
			
			if(string.Length() == 0)
				break;
			
			int32 newdepth = atoi(string.String());
			if(newdepth > 127 || newdepth == 0) {
				setdepth = 10;
				ctrl->SetText("10");
				ctrl->TextView()->Select(2, 2);
			}
			else
				setdepth = (int8)newdepth;
			
			int8 which = 0;
			msg->FindInt8("which", &which);
			
			switch(which) {
				case 0: {
					AppSettings->ReplaceInt8(kPrefsGlobalHistoryDepthInDays, setdepth);
					break;
				}
				case 1: {
					AppSettings->ReplaceInt8(kPrefsGlobalHistoryFreeURLCount, setdepth);
					break;
				}
				case 2: {
					AppSettings->ReplaceInt8(kPrefsTabHistoryDepth, setdepth);
					break;
				}
			}
			
			SaveAppSettings();
			
			if(which < 2) {
				/* tell GlobalHistory abouts its new depth */
				int8 depth;
				AppSettings->FindInt8(kPrefsGlobalHistoryDepthInDays, &depth);
				((App*)be_app)->GetGlobalHistory()->SetDepth(depth);
			}
			else {
				/* tell all windows to update their tab history depths */
				/* use the app to do this*/
				be_app_messenger.SendMessage(RE_INIT_TABHISTORY);
			}
			
			break;
		}
		default :
			BWindow::MessageReceived(msg);
	}
}


void PrefsWin :: SaveAppSettings() {

	be_app_messenger.SendMessage(SAVE_APP_SETTINGS);
}


bool PrefsWin :: QuitRequested() {

	BPoint point(Frame().LeftTop());
	AppSettings->ReplacePoint(kPrefsPrefWindowPoint, point);
	
	AppSettings->ReplaceInt32(kPrefsLastSelectedItem, fPrefsListView->CurrentSelection());
	
	/* Settings get saved once again when handling the following message. */
	be_app_messenger.SendMessage(PREFSWIN_CLOSE);
	return true;
}


/*
 * PrefsListView
 */


PrefsListView :: PrefsListView(BRect aFrame, BMessage * aPluginList)
			  : BView(aFrame,
					  "PrefsListView",
					  0,
					  0) {

	mPluginList = aPluginList;

}

PrefsListView :: ~PrefsListView() {
	
	int32 items = fListView->CountItems();
	for (int32 i = 0; i < items; i++) {
		delete fListView->RemoveItem((int32) 0);
	}
	fListView->MakeEmpty();

	delete mPluginList;
	
}

void PrefsListView :: AttachedToWindow() {

	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	/* add the surrounding box */
	BRect rect = Bounds();
	fBox = new BBox(rect, "Box");
	AddChild(fBox);
	
	/* add the listview */
	rect.InsetBy( 2, 2 );
	fListView = new BListView(
		rect,
		"ListView",
		B_SINGLE_SELECTION_LIST,
		B_FOLLOW_ALL,
		B_WILL_DRAW | B_NAVIGABLE);
	fBox->AddChild(fListView);
	
	/* I want a font size of 12.0 */
	fListView->SetFontSize( 12.0 );

	/* add the list items */
	fListView->AddItem(
		new PrefsListItem(
			"Window", 
			kPrefsIconGeneral));
	fListView->AddItem(
		new PrefsListItem(
			"Privacy", 
			kPrefsIconPrivacy));

	int32 countFound;
	type_code typeFound;
	status_t status = mPluginList->GetInfo(
		"plug_name",
		&typeFound,
		&countFound);
	if (status == B_OK) {
		int32 type;
		const char * name;
		PlugClass * plugin;
		for (int32 i = 0; i < countFound; i++) {
			mPluginList->FindPointer("plug_object", i, (void **)&plugin);
			type = plugin->Type();
			name = plugin->SettingsViewLabel();
			if (name != NULL) {
				if (type != TARGET_PARSER) {
					fListView->AddItem(
						new PrefsListItem(
							name,
							kPrefsIconParser));
					
				}
				else {
					fListView->AddItem(
						new PrefsListItem(
							name,
							kPrefsIconGeneral));
				}
			}
		}
	}
	
	fListView->SetSelectionMessage(new BMessage(LIST_SELECTION_CHANGED));
	fListView->MakeFocus(true);
	
	/* find last selected prefs item */
	int32 last_item = 0;
	AppSettings->FindInt32(kPrefsLastSelectedItem, &last_item);
	if(last_item > fListView->CountItems() - 1) {
		fListView->Select(0);
		AppSettings->ReplaceInt32(kPrefsLastSelectedItem, 0);
	}
	else
		fListView->Select(last_item);
}


int32 PrefsListView :: CurrentSelection() {

	return fListView->CurrentSelection();
}


void PrefsListView :: Select(int32 which) {

	fListView->Select(which);
}


/*
 * PrefsListItem
 */

PrefsListItem :: PrefsListItem(const char* name,
							   const uint8* bitmapdata)
			  : BListItem(),
				fName(name) {

	fBitmap = new BBitmap(
		BRect(
			0,
			0,
			kPrefsIconSize - 1,
			kPrefsIconSize - 1),
		B_RGB32);
	memcpy(
		fBitmap->Bits(),
		bitmapdata,
		kPrefsIconSize * kPrefsIconSize * 4);
}


PrefsListItem::~PrefsListItem() {

	delete fBitmap;
}


void PrefsListItem :: DrawItem(BView* owner,
							   BRect itemRect,
							   bool drawEverything = 0)
{
	/* remember colors */
	rgb_color hi = owner->HighColor();
	rgb_color lo = owner->LowColor();
	
	/* draw outer rect */
	BRect rect = itemRect;
	rect.InsetBy(3, 3);
	owner->SetLowColor(216, 216, 216, 255);
	owner->StrokeRoundRect(
		rect,
		4.0,
		4.0,
		B_SOLID_LOW );
		
	/* check if we are selected, and draw bounding rect */
	if(IsSelected())
		owner->SetLowColor(224, 224, 224, 255);
	else
		owner->SetLowColor(255, 255, 255, 255);
	
	/* fill area of rect according to selected state */
	rect.InsetBy(1, 1);
	owner->FillRoundRect(
		rect,
		4.0,
		4.0,
		B_SOLID_LOW);
	
	/* end of rect drawing */
	
	/* draw bitmap */
	owner->SetDrawingMode(B_OP_ALPHA);
	owner->DrawBitmap(
		fBitmap,
		BPoint(
			itemRect.Width() / 2 - kPrefsIconSize / 2,
			itemRect.top + 5));
	owner->SetDrawingMode(B_OP_COPY);
	
	/* draw name */
	owner->DrawString(
		fName,
		BPoint(
			itemRect.Width() / 2 - owner->StringWidth(fName) / 2,
			itemRect.top + 5 + kPrefsIconSize + fFontHeight));

	/* reset colors */
	owner->SetHighColor(hi);
	owner->SetLowColor(lo);
}


void PrefsListItem :: Update(BView* owner, const BFont* font) {

	font_height fh;
	font->GetHeight(&fh);
	fFontHeight = fh.leading + fh.ascent + fh.descent;
	
	SetHeight(kPrefsIconSize + 10 + fFontHeight);
	SetWidth(owner->Bounds().Width());
}
