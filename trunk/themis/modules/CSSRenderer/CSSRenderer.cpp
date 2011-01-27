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
	Class Start Date: Februari 28, 2010
*/

/*	CSSRenderer implementation
	See CSSRenderer.hpp for more information
	
*/

// Standard C headers
#include <stdio.h>

// BeOS headers
#include <String.h>
#include <Messenger.h>
#include <Autolock.h>

// DOM headers
#include "DOMSupport.h"
#include "TDocument.h"

// DOM Style headers
#include "CSSStyleSheet.hpp"

// Themis headers
#include "framework/app.h"
#include "framework/SiteHandler.h"
#include "framework/SiteEntry.h"
#include "DOMEntry.hpp"
#include "CSSDOMEntry.hpp"
#include "commondefs.h"

// Renderer headers
#include "CSSRenderer.hpp"
#include "CSSScrolledRendererView.hpp"
#include "CSSRendererView.hpp"

CSSRenderer * renderer;
BMessage ** appSettings_p;
BMessage * appSettings;

status_t Initialize(void * aInfo) {
	
	renderer = NULL;
	if (aInfo != NULL) {
		BMessage * message = (BMessage *) aInfo;
		if (message->HasPointer("settings_message_ptr")) {
			message->FindPointer("settings_message_ptr", (void **) & appSettings_p);
			appSettings = *appSettings_p;
		}
		renderer = new CSSRenderer(message);
	}
	else {
		renderer = new CSSRenderer();
	}

	return B_OK;

}

status_t Shutdown(bool aNow) {

	delete renderer;

	return B_OK;

}

PlugClass * GetObject()	{

	return renderer;

}

CSSRenderer :: CSSRenderer(BMessage * aInfo)
			: BHandler("CSSRenderer"),
			PlugClass(aInfo, "CSSRenderer") {

	printf("Constructing CSS Renderer\n");

}

CSSRenderer :: ~CSSRenderer() {

	printf("Destroying CSS Renderer\n");
	
}

void CSSRenderer :: MessageReceived(BMessage * aMessage) {

	BHandler::MessageReceived(aMessage);

}

bool CSSRenderer :: IsHandler() {

	return true;

}

BHandler * CSSRenderer :: Handler() {

	return this;

}

bool CSSRenderer :: IsPersistent() {

	return true;

}

uint32 CSSRenderer :: PlugID() {

	return 'cssr';

}

char * CSSRenderer :: PlugName() {

	return "CSS Renderer";

}

float CSSRenderer :: PlugVersion() {

	return 0.1;

}

void CSSRenderer :: Heartbeat() {

}

status_t CSSRenderer :: ReceiveBroadcast(BMessage * aMessage) {

	int32 command = 0;
	aMessage->FindInt32("command", &command);

	switch (command) {
		case COMMAND_INFO: {
			switch (aMessage->what) {
				case SH_PARSE_DOC_FINISHED: {
					BString type;
					aMessage->FindString("type", &type);
					if (type == "cssdom") {
						// Get the pointer out
						printf("Got the CSS DOM\n");
						int32 siteId = 0;
						aMessage->FindInt32("site_id", &siteId);

						SiteHandler * sh = ((App *)be_app)->GetSiteHandler();
						SiteEntry * site = sh->GetEntry(siteId);
						if (site != NULL) {
							int32 cssId = 0;
							aMessage->FindInt32("cssdom_id", &cssId);
							CSSDOMEntry * entry = (CSSDOMEntry *) site->getEntry(cssId);
							if (entry != NULL) {
								DOMEntry * domEntry = (DOMEntry *) entry->getParent();
								if (domEntry != NULL) {
									CSSStyleSheetPtr stylesheet = entry->getStyleSheet();
									TDocumentPtr document = domEntry->getDocument();
									int width = 0;
									int height = 0;
									site->GetSize(width, height);
									CSSScrolledRendererView * view =
										new CSSScrolledRendererView(document,
																	stylesheet,
																	width,
																	height);
									/* Get an unique ID from the app for the DOM entry */
									int32 viewId = ((App *)be_app)->GetNewID();
									BaseEntry * entry = new BaseEntry(viewId);
									entry->set("render_view", (void *) view);
									((App *)be_app)->GetSiteHandler()->AddEntry(entry, siteId, domEntry->getId());
								
									int32 urlId = 0;
									aMessage->FindInt32("url_id", &urlId);
									//Do the Broadcasting to give the view to the UI
									BMessage *message = new BMessage(SH_RENDER_FINISHED);
									message->AddInt32("command", COMMAND_INFO);
									message->AddInt32("site_id", siteId);
									message->AddInt32("url_id", urlId);
									message->AddInt32("view_id", viewId);
									Broadcast(MS_TARGET_ALL, message);
								}
							}
							
						}
					}
					break;
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

status_t CSSRenderer :: BroadcastReply(BMessage * aMessage) {

	return B_OK;

}

uint32 CSSRenderer :: BroadcastTarget() {

	return MS_TARGET_RENDERER;

}

int32 CSSRenderer :: Type() {

	return TARGET_RENDERER;

}
