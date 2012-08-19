/*
Copyright (c) 2002 Raymond "Z3R0 One" Rodgers. All Rights Reserved. 

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

Original Author & Project Manager: Raymond "Z3R0 One" Rodgers (z3r0_one@yahoo.com)
Project Start Date: October 18, 2000
*/

// Standard C+ headers
#include <string.h>

// BeOS headers
#include <Application.h>
#include <Messenger.h>
#include <Window.h>
#include <String.h>
#include <TextView.h>
#include <Box.h>
#include <ListView.h>
#include <List.h>
#include <Button.h>
#include <ScrollView.h>

// Themis headers
#include "appaboutview.h"
#include "plugclass.h"
#include "aboutview.h"
#include "AboutItem.hpp"

#define SelectionChanged 'selc'

int sortaboutitems(const void * aFirst, const void * aSecond) {

	int result = 0;

	if ((aFirst == NULL) && (aSecond != NULL)) {
		result = 1;
	}
	else if ((aFirst != NULL) && (aSecond == NULL)) {
		result = -1;
	}
	else if ((aFirst == NULL) && (aSecond == NULL)) {
		result = 0;
	}
	else {
		AboutItem *firstItem, *secondItem;
		firstItem = *((AboutItem**) aFirst);
		secondItem = *((AboutItem**) aSecond);
		switch (firstItem->Type()) {
			case APPLICATION_TYPE: {
				result = -1;
				break;
			}
			case LIBRARY_TYPE: {
				if (secondItem->Type() == APPLICATION_TYPE) {
					result = 1;
				}
				else if (secondItem->Type() != LIBRARY_TYPE) {
					result = -1;
				}
				else {
					BString firstString = firstItem->Text();
					BString secondString = secondItem->Text();
					result = firstString.Compare(secondString);
				}
				break;
			}
			case PLUGIN_TYPE: {
				if (secondItem->Type() != PLUGIN_TYPE) {
					result = 1;
				}
				else {
					BString firstString = firstItem->Text();
					BString secondString = secondItem->Text();
					result = firstString.Compare(secondString);
				}
			}
		}
	}
	
	return result;

}

aboutview :: aboutview(BRect frame,
					   const char *name,
					   uint32 resizem,
					   uint32 flags)
		  : BView(frame, name, resizem, flags) {

	SetViewColor(216, 216, 216);
	BRect r(Bounds());
	r.InsetBy(10, 10);
	r.bottom -= 25;
	outerbox= new BBox(
		r,
		"outer-about-box",
		B_FOLLOW_ALL,
		B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE_JUMP,
		B_FANCY_BORDER);
	AddChild(outerbox);
	r = Bounds();
	r.bottom -= 15.0;
	r.top = r.bottom-15.0;
	r.left = (r.right / 2.0) - 20.0;
	r.right= (r.right / 2.0) + 20.0;
	OkB = new BButton(
		r,
		"Ok-button",
		"Done",
		(new BMessage(B_QUIT_REQUESTED)),
		B_WILL_DRAW | B_NAVIGABLE,
		B_FOLLOW_BOTTOM | B_FOLLOW_H_CENTER);
	OkB->MakeDefault(true);
	AddChild(OkB);
	r = outerbox->Bounds();
	r.right /= 3.0;
	r.InsetBy(5.0, 5.0);
	r.right -= B_V_SCROLL_BAR_WIDTH;
	mListView = new BListView(
		r,
		"about_items_listv",
		B_SINGLE_SELECTION_LIST,
		B_FOLLOW_ALL,
		B_WILL_DRAW |B_NAVIGABLE | B_FRAME_EVENTS);
	mListView->SetSelectionMessage((new BMessage(SelectionChanged)));
	scroll = new BScrollView(
		"about_list_scroller",
		mListView,
		B_FOLLOW_H_CENTER | B_FOLLOW_TOP_BOTTOM | B_FOLLOW_LEFT,
		B_WILL_DRAW | B_NAVIGABLE_JUMP,
		false,
		true,
		B_FANCY_BORDER);
	outerbox->AddChild(scroll);
	r.left = r.right + 20.0;
	r.right = outerbox->Bounds().right - 5.0;
	innerbox = new BBox(
		r,
		"inner-about-box",
		B_FOLLOW_TOP_BOTTOM | B_FOLLOW_LEFT_RIGHT,
		B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE_JUMP,
		B_FANCY_BORDER);
	outerbox->AddChild(innerbox);
	//add known items for about list.

	mListView->AddItem(new AboutItem("Themis Framework", APPLICATION_TYPE));
	mListView->AddItem(new AboutItem("SSL", LIBRARY_TYPE));
	mListView->AddItem(new AboutItem("DOM", LIBRARY_TYPE));
	mListView->AddItem(new AboutItem("DOM Style", LIBRARY_TYPE));
}

aboutview :: ~aboutview() {

}

BView * aboutview :: CreateSimpleAboutView(
	BRect aRect,
	const char * aName,
	const char * aText) {

	aRect.right -= B_V_SCROLL_BAR_WIDTH;
	BTextView * view = new BTextView(
		aRect,
		aName,
		aRect,
		B_FOLLOW_ALL,
		B_WILL_DRAW);
	view->MakeEditable(false);
	view->Insert(aText);
	BScrollView * scrollView = new BScrollView(
		"about_view_scroller",
		view,
		B_FOLLOW_ALL,
		B_WILL_DRAW | B_NAVIGABLE_JUMP,
		false,
		true,
		B_FANCY_BORDER);

	return scrollView;

}

void aboutview :: MessageReceived(BMessage *msg) {

	switch(msg->what) {
		case B_QUIT_REQUESTED: {
			BMessenger msgr(Window());
			msgr.SendMessage(B_QUIT_REQUESTED);
			break;
		};
		case SelectionChanged:
		{
			int32 index = 0;
			msg->FindInt32("index", &index);
			AboutItem *item = (AboutItem *)mListView->ItemAt(index);
			if (item) {
				if (innerbox->CountChildren() > 0) {
					BView *child = NULL;
					for (int32 i = 0; i < innerbox->CountChildren(); i++) {
						child = innerbox->ChildAt(i);
						innerbox->RemoveChild(child);
						delete child;
					}
				}
				
				BRect r = innerbox->Bounds();
				r.InsetBy(5.0, 5.0);
				switch (item->Type()) {
					case APPLICATION_TYPE: {
						innerbox->AddChild(
							new appaboutview(
								r,
								"About Themis View",
								B_FOLLOW_ALL,
								B_WILL_DRAW
							)
						);
						break;
					}
					case PLUGIN_TYPE: {
						/* Currently not implemented. This never worked,
						   so we don't lose any functionality.
						   Have to think about how to implement this a bit.
						*/
						break;
					}
					case LIBRARY_TYPE: {
						BString itemText = item->Text();
						if (itemText == "SSL") {
							BView * view = CreateSimpleAboutView(
								r,
								"About SSL",
								"SSL and other encryption support is provided by libcrypt:\nhttp://www.cs.auckland.ac.nz/~pgut001/cryptlib/\n"
							);
							innerbox->AddChild(view);
						}
						else if (itemText == "DOM") {
							BView * view = CreateSimpleAboutView(
								r,
								"About DOM",
								"DOM library is custom created for Themis.\nBased on the specs provided by the w3c at:\nhttp://www.w3.org/TR/2004/REC-DOM-Level-3-Core-20040407/\n"
							);
							innerbox->AddChild(view);
						}
						else if (itemText == "DOM Style") {
							BView * view = CreateSimpleAboutView(
								r,
								"About DOM Style",
								"DOM Style library is custom created for Themis.\nBased on the specs provided by the w3c at:\nhttp://www.w3.org/TR/2000/REC-DOM-Level-2-Style-20001113/\n"
							);
							innerbox->AddChild(view);
						}
						break;
					}
				}
			}
			break;
		}  // case SelectionChanged:
		default: {
			BView::MessageReceived(msg);
		}
	}
}

void aboutview :: AttachedToWindow() {

	LockLooper();
	OkB->SetTarget(this);
	mListView->SortItems(sortaboutitems);
	mListView->SetTarget(this);
	mListView->Select(0);
	UnlockLooper();

}
