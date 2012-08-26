/* ThemisUrlPopUpWindow.cpp */

// Standard C headers
#include <stdio.h>

// Themis headers
#include "ThemisUrlView.h"
#include "ThemisNavView.h"
#include "win.h"
#include "../common/commondefs.h"
#include "ThemisUrlPopUpWindow.h"
#include "ThemisUrlPopUpView.h"
#include "UrlItem.hpp"

ThemisUrlPopUpWindow :: ThemisUrlPopUpWindow(BWindow * aParent,
											 BRect aFrame,
											 BList * aList)
					 : BWindow(aFrame,
							   "UrlPopUpWindow",
							   B_NO_BORDER_WINDOW_LOOK,
							   B_FLOATING_APP_WINDOW_FEEL,
							   B_NOT_MOVABLE | B_NOT_CLOSABLE | B_NOT_ZOOMABLE |
							   B_NOT_MINIMIZABLE | B_WILL_ACCEPT_FIRST_CLICK |
							   B_AVOID_FOCUS | B_ASYNCHRONOUS_CONTROLS,
							   B_CURRENT_WORKSPACE) {

	parentwindow = aParent;
	lastitem = 0;
	mUrlList = new BList();
	
	mMaxHeight = (int32) Bounds().bottom;
	mItemHeight = 0;
	mUrlListView = new ThemisUrlPopUpView(
		Bounds());
	mUrlListView->SetFontSize(10.0);
	ListToDisplay(aList);

	AddChild(mUrlListView);
}

ThemisUrlPopUpWindow::~ThemisUrlPopUpWindow() {
	
	EmptyUrlList();
	delete mUrlList;

}

void ThemisUrlPopUpWindow :: SetUrlSelection(int aOffset) {


	const char * urlText = mUrlListView->SetUrlSelection(aOffset);

	if (urlText) {
		BMessenger messenger(parentwindow);
		BMessage message(SET_NAV_URL);
		message.AddString("url", urlText);
		messenger.SendMessage(&message);
	}

}

void ThemisUrlPopUpWindow :: EmptyUrlList() {

	UrlItem * item = NULL;
	
	int32 length = mUrlList->CountItems();
	
	for (int32 i = 0; i < length; i++) {
		item = (UrlItem *)mUrlList->RemoveItem((int32)0);
		delete item;
	}
	
}


void ThemisUrlPopUpWindow :: MessageReceived(BMessage *msg) {

	switch (msg->what) {
		case URL_SELECT_MOUSE: {
			// set the urlview-text
			SetUrlSelection();
			
			// doubleclick opens url
			uint32 clickitem = mUrlListView->CurrentSelection();
			if (msg->HasBool( "doubleclick" )) {
				if (msg->FindBool("doubleclick") == true) {
					if (clickitem == lastitem) {
						BMessenger msgr(NULL, parentwindow, NULL);
						msgr.SendMessage(new BMessage(URL_OPEN));
					}
				}
			}
			lastitem = clickitem;
			
			break;
		}
		case URL_SELECT_NEXT: {
			SetUrlSelection(1);
			break;
		}
		case URL_SELECT_PREV: {
			SetUrlSelection(-1);
			break;
		}
		case B_MOUSE_WHEEL_CHANGED: {
			float value = msg->FindFloat( "be:wheel_delta_y");
			value *= 10;
			mUrlListView->ScrollBy(0, value);

			break;
		}
		default:
			BWindow::MessageReceived(msg);
	}
}

void ThemisUrlPopUpWindow :: ListToDisplay(BList * list) {

	if (list != NULL) {
		mUrlListView->MakeEmpty();
		EmptyUrlList();
		mUrlList->AddList(list);
		mUrlListView->AddList(mUrlList);
		
		ResizeToPrefered();
	}
}

void ThemisUrlPopUpWindow :: ResizeToPrefered() {

	parentwindow->Lock();
	float urlview_width = ((Win*)parentwindow)->GetBoundsOfUrlView().right;
	parentwindow->Unlock();
	
	// Calculate the total height of the items to see if we should shrink the list view.
	if (mItemHeight == 0) {
		// Only calculate the height once. It won't change.
		mItemHeight = mUrlListView->FirstItem()->Height();
		if (mItemHeight == 0) {
			// Happens sometimes and I don't know why. Get the lineheight as an approximation.
			font_height height;
			mUrlListView->GetFontHeight(&height);
			mItemHeight = height.ascent + height.descent + height.leading;
			
		}
	}
	
	int32 count = mUrlListView->CountItems();
	
	int32 listHeight = (int32) ((mItemHeight * count) + (1.9 * count) + 1);
	if (listHeight > mMaxHeight) {
		mUrlListView->SetScrollBarRange(0, listHeight - mUrlListView->Bounds().bottom);
		listHeight = mMaxHeight;
	}
	else {
		mUrlListView->RemoveScrollBar();
	}
	
	// Only resize if needed
	BRect rect = Bounds();
	int32 width = (int32) rect.Width();
	int32 height = (int32) rect.Height();
	if ((width != urlview_width) || (height != listHeight)) {
		ResizeTo(urlview_width, listHeight);
	}
	
}
