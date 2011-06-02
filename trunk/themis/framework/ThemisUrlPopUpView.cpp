/* ThemisUrlPopUpView.cpp */

// Standard C++ headers
#include <iostream>
#include <string>

// Themis headers
#include "../common/commondefs.h"
#include "ThemisUrlPopUpView.h"

ThemisUrlPopUpView :: ThemisUrlPopUpView(BRect frame)
				   : BView(frame,
						   "THEMISURLPOPUPVIEW",
						   B_FOLLOW_ALL,
						   B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE) {

	BRect rect = Bounds();
	
	ulv = new BListView(
		BRect(rect.left + 19,
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
