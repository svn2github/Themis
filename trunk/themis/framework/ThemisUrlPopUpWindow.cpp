/* ThemisUrlPopUpWindow.cpp */

// BeOS headers

// Standard C headers
#include <stdio.h>

// Standard C++ headers
#include <iostream>

// Themis headers
#include "ThemisUrlView.h"
#include "ThemisNavView.h"
#include "win.h"
#include "../common/commondefs.h"
#include "ThemisUrlPopUpWindow.h"
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
	vscroll = NULL;
	url_list = new BList;
	
	urlpopupview = new ThemisUrlPopUpView(Bounds());
	AddChild(urlpopupview);
	ListToDisplay(aList);
}

ThemisUrlPopUpWindow::~ThemisUrlPopUpWindow()
{
	UrlItem * item = NULL;
	
	int32 length = url_list->CountItems();
	for( int32 i = 0; i < length; i++ )
	{
		item = (UrlItem *)url_list->RemoveItem((int32)0);
		delete item;
	}
	
	delete url_list;
}

void ThemisUrlPopUpWindow :: SetUrlSelection(int aOffset) {

	if (aOffset != 0) {
		urlpopupview->ulv->Select(urlpopupview->ulv->CurrentSelection() + aOffset);
		urlpopupview->ulv->ScrollToSelection();
	}

	UrlItem * item = (UrlItem *)url_list->ItemAt( 
		urlpopupview->ulv->CurrentSelection());

	if (item) {
		parentwindow->Lock();
		((Win *)parentwindow)->GetNavView()->SetUrl(item->Text());
		parentwindow->Unlock();
	}

}


void
ThemisUrlPopUpWindow::MessageReceived( BMessage *msg )
{
	switch( msg->what )
	{
		case URL_SELECT_MOUSE :
		{
			// set the urlview-text
			SetUrlSelection();
			
			// doubleclick opens url
			uint32 clickitem = urlpopupview->ulv->CurrentSelection();
			if( msg->HasBool( "doubleclick" ) )
			{
				if( msg->FindBool( "doubleclick" ) == true )
				{
					if( clickitem == lastitem )
					{
						BMessenger msgr( NULL, parentwindow, NULL );
						msgr.SendMessage( new BMessage( URL_OPEN ) );
					}
				}
			}
			lastitem = clickitem;
			
			break;
		}
		case URL_SELECT_NEXT :
		{
			SetUrlSelection(1);
			break;
		}
		case URL_SELECT_PREV :
		{
			SetUrlSelection(-1);
			break;
		}
		case B_MOUSE_WHEEL_CHANGED: {
			float value = msg->FindFloat( "be:wheel_delta_y");
			value *= 10;
			urlpopupview->ulv->ScrollBy(0, value);

			break;
		}
		default :
			BWindow::MessageReceived( msg );
	}
}

void ThemisUrlPopUpWindow :: ListToDisplay(BList * list) {

	if (list != NULL) {
		// First delete the contents of the old lists.
		int32 length = url_list->CountItems();
		for (int32 i = 0; i < length; i++) {
			delete url_list->RemoveItem((int32) 0);
		}
		
		// we need to totally independant but similar BLists
		// ( copying with the copy-constructor won't do it )
		url_list->AddList(list);
		
		urlpopupview->ulv->MakeEmpty();
		urlpopupview->ulv->AddList(url_list);

		ResizeToPrefered(); 
	}
}

void
ThemisUrlPopUpWindow::ResizeToPrefered()
{
	BPoint parent_bottom = parentwindow->Bounds().RightBottom();
	BPoint parent_bottom_cts = parentwindow->ConvertToScreen( parent_bottom );
	
	float itemheight = urlpopupview->ulv->ItemAt( 0 )->Height();
	int32 count = urlpopupview->ulv->CountItems();
	
	float newheight = ( itemheight * count ) + ( 1.9 * count ) + 1;
		
	BPoint new_popup_bottom;
	new_popup_bottom.y = newheight;
	BPoint new_popup_bottom_cts = ConvertToScreen( new_popup_bottom );
	
	float difference = parent_bottom_cts.y - new_popup_bottom_cts.y - 20;
	
	float urlview_width = ((Win*)parentwindow)->GetNavView()->GetBoundsOfUrlView().right;
	
	if( difference < 0 )
	{
		// the urlpopup would reach over the parentwindows bottom
		
		new_popup_bottom.y -= -difference;
		ResizeTo( urlview_width, new_popup_bottom.y );
		
		if( vscroll == NULL )
		{
			urlpopupview->ResizeTo(
				urlpopupview->Bounds().right - B_V_SCROLL_BAR_WIDTH,
				urlpopupview->Bounds().bottom );
							
			BRect bounds = Bounds();
		
			vscroll = new BScrollBar(
				BRect(
					bounds.right - B_V_SCROLL_BAR_WIDTH,
					bounds.top,
					bounds.right,
					bounds.bottom ),
				"URLPOPUPVSCROLLBAR",
				urlpopupview->ulv,
				0,
				newheight - urlpopupview->ulv->Bounds().bottom,
				B_VERTICAL );
			AddChild( vscroll );
		}
		else
		{
			vscroll->SetRange(
				0,
				newheight - urlpopupview->ulv->Bounds().bottom );
		}
	}
	else
	{
		// the urlpopup fits into the parentwindow
		
		ResizeTo( urlview_width, new_popup_bottom.y );
				
		if( vscroll != NULL )
		{
			RemoveChild( vscroll );
			vscroll = NULL;
			
			urlpopupview->ResizeTo(
				urlpopupview->Bounds().right + B_V_SCROLL_BAR_WIDTH,
				urlpopupview->Bounds().bottom );
		}
	}
}

bool ThemisUrlPopUpWindow :: HasScrollBar() const {

	return (vscroll != NULL);

}
