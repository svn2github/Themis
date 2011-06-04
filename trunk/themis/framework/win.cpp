/*
Copyright (c) 2000 Z3R0 One. All Rights Reserved. 

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

Original Author & Project Manager: Z3R0 One (z3r0_one@yahoo.com)
Project Start Date: October 18, 2000
*/

// Standard C headers
#include <stdlib.h>
#include <ctype.h>

// BeOS headers
#include <Deskbar.h>
#include <Directory.h>
#include <MenuItem.h>
#include <Screen.h>
#include <TranslationKit.h>
#include <be/storage/FindDirectory.h>
#include <be/storage/Path.h>

// Themis headers
#include "app.h"
#include "protocol_plugin.h"
#include "ThemisIcons.h"
#include "ThemisTab.h"
#include "SiteHandler.h"
#include "SiteEntry.h"
#include "UrlEntry.h"
#include "ThemisUrlView.h"
#include "ThemisNavView.h"
#include "ThemisStatusView.h"
#include "ThemisTabView.h"
#include "ThemisUrlPopUpWindow.h"
#include "../common/commondefs.h"
#include "../common/PrefsDefs.h"
#include "GlobalHistory.h"
#include "TPictureButton.h"
#include "win.h"

extern plugman *PluginManager;
extern BMessage *AppSettings;

Win :: Win(BRect frame,
		   const char *title,
		   window_type type,
		   uint32 flags,
		   uint32 wspace)
    : BWindow(frame,
    		  title,
    		  type,
    		  flags,
    		  wspace),
      MessageSystem("Themis Window") {

	MsgSysRegister(this);
	// size limits
	BScreen Screen;
	SetSizeLimits(300, Screen.Frame().right, 200, Screen.Frame().bottom);

	protocol=0;
	startup=true;
	urlpopupwindow = NULL;
	fNextWindow = NULL;
	fOldFrame = Frame();
	fMaximized = false;
	fQuitConfirmed = false;

	LoadInterfaceGraphics();

	BRect rect;
	rect = Bounds();

	// lets add a menubar
	menubar = new BMenuBar(BRect(0, 0, 0, 0), "MENUBAR");
	AddChild(menubar);

	// menu messages
	BMessage* tabaddmsg = new BMessage(TAB_ADD);
	tabaddmsg->AddBool("force_non_hidden", true);

	// filemenu
	filemenu = new BMenu("File");
	menubar->AddItem(filemenu);
	BMenuItem * newwinitem =
		new BMenuItem("New Window", new BMessage(WINDOW_NEW), 'N');
	filemenu->AddItem(newwinitem);
	BMenuItem * newtabitem =
		new BMenuItem("New Tab", tabaddmsg, 'T');
	filemenu->AddItem(newtabitem);
	BMenuItem * aboutitem =
		new BMenuItem("About Themis", new BMessage(B_ABOUT_REQUESTED), 'A', B_SHIFT_KEY);
	filemenu->AddItem(aboutitem);
	BMenuItem * quitwentry =
		new BMenuItem("Close Window", new BMessage(B_QUIT_REQUESTED), 'W');
	filemenu->AddItem(quitwentry);
	BMenuItem * quitentry =
		new BMenuItem("Quit Themis", new BMessage(B_QUIT_REQUESTED), 'Q');
	filemenu->AddItem(quitentry);
	quitentry->SetTarget(be_app);

	// optionsmenu
	optionsmenu = new BMenu("Options");
	menubar->AddItem(optionsmenu);
	BMenuItem * prefsentry =
		new BMenuItem("Preferences", new BMessage(PREFSWIN_SHOW), 'P');
	prefsentry->SetTarget(be_app);
	optionsmenu->AddItem(prefsentry);

	// now we need the navigation view
	navview = new ThemisNavView(
		BRect(rect.left,
			  menubar->Bounds().Height() + 1,
			  rect.right,
			  menubar->Bounds().Height() + 31));
	AddChild(navview);
	
	// and finally the statusview at the bottom
	statusview = new ThemisStatusView(
		BRect(rect.left,
			  rect.bottom - 13,	// heigth of the lower-right doc-view corner
			  rect.right,		// - 13 taken out because of dano decors
			  rect.bottom));
	AddChild(statusview);
	
	// the tabview-system comes last as we need the height of the statusview
	CreateTabView();
	
	// make the urlview the focusview for fast pasting :D
	if (CurrentFocus() != NULL)
		CurrentFocus()->MakeFocus(false);
	navview->urlview->TextView()->MakeFocus(true);

}

Win :: ~Win() {

	for (int32 i = 0; i < 10; i++)
		delete bitmaps[i];
	
	MsgSysUnregister(this);

}

void Win :: SendUrlOpenMessage(const char * aUrl, bool aAddToHistory) {
	
	BMessage * message = new BMessage(URL_OPEN);
	message->AddString("url_to_open", aUrl);
	if (!aAddToHistory) {
		message->AddBool("no_history_add", true);
	}
	BMessenger msgr(this);
	msgr.SendMessage(message);
	delete message;

}

bool Win :: QuitRequested() {

 	if (fQuitConfirmed == false) {
		BMessage * closemsg = new BMessage(WINDOW_CLOSE);
		closemsg->AddPointer("win_to_close", this);
		be_app_messenger.SendMessage(closemsg);
		delete closemsg;
		return false;
	}
	else {
		CloseUrlPopUpWindow();
		AppSettings->ReplaceRect(kPrefsMainWindowRect, Frame());
		return true;
	}

}

void Win :: MessageReceived(BMessage * msg) {

	switch (msg->what) {
		case B_ABOUT_REQUESTED: {
			be_app_messenger.SendMessage(B_ABOUT_REQUESTED);
			break;
		}
		case B_QUIT_REQUESTED: {
			PostMessage(B_QUIT_REQUESTED);
			break;
		}
		case PlugInLoaded: {
			PlugClass * pobj = NULL;
			msg->FindPointer("plugin", (void**)&pobj);
			if (pobj != NULL) {
				pobj->Window = this;
				pobj->AddMenuItems(optionsmenu);
			}
			break;
		}
		case B_MOUSE_WHEEL_CHANGED: {
			// need to do the scrolling of the urlpopup here
			// as the urlpopupwindow never gets focus and thus
			// never get the mouse wheel messages
			if (urlpopupwindow != NULL) {
				if (urlpopupwindow->HasScrollBar()) {
					BMessenger* msgr = new BMessenger(NULL, urlpopupwindow, NULL);
					msgr->SendMessage(msg);
				}
			}
			break;
		}
		case BUTTON_BACK: {
			const char * previous = NULL;
			previous = ((ThemisTab *)tabview->TabAt(tabview->Selection()))->GetHistory()->GetPreviousEntry();
			if (previous != NULL) {
				SendUrlOpenMessage(previous, false);
			}
			break;
		}
		case BUTTON_FORWARD: {
			const char * next = NULL;
			next = ((ThemisTab *)tabview->TabAt(tabview->Selection()))->GetHistory()->GetNextEntry();
			if (next != NULL) {
				SendUrlOpenMessage(next, false);
			}
			else
			{
				tabview->SetNavButtonsByTabHistory();	// needed to disable the fwd button
			}
			break;
		}
		case BUTTON_STOP: {
			break;
		}
		case BUTTON_HOME: {
			BString homepage;
			AppSettings->FindString(kPrefsHomePage, &homepage);
			SendUrlOpenMessage(homepage.String());
			break;
		}
		case CLOSE_OTHER_TABS: {
			// this function is used for the tab-pop-up-menu function
			// "Close other Tabs" and also for the closetabview_button
			Lock();
			
			// if the newtab button is disabled, enable it again
			navview->buttons[4]->SetMode(0);
						
			uint32 current_tab = 0;
			uint32 count = tabview->CountTabs();
			
			if (msg->HasInt32("tabindex"))
				current_tab = msg->FindInt32("tabindex");
			else
				current_tab = tabview->Selection();
			
			while (count > 1) {
				if (count - 1 > current_tab) {
					tabview->RemoveTab(count - 1);
				}
				else {
					current_tab = current_tab - 1;
					tabview->RemoveTab(current_tab);
				}
				tabview->DynamicTabs(false);
				count = tabview->CountTabs();
			}

			// used by the closetabview button
			if (msg->HasBool("close_last_tab"))
				if(msg->FindBool("close_last_tab") == true)
					tabview->SetFakeSingleView();

			Unlock();
			break;
		}
		case CLOSE_URLPOPUP: {
			CloseUrlPopUpWindow();
			break;
		}
		case RE_INIT_TABHISTORY: {
			ReInitTabHistory();
			break;
		}
		case TAB_ADD: {
			// dissallow adding of new tabs, if they wouldnt fit in the
			// window anymore, and disable newtab button
			if (tabview->tab_width <= 30) {
				// calculate if still one tab would fit and not cover
				// the closetabview button partially
				float width = (tabview->CountTabs() + 1) * 25;
				
				if (Bounds().right - width <= 22) {
					// disable the newtab button
					navview->buttons[4]->SetMode(3);
					break;
				}
				// if we won't cover the button, go on...
			}
						
			// if the prefs are not set to sth like "open new tabs hidden"
			// we pass hidden = false to AddNewTab
			// this selects the last tab ( the new one )
			bool hidden = true;
			AppSettings->FindBool("OpenTabsInBackground", &hidden);
			// OPTION+T shortcut for new tab should open tabs non hidden
			bool force_non_hidden = false;
			msg->FindBool("force_non_hidden", &force_non_hidden);
			if (force_non_hidden == true)
				hidden = false;
			AddNewTab(hidden);
			
			if (msg->HasString("url_to_open")) {
				BMessage * urlopenmsg = new BMessage(URL_OPEN);
				urlopenmsg->AddString("url_to_open", msg->FindString("url_to_open"));
				urlopenmsg->AddInt32("tab_to_open_in", tabview->CountTabs() - 1);
				urlopenmsg->AddBool("hidden", hidden);
				
				BMessenger * target = new BMessenger(this);
				target->SendMessage(urlopenmsg);

				delete urlopenmsg;
				delete target;
			}				
			break;
		}
		case BUTTON_RELOAD:
		case URL_OPEN: {
			// close urlpopup if needed
			CloseUrlPopUpWindow();
			// get the url
			BString url;
			if (msg->HasString("url_to_open"))
				url = msg->FindString("url_to_open");
			else
				url = navview->urlview->Text();

			// stop, if there is no url, or about:blank
			if ((url.Length() == 0) || (strcmp(url.String(), kAboutBlankPage) == 0))
				break;
			
			uint32 selection = tabview->Selection();
			/* Get an unique ID from the app for the site entry*/
			int32 siteId = ((App *)be_app)->GetNewID();
			SiteEntry * siteEntry = new SiteEntry(siteId, url.String());
			/* Get an unique ID from the app for the url entry */
			int32 urlId = ((App *)be_app)->GetNewID();
			UrlEntry * urlEntry = new UrlEntry(urlId, url.String());
			siteEntry->addEntry(urlEntry);
			((App *)be_app)->GetSiteHandler()->AddEntry(siteEntry);

			ThemisTab * tab = NULL;
			if (msg->HasInt32("tab_to_open_in")) {
				int32 index = msg->FindInt32("tab_to_open_in");
				tab = (ThemisTab *) tabview->TabAt(index);
			}
			else {
				tab = (ThemisTab *) tabview->TabAt(selection);
			}
			tab->SetSiteID(siteId);
			// add history entry for tab
			if(msg->HasBool("no_history_add") == false)
				tab->GetHistory()->AddEntry(url.String());
			BRect rect = tabview->Bounds();
			siteEntry->SetSize(rect.Width() - 2, rect.Height() - 2);
			
			/* add url to global history
			 * ( You ask, why I am also adding the url on RELOAD, and not only on URL_OPEN?
			 *   Simple. Imagine a browser being open for longer then GlobalHistoryDepthInDays.
			 *   The url would be deleted after this period of days. If the user would then reload
			 *   the page, it would not be saved in the history. If he closes the browser, his url
			 *   would be lost. Ok, this is somewhat unlikely, but could happen. ) 
			*/
			((App *)be_app)->GetGlobalHistory()->AddEntry(url.String());
			
			// I don't want to destroy anything working right now. So let's just
			// get something new in.
			
			BMessage * load = new BMessage(SH_RETRIEVE_START);
			load->AddInt32("command", COMMAND_RETRIEVE);
			load->AddInt32("site_id", siteId);
			load->AddInt32("url_id", urlId);
			load->AddString("url", url.String());
			if (msg->what != URL_OPEN)
				load->AddBool("reload", true);
			
			Broadcast(MS_TARGET_PROTOCOL, load);
			delete load;

			/*
			 * Trigger this after the UrlHandler knows about the URL.
			 * Otherwise we wouldn't see any URL in UrlView during load.
			 * Sad, but true.
			 */
						
			if (msg->FindBool("hidden") == true)
				tabview->DrawTabs();
			else
				tabview->Select(selection);

			break;
		}
		case URL_TYPED: {
			bool show_all = false;
			msg->FindBool("show_all", &show_all);
			
			bool prefs_show_type_ahead = false;
			AppSettings->FindBool(kPrefsShowTypeAheadWindow, &prefs_show_type_ahead);
									
			if ((prefs_show_type_ahead == true) || (show_all == true)) {
				if (show_all == true)
					UrlTypedHandler(true);
				else
					UrlTypedHandler(false);
			}

			break;
		}
		case WINDOW_NEW: {
			// resend the message to the app
			be_app_messenger.SendMessage(msg);
			break;
		}
		default: {
			BWindow::MessageReceived(msg);
			break;
		}
	}
}

uint32 Win :: BroadcastTarget() {

	return MS_TARGET_WINDOW;

}

void Win :: WindowActivated(bool active) {

	if (startup) {
		startup = false;
		printf("WindowActivated\n");
	}

	if(active == false) {
		CloseUrlPopUpWindow();
	}
	BWindow::WindowActivated(active);

}

void Win :: WorkspacesChanged(uint32 oldws, uint32 newws) {

	//we don't really care what workspace we're running in, however, we need to
	//reset the size limits to match.
	BScreen Screen;
	SetSizeLimits(300, Screen.Frame().right, 200, Screen.Frame().bottom);

}

void Win :: FrameMoved(BPoint origin) {

	BWindow::FrameMoved(origin);
	
	if (urlpopupwindow != NULL) {
		BPoint point(navview->ConvertToScreen(navview->urlview->Frame().LeftBottom()));
		urlpopupwindow->MoveTo(point);
	}

}

void Win :: FrameResized(float width, float height) {

	// calculate new tab width
	tabview->DynamicTabs(false);

	// enable or disable the newtab button
	if ((tabview->CountTabs() * tabview->tab_width) <= (Bounds().right - 22))
		navview->buttons[4]->SetMode(0);
	else
		navview->buttons[4]->SetMode(3);

	// resize urlpopupwindow
	if (urlpopupwindow != NULL) {
		urlpopupwindow->Lock();
		urlpopupwindow->ResizeToPrefered();
		urlpopupwindow->Unlock();
	}

}

void Win :: Zoom(BPoint origin, float width, float height) {

	bool IM = false;
	AppSettings->FindBool(kPrefsIntelligentZoom, &IM);
	
	if (IM == true) {
		if (fMaximized == false) {
			fOldFrame = Frame();
			BScreen screen;
			BDeskbar deskbar;
			BRect dbframe = deskbar.Frame();
		
			switch (deskbar.Location()) {
				case B_DESKBAR_TOP: {
					origin.y += dbframe.Height() + 2;
					break;
				}
				case B_DESKBAR_BOTTOM: {
					height -= dbframe.Height() + 2;
					break;
				}
				case B_DESKBAR_LEFT_BOTTOM: {
					origin.x += dbframe.Width() + 2;
					width -= dbframe.right + 2;
					break;
				}
				case B_DESKBAR_RIGHT_BOTTOM: {
					width = dbframe.left - origin.x - 7;
					break;
				}
				case B_DESKBAR_LEFT_TOP: {
					origin.x += dbframe.Width() + 2;
					width -= dbframe.right + 2;
					break;
				}
				case B_DESKBAR_RIGHT_TOP: {
					width = dbframe.left - origin.x - 7;
					break;
				}
			}
			fMaximized = true;

			BWindow::Zoom(origin, width, height);
		}
		else {
			origin = fOldFrame.LeftTop();
			width = fOldFrame.right - fOldFrame.left;
			height = fOldFrame.bottom - fOldFrame.top;
			fMaximized = false;
			
			BWindow::Zoom(origin, width, height);
		}
	}
	else
		BWindow::Zoom(origin, width, height);

}

void Win :: AddNewTab(bool hidden) {

	Lock();
	
	// needed already here as DynamicTabs would stop if true
	// but we cannot call SetNormalTabView() here
	tabview->fake_single_view = false;
	tabview->DynamicTabs(true);
	ThemisTab * newtab = new ThemisTab(NULL);
	tabview->AddTab(NULL, newtab);
	
	if (hidden == false)
		tabview->Select(tabview->CountTabs() - 1);
	
	tabview->Draw(tabview->Bounds());
	
	if (tabview->CountTabs() > 1)
		tabview->SetNormalTabView();

	Unlock();

}

ThemisTabView * Win :: GetTabView() const {
	
	return tabview;
	
}

ThemisNavView * Win :: GetNavView() const {

	return navview;

}

ThemisUrlPopUpWindow * Win :: GetUrlPopUpWindow() const {

	return urlpopupwindow;

}

BBitmap * Win :: GetBitmap(unsigned int aIndex = 0) const {

	return bitmaps[aIndex];

}

bool Win :: CloseUrlPopUpWindow() {

	bool result = false;

	if (urlpopupwindow != NULL) {
		urlpopupwindow->Lock();
		urlpopupwindow->Quit();
		urlpopupwindow = NULL;
		
		result = true;
	}

	return result;	

}

void Win :: SetLoadingInfo(int doc_progress,
							const char* status_text) {

	statusview->SetLoadingInfo(doc_progress, status_text);

}

void Win :: CreateTabView() {

	BRect rect = Bounds();
	tabview = new ThemisTabView(
		BRect(
			rect.left,
			rect.top + menubar->Bounds().Height() + 1 + navview->Bounds().Height() + 1,
			rect.right,
			rect.bottom - statusview->Bounds().Height() - 1),
		"THEMISTABVIEW",
		B_WIDTH_AS_USUAL,
		B_FOLLOW_ALL,
		B_FULL_UPDATE_ON_RESIZE |
			B_WILL_DRAW |
			B_NAVIGABLE_JUMP |
			B_FRAME_EVENTS |
			B_NAVIGABLE);
	AddChild(tabview);
	
	// add the first tab
	BMessenger * target = new BMessenger(this);
	target->SendMessage(TAB_ADD);
	delete target;

}

void Win :: CreateUrlPopUpWindow(BList * aList) {

	if (urlpopupwindow == NULL) {
		BRect frame(navview->urlview->Frame());
		frame = navview->ConvertToScreen(frame);

		BRect wframe(frame);
		wframe.top = frame.bottom;
		wframe.bottom = wframe.top + 30;
		
		urlpopupwindow = new ThemisUrlPopUpWindow(this,	wframe, aList);
		urlpopupwindow->Run();
		urlpopupwindow->Show();
		
		// make the urlview the focusview otherwise
		// the urlpopup wouldnt be navigable by keyb
		if (CurrentFocus() != NULL)
			CurrentFocus()->MakeFocus(false);
	 	navview->urlview->textview->MakeFocus(true);
	}
	else {
		urlpopupwindow->Lock();
		urlpopupwindow->ListToDisplay(aList);
		urlpopupwindow->Unlock();
	}

}

ThemisTab * Win :: FindTabFor(int32 id,	int32* tabindex = NULL) {

	int32 count = tabview->CountTabs();
	for (int32 i = 0; i < count; i++) {
		ThemisTab * tab = (ThemisTab *)tabview->TabAt(i);
		if (tab) {
			if (tab->GetSiteID() == id) {
				if (tabindex)
					*tabindex = i;
				return tab;
			}
		}
	}

	return NULL;

}

void Win :: LoadInterfaceGraphics() {
	
	bool dir_exists = true;
	BString dirstr;
	BPath settingsPath;
	status_t status = find_directory(B_USER_SETTINGS_DIRECTORY, &settingsPath, false);
	if (status == B_OK) {
		settingsPath.Append("Themis/interface/");
		dirstr = settingsPath.Path();
		BDirectory dir(dirstr.String());
		if (dir.InitCheck() != B_OK) {
			dir_exists = false;
		}
	}
	else {
		dir_exists = false;
	}
	
	// load the icons either from bitmap-file or if not present, from internal
	// icon hexdumps located in "ThemisIcons.h"
	
	// initialize all bitmaps with NULL
	for (int i=0; i < 10; i++)
		bitmaps[i] = NULL;

	// the array with file-names
	const char * names[9];
	// 4-pic bitmap
	names[0] = "button_back.png";
	names[1] = "button_forward.png";
	names[2] = "button_stop.png";
	names[3] = "button_home.png";
	names[4] = "button_newtab.png";
	// 3-pic bitmap
	names[5] = "button_reload.png";
	names[6] = "button_go.png";
	names[7] = "button_closetabview.png";
	// 1-pic bitmap
	names[8] = "icon_document.png";
	names[9] = "icon_document_empty.png";
	
	// the array with pointers to the hexdumps
	const unsigned char * hexp[10];
	// 4-pic bitmap
	hexp[0] = button_back_hex;
	hexp[1] = button_forward_hex;
	hexp[2] = button_stop_hex;
	hexp[3] = button_home_hex;
	hexp[4] = button_newtab_hex;
	// 3-pic bitmap
	hexp[5] = button_reload_hex;
	hexp[6] = button_go_hex;
	hexp[7] = button_closetabview_hex;
	// 1-pic bitmap
	hexp[8] = icon_document_hex;
	hexp[9] = icon_document_empty_hex;
	
	// a tempstring :D
	// ( i had to move this below the 2 char arrays .. otherwise i got a
	// seqm viol. and tstr was initialized with hexp[9] ..
	// dunno if i did some very lousy coding mistakes or if sth is playing me a trick )
	BString tstr(dirstr.String());
		
	for (int i = 0; i < 10; i++) {
		if (dir_exists) {
			// load from file
			tstr.SetTo(dirstr.String());
			bitmaps[i] = BTranslationUtils::GetBitmapFile((tstr += names[i]).String());
		}
		if (bitmaps[i] == NULL) {
			// load from hexdump
			if (i <= 4) {
				bitmaps[i] = new BBitmap(BRect(0, 0, 63, 15), B_RGB32);
				memcpy(bitmaps[i]->Bits(), hexp[i], 4096);
			}
			if (i >= 5 && i <= 7) {
				bitmaps[i] = new BBitmap(BRect(0, 0, 47, 15), B_RGB32);
				memcpy(bitmaps[i]->Bits(), hexp[i], 3072);
			}
			if (i == 8 || i == 9) {
				bitmaps[i] = new BBitmap(BRect(0, 0, 15, 15), B_RGB32);
				memcpy(bitmaps[i]->Bits(), hexp[i], 1024);
			}
		}
	}

}

Win * Win :: NextWindow() {

	if (fNextWindow != NULL) {
		return fNextWindow;
	}
	else {
		return NULL;
	}

}

void Win :: ReInitTabHistory() {

	/* update the tabs history depth */
	int8 depth;
	AppSettings->FindInt8(kPrefsTabHistoryDepth, &depth);
	for (int32 i = 0; i < tabview->CountTabs(); i++)
		((ThemisTab *)tabview->TabAt(i))->GetHistory()->SetDepth(depth);

	/* and update the nav buttons states */
	tabview->SetNavButtonsByTabHistory();

}

void Win :: SetNextWindow(Win * nextwin) {

	fNextWindow = nextwin;

}

void Win :: SetQuitConfirmed(bool state) {

	fQuitConfirmed = state;

}

void Win :: UrlTypedHandler(bool show_all) {
	
	BString typed_url;					// the typed url
	
	if (show_all == true)
		typed_url.SetTo("");
	else {
		typed_url.SetTo(navview->urlview->Text());
		typed_url.ToLower();
	}

	// create the matching urls list
	BList * list = ((App *)be_app)->GetGlobalHistory()->GetFilteredList(typed_url);

	// add the urlpopupwindow if needed
	if (list->CountItems() > 0) {
		CreateUrlPopUpWindow(list);
	}
	else {
		CloseUrlPopUpWindow();
	}

	delete list;

}

status_t Win :: ReceiveBroadcast(BMessage * message) {

	uint32 command=0;
	message->FindInt32("command", (int32 *)&command);

	int16 win_uid = -1;
	if (message->HasInt16("window_uid")) {
		message->FindInt16("window_uid", &win_uid);
	}

	switch (command) {
		case COMMAND_INFO: {
			switch (message->what) {
				case SH_RENDER_FINISHED: {
					int32 site_id = 0;
					message->FindInt32("site_id", &site_id);
					if (site_id != 0) {
						int32 tabindex;
						ThemisTab * tab = FindTabFor(site_id, &tabindex);
						if (tab != NULL) {
							SiteHandler * sh = ((App *)be_app)->GetSiteHandler();
							if (sh) {
								SiteEntry * site = sh->GetEntry(site_id);
								int32 viewId = 0;
								message->FindInt32("view_id", &viewId);
								if (viewId != 0) {
									BaseEntry * entry = site->getEntry(viewId);
									if (entry != NULL) {
										BView * renderview = (BView *) entry->getPointer("render_view");
										if (renderview != NULL) {
											/* Attach the renderview to the correct tab. */
											Lock();
											/*
											 * We don't need to resize the renderview here, as this is done in
											 * ThemisTabView::Select().
											 */
											tabview->TabAt(tabindex)->SetView(renderview);
											
											if (tabview->Selection() == tabindex) {
												tabview->Select(tabindex);
												
												if (CurrentFocus() != NULL)
													CurrentFocus()->MakeFocus(false);
												tabview->TabAt(tabindex)->View()->MakeFocus(true);
											}
											Unlock();
										}
									}
								}
							}
						}
					}

					break;
				}
				case SH_LOADING_PROGRESS: {
					int32 site_id = 0;
					message->FindInt32("site_id", &site_id);

					SiteHandler * sh = ((App *)be_app)->GetSiteHandler();
					if (sh) {
						SiteEntry * site_entry = sh->GetEntry(site_id);
						UrlEntry * url_entry = NULL;

						if (site_entry != NULL) {
							int32 url_id = 0;
							message->FindInt32("url_id", &url_id);
							url_entry = (UrlEntry *) site_entry->getEntry(url_id);

							if (url_entry != NULL) {
								int64 content_length = 0;
								int64 bytes_received = 0;
								bool request_done = false;
								bool secure = false;
			
								message->FindInt64("bytes-received", &bytes_received);
								message->FindBool("request_done", &request_done);
								message->FindBool("secure", &secure );
			
								if (message->HasInt64("content-length"))
									message->FindInt64("content-length", &content_length);
			
								site_entry->SetSecureConnection(secure);
								int8 loadprogress = -1;
								if (request_done == false) {
									if (content_length != 0) {
										loadprogress = (int8)(((float)bytes_received / (float)content_length) * 100);
									}
									else { // chunked transfer mode
										loadprogress = 50;
									}
								}
								else {
									loadprogress = 100;
										
									/*
									 * As long, as we have no FavIcon loading ready, I do set
									 * the non-empty document icon here.
									 */
									BBitmap * bmp =
										new BBitmap(BRect(0, 0, 15,15), B_RGB32);
									memcpy(bmp->Bits(), icon_document_hex, 1024);
									site_entry->SetFavIcon(bmp);
									delete bmp;
								}
								url_entry->SetLoadingProgress(loadprogress);

								int32 tabindex;
								ThemisTab * tab = FindTabFor(site_id, &tabindex);
								if (tab && tabindex == tabview->Selection()) {
									Lock();
									/* Update the StatusView */
									statusview->SetLoadingInfo(
										sh->GetLoadingProgressFor(site_id),
										sh->GetStatusTextFor(site_id));
									Unlock();
								}
							}
						}
					}
					break;
				}
			} // switch( message->what )
			break;
		} // case COMMAND_INFO :
	} // switch( command )

	return B_OK;

}
