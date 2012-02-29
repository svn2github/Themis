/*
	Copyright (c) 2011 Mark Hellegers. All Rights Reserved.
	
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
	Class Start Date: January 16, 2011
*/

/*	CSSScrolledRendererView implementation
	See CSSScrolledRendererView.hpp for more information
	
*/

// Standard C headers
#include <stdio.h>

// BeOS headers
#include <ScrollView.h>
#include <Message.h>
#include <Entry.h>
#include <Node.h>
#include <Path.h>
#include <String.h>
#include <Messenger.h>
#include <Window.h>

// DOM headersa
#include "TNode.h"
#include "TDocument.h"

// Themis headers
#include "commondefs.h"

// CSS Renderer headers
#include "CSSScrolledRendererView.hpp"
#include "CSSRendererView.hpp"

CSSScrolledRendererView :: CSSScrolledRendererView(TDocumentPtr aDocument,
												   CSSStyleSheetPtr aStyleSheet,
												   float aWidth,
												   float aHeight,
												   int32 aSiteId,
												   int32 aUrlId)
				  		: BView(BRect(0, 0, aWidth, aHeight),
							"CSSScrolledRendererView",
							B_FOLLOW_ALL_SIDES,
							B_WILL_DRAW | B_FRAME_EVENTS | B_FULL_UPDATE_ON_RESIZE) {

	printf("Creating CSS Scrolled Renderer View\n");
	mStyleSheet = aStyleSheet;
	mView = NULL;
	mDocument = aDocument;
	BRect rect = Bounds();
	rect.bottom -= B_H_SCROLL_BAR_HEIGHT;
	rect.right -= B_V_SCROLL_BAR_WIDTH;
	mView = new CSSRendererView(rect, mDocument, mStyleSheet, aSiteId, aUrlId);
	mScrollView = new BScrollView("CSS Scroll view",
								  mView,
								  B_FOLLOW_ALL_SIDES,
								  0,
								  true,
								  true);
	mScrollView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	AddChild(mScrollView);

	SetScrollbars();

}
				
CSSScrolledRendererView :: ~CSSScrolledRendererView() {

	printf("Destroying CSSScrolledRendererView\n");
/*
	if (mScrollView) {
		RemoveChild(mScrollView);
		delete mScrollView;
		delete mView;
	}
*/
	printf("Destroyed CSSScrolledRendererView\n");

}

void CSSScrolledRendererView :: SetScrollbars() {

	BRect rect = Bounds();
	rect.bottom -= B_H_SCROLL_BAR_HEIGHT;
	rect.right -= B_V_SCROLL_BAR_WIDTH;

	BScrollBar * bar = mScrollView->ScrollBar(B_VERTICAL);
	BRect newRect = mView->CSSViewSize();
//	printf("Vertical: Old height: %f, new height: %f\n", rect.Height(), newRect.Height());
	if (newRect.Height() > rect.Height()) {
		bar->SetRange(0, newRect.Height() - rect.bottom);
		bar->SetProportion(rect.Height() / newRect.Height());
	}
	else {
		bar->SetRange(0, 0);
	}
	bar = mScrollView->ScrollBar(B_HORIZONTAL);
//	printf("Horizontal: Old width: %f, new width: %f\n", rect.Width(), newRect.Width());
	if (newRect.Width() > rect.Width()) {
		bar->SetRange(0, newRect.Width() - rect.right);
		bar->SetProportion(rect.Width() / newRect.Width());
	}
	else {
		bar->SetRange(0, 0);
	}
}

void CSSScrolledRendererView :: FrameResized(float aWidth, float aHeight) {

	SetScrollbars();
	
}

void CSSScrolledRendererView :: MessageReceived(BMessage * aMessage) {
	
	switch (aMessage->what) {
		case B_SIMPLE_DATA: {
			// Get the ref that is dropped.
			entry_ref ref;
			aMessage->FindRef("refs", &ref);
			BEntry entry(&ref);
			BPath path;
			entry.GetPath(&path);
			BNode entryNode(&entry);
			char attributeValue[B_MIME_TYPE_LENGTH];
			ssize_t attributeSize = entryNode.ReadAttr("BEOS:TYPE", B_STRING_TYPE, 0, attributeValue, sizeof(attributeValue) - 1);
			if (attributeSize > 0) {
				BString mimeString = attributeValue;
				if (mimeString == "application/x-vnd.Be-bookmark") {
					attributeSize = entryNode.ReadAttr("META:url", B_STRING_TYPE, 0, attributeValue, sizeof(attributeValue) - 1);
					if (attributeSize > 0) {
						BMessage message(URL_OPEN);
						message.AddString("url_to_open", attributeValue);
						BMessenger messenger(NULL, Window());
						messenger.SendMessage(&message);
					}
				}
				else if (mimeString.ToLower() == "text/html") {
					BString filePath = "file://";
					filePath += path.Path();
					BMessage message(URL_OPEN);
					message.AddString("url_to_open", filePath);
					BMessenger messenger(NULL, Window());
					messenger.SendMessage(&message);
				}
			}
			break;
		}
		case B_MOUSE_WHEEL_CHANGED: {
			float wheelValue = aMessage->FindFloat( "be:wheel_delta_y");
			// We multiply it by 50 to make the scrolling usable.
			float scrollValue = wheelValue * 50;
			// Before we scroll, we check if we are still within the limits.
			BScrollBar * bar = mScrollView->ScrollBar(B_VERTICAL);
			float min = 0;
			float max = 0;
			bar->GetRange(&min, &max);
			float value = bar->Value();
			if (scrollValue + value > max) {
				// Adjust it, so it fits.
				scrollValue = max - value;
			}
			else if (scrollValue + value < min) {
				// Adjust it, so it fits.
				scrollValue = min - value;
			}
			if (scrollValue != 0) {
				// Only scroll when it has any effect.
				mView->ScrollBy(0, scrollValue);
			}
			break;
		}
		default: {
			BView::MessageReceived(aMessage);
		}
	};
	
}
