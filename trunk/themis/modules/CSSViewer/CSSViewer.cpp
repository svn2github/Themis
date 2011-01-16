/*
	Copyright (c) 2001 Mark Hellegers. All Rights Reserved.
	
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
	Class Start Date: November 22, 2009
*/
/*
	CSSViewer implementation
	See CSSViewer.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// CSSViewer headers
#include "CSSViewer.hpp"
#include "CSSView.hpp"

// DOM Style headers
#include "CSSStyleSheet.hpp"

// BeOS headers
#include <Messenger.h>
#include <Autolock.h>

// Themis headers
#include "framework/app.h"
#include "framework/SiteHandler.h"
#include "framework/SiteEntry.h"
#include "CSSDOMEntry.hpp"
#include "commondefs.h"

CSSViewer * viewer;
BMessage ** appSettings_p;
BMessage * appSettings;

status_t Initialize(void * aInfo) {

	viewer = NULL;
	if (aInfo != NULL) {
		BMessage * message = (BMessage *) aInfo;
		if (message->HasPointer("settings_message_ptr")) {
			message->FindPointer("settings_message_ptr", (void **) & appSettings_p);
			appSettings = *appSettings_p;
		}
		viewer = new CSSViewer(message);
	}
	else {
		viewer = new CSSViewer();
	}

	return B_OK;

}

status_t Shutdown(bool aNow) {

	delete viewer;

	return B_OK;

}

PlugClass * GetObject() {

	return viewer;

}

CSSViewer :: CSSViewer(BMessage * aInfo)
		  : BHandler("CSSViewer"), PlugClass(aInfo) {

	mView = NULL;

}

CSSViewer :: ~CSSViewer() {

	if (mView) {
		BMessenger messenger(mView);
		messenger.SendMessage(B_QUIT_REQUESTED);
	}
}

void CSSViewer :: MessageReceived(BMessage * aMessage) {

}

bool CSSViewer :: IsHandler() {

	return true;

}

BHandler * CSSViewer :: Handler() {

	return this;

}

bool CSSViewer :: IsPersistent() {

	return true;

}

uint32 CSSViewer :: PlugID() {

	return 'csst';

}

char * CSSViewer :: PlugName() {

	return "CSS Viewer";

}

float CSSViewer :: PlugVersion() {

	return 0.2;

}

void CSSViewer :: Heartbeat() {

}

status_t CSSViewer :: ReceiveBroadcast(BMessage * aMessage) {

	int32 command = 0;
	aMessage->FindInt32("command", &command);

	switch (command) {
		case COMMAND_INFO: {
			// Check if it is css dom data
			BString typeOfDocument;
			aMessage->FindString("type", &typeOfDocument);
			if (typeOfDocument == "cssdom") {
				int32 siteId = aMessage->FindInt32("site_id");
				SiteEntry * site = ((App *)be_app)->GetSiteHandler()->GetEntry(siteId);
				if (site != NULL) {
					int32 cssId = aMessage->FindInt32("cssdom_id");
					CSSDOMEntry * entry = (CSSDOMEntry *) site->getEntry(cssId);
					if (entry != NULL) {
						CSSStyleSheetPtr document = entry->getStyleSheet();
						if (!mView) {
							mView = new CSSView(document);
						}
						else {
							BAutolock viewLock(mView);
							if (viewLock.IsLocked()) {
								mView->SetStyleSheet(document);
							}
						}
					}
				}
			}
			break;
		}
		default:	{
			return PLUG_DOESNT_HANDLE;
		}
	}
	
	return PLUG_HANDLE_GOOD;
	
}

status_t CSSViewer :: BroadcastReply(BMessage * aMessage) {

	return B_OK;

}

uint32 CSSViewer :: BroadcastTarget() {

	return MS_TARGET_DOM_VIEWER;

}

int32 CSSViewer :: Type() {

	return TARGET_DOM;

}
