/* ThemisUrlPopUpView.cpp */

// BeOS headers
//#include <Window.h>

// C/C++ headers
#include <iostream>
#include <string.h>

// myheaders
#include "ThemisUrlPopUpView.h"
#include "win.h"

ThemisUrlPopUpView::ThemisUrlPopUpView(
	BRect frame )
	: BView(
			frame,
			"THEMISURLPOPUPVIEW",
			B_FOLLOW_ALL,
			B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE )
{
	BRect rect = Bounds();
	
	ulv = new ThemisUrlListView(
		BRect(
			rect.left + 19,
			rect.top + 1,
			rect.right - 1,
			rect.bottom - 1 ) );
	AddChild( ulv );
	ulv->AddFilter( new ThemisUrlPopUpViewMessageFilter( Window() ) );
}

void
ThemisUrlPopUpView::Draw( BRect updaterect )
{
	//cout << "ThemisUrlPopUpView::Draw()" << endl;
			
	rgb_color hi = HighColor();
	
	SetHighColor( 255,255,255 );
	FillRect( updaterect, B_SOLID_HIGH );
	//FillRect( updaterect, B_SOLID_LOW );
	
	SetHighColor( 190,190,190 );
	StrokeRect( updaterect, B_SOLID_HIGH );
	
	
	
	SetHighColor( hi );
}

/////////////////////////////////////
// ThemisUrlListView
/////////////////////////////////////

ThemisUrlListView::ThemisUrlListView(
	BRect frame )
	: BListView(
			frame,
			"THEMISURLLISTVIEW",
			B_SINGLE_SELECTION_LIST,
			B_FOLLOW_ALL,
			B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE )
{
	SetFontSize( 10.0 );
}

/////////////////////////////////////
// ThemisUrlPopUpViewMessageFilter
/////////////////////////////////////

ThemisUrlPopUpViewMessageFilter::ThemisUrlPopUpViewMessageFilter( BWindow* win )
	: BMessageFilter( B_ANY_DELIVERY, B_ANY_SOURCE )
{
	window = win;
	lastbutton = 0;
}

filter_result
ThemisUrlPopUpViewMessageFilter::Filter( BMessage *msg, BHandler **target )
{
	filter_result result( B_DISPATCH_MESSAGE );
	
	switch( msg->what )
	{
		case B_MOUSE_DOWN :
		{
			int32 buttons, clicks;
			buttons = msg->FindInt32( "buttons" );
			clicks = msg->FindInt32( "clicks" );
			
			BMessage* selectmsg = new BMessage( URL_SELECT_MOUSE );
			
			if( buttons == lastbutton && clicks > 1 )
			{
				selectmsg->AddBool( "doubleclick", true );
			}
			lastbutton = buttons;
						
			BMessenger msgr( (*target) );
			msgr.SendMessage( selectmsg );
			
			result = B_DISPATCH_MESSAGE;
			
			break;
		}
		default :
			break;
	}
	return result;
}
