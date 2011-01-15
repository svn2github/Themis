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
	Class Start Date: September 21, 2002
*/
/*
	DOMViewer implementation
	See DOMViewer.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// DOMViewer headers
#include "DOMViewer.hpp"
#include "DOMView.h"

// BeOS headers
#include <Messenger.h>
#include <Autolock.h>

// Themis headers
#include "framework/app.h"
#include "framework/SiteHandler.h"
#include "framework/SiteEntry.h"
#include "DOMEntry.hpp"
#include "commondefs.h"

DOMViewer * viewer;
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
		viewer = new DOMViewer(message);
	}
	else {
		viewer = new DOMViewer();
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

DOMViewer :: DOMViewer(BMessage * aInfo)
		  : BHandler("DOMViewer"), PlugClass(aInfo) {

	mView = NULL;

}

DOMViewer :: ~DOMViewer() {

	if (mView) {
		BMessenger messenger(mView);
		messenger.SendMessage(B_QUIT_REQUESTED);
	}

}

void DOMViewer :: MessageReceived( BMessage * aMessage) {

}

bool DOMViewer :: IsHandler() {

	return true;

}

BHandler * DOMViewer :: Handler() {

	return this;

}

bool DOMViewer :: IsPersistent() {

	return true;

}

uint32 DOMViewer :: PlugID() {

	return 'tree';

}

char * DOMViewer :: PlugName() {

	return "DOM Viewer";

}

float DOMViewer :: PlugVersion() {

	return 0.2;

}

void DOMViewer :: Heartbeat() {

}

status_t DOMViewer :: ReceiveBroadcast(BMessage * aMessage) {

	int32 command = 0;
	aMessage->FindInt32("command", &command);

	switch (command) {
		case COMMAND_INFO: {
			// Check if it is dom data
			BString typeOfDocument;
			aMessage->FindString("type", &typeOfDocument);
			if (typeOfDocument == "dom") {
				int32 siteId = aMessage->FindInt32("site_id");
				SiteEntry * site = ((App *)be_app)->GetSiteHandler()->GetEntry(siteId);
				if (site != NULL) {
					int32 domId = aMessage->FindInt32("dom_id");
					DOMEntry * entry = (DOMEntry *) site->getEntry(domId);
					if (entry != NULL) {
						TDocumentPtr document = entry->getDocument();
						if (!mView) {
							mView = new DOMView(document);
						}
						else {
							BAutolock viewLock(mView);
							if (viewLock.IsLocked()) {
								mView->setDocument(document);
							}
						}
					}
				}
			}
			break;
		}
		default: {
			return PLUG_DOESNT_HANDLE;
		}
	}

	return PLUG_HANDLE_GOOD;

}

status_t DOMViewer :: BroadcastReply(BMessage * aMessage) {

	return B_OK;

}

uint32 DOMViewer :: BroadcastTarget() {

	return MS_TARGET_DOM_VIEWER;

}

int32 DOMViewer :: Type() {

	return TARGET_DOM;

}
