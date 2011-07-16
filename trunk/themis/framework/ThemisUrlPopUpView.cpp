/* ThemisUrlPopUpView.cpp */

// Standard C++ headers
#include <iostream>
#include <string>

// BeOS headers
#include <interface/ScrollBar.h>

// Themis headers
#include "../common/commondefs.h"
#include "ThemisUrlPopUpView.h"
#include "UrlItem.hpp"

ThemisUrlPopUpView :: ThemisUrlPopUpView(BRect frame)
				   : BView(frame,
						   "THEMISURLPOPUPVIEW",
						   B_FOLLOW_ALL,
						   B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE) {

	BRect rect = Bounds();
	
	ulv = new BListView(
		BRect(rect.left + 1,
			  rect.top + 1,
			  rect.right - 1,
			  rect.bottom - 1),
		"THEMISURLLISTVIEW",
		B_SINGLE_SELECTION_LIST,
		B_FOLLOW_ALL,
		B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE);
	ulv->SetFontSize(10.0);
	AddChild(ulv);
	ulv->AddFilter(new ThemisUrlPopUpViewMessageFilter(Window()));
	mScrollBar = NULL;
}

void
ThemisUrlPopUpView::Draw( BRect updaterect )
{
	//cout << "ThemisUrlPopUpView::Draw()" << endl;
	
	// compromise for now
	// make the updaterect the Bounds rectangle
	updaterect = Bounds();
			
	rgb_color hi = HighColor();
	
	SetHighColor( kColorLightBorder );
	StrokeRect( updaterect, B_SOLID_HIGH );
	
	SetHighColor( 255, 255, 255 );
	updaterect.InsetBy( 1, 1 );
	FillRect( updaterect, B_SOLID_HIGH );
	
	SetHighColor( hi );
}

const char * ThemisUrlPopUpView :: SetUrlSelection(int aOffset) {
	
	if (aOffset != 0) {
		ulv->Select(ulv->CurrentSelection() + aOffset);
		ulv->ScrollToSelection();
	}

	UrlItem * item = (UrlItem *)ulv->ItemAt(ulv->CurrentSelection());
	
	return item->Text();
	
}

int32 ThemisUrlPopUpView :: CurrentSelection() {
	
	return ulv->CurrentSelection();
	
}

void ThemisUrlPopUpView :: MakeEmpty() {
	
	ulv->MakeEmpty();
	
}

void ThemisUrlPopUpView :: AddList(BList * aList) {
	
	ulv->AddList(aList);

}

UrlItem * ThemisUrlPopUpView :: FirstItem() {
	
	return (UrlItem *) ulv->FirstItem();
	
}

int32 ThemisUrlPopUpView :: CountItems() {
	
	return ulv->CountItems();
	
}

void ThemisUrlPopUpView :: SetScrollBarRange(float aMin, float aMax) {

	if (!mScrollBar) {
		BRect scrollRect = Bounds();
		scrollRect.left = scrollRect.right - B_V_SCROLL_BAR_WIDTH;
		mScrollBar = new BScrollBar(
			scrollRect,
			"UrlScrollBar",
			ulv,
			aMin,
			aMax,
			B_VERTICAL);
		AddChild(mScrollBar);
		ulv->ResizeBy(-B_V_SCROLL_BAR_WIDTH - 1, 0);
	}
	else {
		mScrollBar->SetRange(aMin, aMax);
	}

}

void ThemisUrlPopUpView :: RemoveScrollBar() {

	if (mScrollBar) {
		RemoveChild(mScrollBar);
		mScrollBar = NULL;
		ulv->ResizeBy(B_V_SCROLL_BAR_WIDTH + 1, 0);
	}

}

/////////////////////////////////////
// ThemisUrlPopUpViewMessageFilter
/////////////////////////////////////

ThemisUrlPopUpViewMessageFilter :: ThemisUrlPopUpViewMessageFilter(BWindow* win)
								: BMessageFilter(B_ANY_DELIVERY, B_ANY_SOURCE) {
	lastbutton = 0;
}

filter_result ThemisUrlPopUpViewMessageFilter :: Filter(BMessage * msg,
														BHandler ** target) {

	filter_result result(B_DISPATCH_MESSAGE);
	
	switch(msg->what) {
		case B_MOUSE_DOWN: {
			int32 buttons, clicks;
			buttons = msg->FindInt32("buttons");
			clicks = msg->FindInt32("clicks");
			
			BMessage* selectmsg = new BMessage(URL_SELECT_MOUSE);
			
			if (buttons == lastbutton && clicks > 1) {
				selectmsg->AddBool("doubleclick", true);
			}
			lastbutton = buttons;
						
			BMessenger msgr((*target));
			msgr.SendMessage(selectmsg);
			
			delete selectmsg;
			
			result = B_DISPATCH_MESSAGE;
			
			break;
		}
		default :
			break;
	}

	return result;
}
