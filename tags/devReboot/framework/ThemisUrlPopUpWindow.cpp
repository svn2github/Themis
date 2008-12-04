/* ThemisUrlPopUpWindow.cpp */

// BeOS headers

// C/C++ headers
#include <iostream>
#include <stdio.h>

// myheaders
#include "ThemisUrlPopUpWindow.h"
#include "win.h"
#include "../common/commondefs.h"

ThemisUrlPopUpWindow::ThemisUrlPopUpWindow( BWindow* parent, BRect frame )
	: BWindow(
			frame,
			"UrlPopUpWindow",
			B_NO_BORDER_WINDOW_LOOK,
			B_FLOATING_APP_WINDOW_FEEL,
			B_NOT_MOVABLE | B_NOT_CLOSABLE | B_NOT_ZOOMABLE |
				B_NOT_MINIMIZABLE | B_WILL_ACCEPT_FIRST_CLICK |
				B_AVOID_FOCUS | B_ASYNCHRONOUS_CONTROLS,
			B_CURRENT_WORKSPACE )
{
	parentwindow = parent;
	lastitem = 0;
	vscroll = NULL;
	url_list = NULL;
	trunc_list = NULL;
	
	urlpopupview = new ThemisUrlPopUpView(
		Bounds() );
	AddChild( urlpopupview );
}

ThemisUrlPopUpWindow::~ThemisUrlPopUpWindow()
{
	BStringItem* item = NULL;
	
	for( int32 i = 0; i < url_list->CountItems(); i++ )
	{
		item = ( BStringItem* )url_list->RemoveItem( (int32)0 );
		delete item;
	}
	
	for(int32 i = 0; i < url_list->CountItems(); i++ )
	{
		item = ( BStringItem* )trunc_list->RemoveItem( (int32)0 );
		delete item;
	}
	
	delete url_list;
	delete trunc_list;
}

void
ThemisUrlPopUpWindow::MessageReceived( BMessage *msg )
{
	switch( msg->what )
	{
		case URL_SELECT_MOUSE :
		{
			// set the urlview-text
			//cout << "URL_SELECT_MOUSE" << endl;
			
			BStringItem* item = ( BStringItem* )url_list->ItemAt( 
				urlpopupview->ulv->CurrentSelection() );
						
			parentwindow->Lock();
			((Win*)parentwindow)->navview->urlview->SetText( item->Text() );
			parentwindow->Unlock();
			
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
			//cout << "URL_SELECT_NEXT received" << endl;
			Lock();
			urlpopupview->ulv->Select(
				urlpopupview->ulv->CurrentSelection() + 1 );
			urlpopupview->ulv->ScrollToSelection();
			
			// set the urlview-text
			BStringItem* item = ( BStringItem* )url_list->ItemAt( 
				urlpopupview->ulv->CurrentSelection() );
			if( !item )
				break;
			
			parentwindow->Lock();
			((Win*)parentwindow)->navview->urlview->SetText( item->Text() );
			parentwindow->Unlock();
			
			Unlock();				
			break;
		}
		case URL_SELECT_PREV :
		{
			//cout << "URL_SELECT_PREV received" << endl;
			Lock();
			urlpopupview->ulv->Select(
				urlpopupview->ulv->CurrentSelection() - 1 );
			urlpopupview->ulv->ScrollToSelection();
			
			// set the urlview-text
			BStringItem* item = ( BStringItem* )url_list->ItemAt( 
				urlpopupview->ulv->CurrentSelection() );
			if( !item )
				break;
			
			parentwindow->Lock();
			((Win*)parentwindow)->navview->urlview->SetText( item->Text() );
			parentwindow->Unlock();
			
			Unlock();
			break;
		}
		default :
			BWindow::MessageReceived( msg );
	}
}

void
ThemisUrlPopUpWindow::ListToDisplay( BList* list )
{
	//cout << "ThemisUrlPopUpWindow::ListToDisplay()" << endl;
	
	if( list != NULL )
	{
		// we need the url_list with untruncated strings
		url_list = new BList( *list );
		// and the list with truncated urls if urlpopup is
		// smaller then the urls-length
		trunc_list = new BList( url_list->CountItems() );
		
		// we need to totally independant but similar BLists
		// ( copying with the copy-constructor won't do it )
		int i = 0;
		BStringItem* item;
		while( url_list->ItemAt( i ) != NULL )
		{
			item = ( BStringItem* )url_list->ItemAt( i );
			trunc_list->AddItem( new BStringItem( item->Text() ) );
			i++;
		}
		
		TruncateUrlStrings();
		
		urlpopupview->ulv->MakeEmpty();
		urlpopupview->ulv->AddList( trunc_list );
		
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
	
	float urlview_width = ((Win*)parentwindow)->navview->urlview->Bounds().right;
	
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
		TruncateUrlStrings();
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
		TruncateUrlStrings();
	}
}

void
ThemisUrlPopUpWindow::TruncateUrlStrings()
{
	int i = 0;
	BStringItem* urlitem = NULL;
	BStringItem* truncitem = NULL;
	BString* string = new BString;
	
	while( url_list->ItemAt( i ) != NULL )
	{
		urlitem = ( BStringItem* )url_list->ItemAt( i );
		if( urlitem != NULL );
			string->SetTo( urlitem->Text() );
				
		urlpopupview->TruncateString(
			string,
			B_TRUNCATE_END,
			urlpopupview->ulv->Bounds().right - 10 );
		
		truncitem = ( BStringItem* )urlpopupview->ulv->ItemAt( i );
		if( truncitem != NULL )
			truncitem->SetText( string->String() );
					
		i++;
	}
	delete string;
}
