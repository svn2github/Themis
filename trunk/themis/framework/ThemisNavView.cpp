/* ThemisNavView.cpp */

// BeOS headers
#include <Button.h>
#include <Bitmap.h>
#include <Picture.h>
#include <String.h>

// C/C++ headers
#include <iostream>

// myheaders
#include "ThemisTVS.h"
#include "ThemisNavView.h"
#include "ThemisIcons.h"
#include "win.h"

ThemisNavView::ThemisNavView( BRect rect ) :
	BView( rect, "THEMISNAVVIEW", B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE )
{
}

ThemisNavView::~ThemisNavView( void )
{
}

void
ThemisNavView::AttachedToWindow( void )
{
	BRect rect = Bounds();
	
	SetViewColor( 240,240,240,0 );
	
	// create the urlview first( we need its frame for go button later )
	urlview = new ThemisUrlView(
		BRect(
			rect.left + 160,
			rect.top + 4,
			rect.right - 30,
			rect.bottom - 5 ), // minimum height automatically set by BTextControl
		"URLVIEW", B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW | //B_NAVIGABLE |
		B_FULL_UPDATE_ON_RESIZE );
	urlview->TextView()->AddFilter( new ThemisUrlViewMessageFilter( ( Win* )Window() ) );
	AddChild( urlview );
	
	// now create the buttons in my adventurous method :D
	CreateInterfaceButtons();
}

void
ThemisNavView::Draw( BRect updaterect )
{
	rgb_color lo = LowColor();
	
	SetLowColor( 240,240,240,0 );	
	FillRect( updaterect, B_SOLID_LOW );
	
	SetLowColor( 51,51,51,0 );
	StrokeLine(
		BPoint( updaterect.left, updaterect.bottom ),
		BPoint( updaterect.right, updaterect.bottom ),
		B_SOLID_LOW );
	
	SetLowColor( 187,187,187,0 );
	StrokeLine(
		BPoint( updaterect.left, updaterect.bottom - 1 ),
		BPoint( updaterect.right, updaterect.bottom - 1 ),
		B_SOLID_LOW );
	
	SetLowColor( lo );
	
	BView::Draw( updaterect );
}

void
ThemisNavView::CreateInterfaceButtons()
{
	//cout << "ThemisNavView::CreateInterfaceButton()" << endl;
	
	// the small 16x16 bitmap for on, active or off
	BBitmap* smallbmp = NULL;
	
	// the pictures
	BPicture* onpic = NULL;
	BPicture* activepic = NULL;
	BPicture* offpic = NULL;
	
	// the start rect
	BRect posrect( 10,7,25,22 );
	
	// the view in which we will draw the Pictures
	BView *tempview = new BView( BRect( 0,0,15,15 ), "TEMPVIEW", B_FOLLOW_NONE, B_WILL_DRAW );
	tempview->SetDrawingMode( B_OP_ALPHA );
	AddChild( tempview );
	
	Win* win = ( Win* )Window();
	
	// the array with button-names
	const char* names[7];
	// 3-bitmap button
	names[0] = "BUTTONBACKPICBUTTON";
	names[1] = "BUTTONFORWARDPICBUTTON";
	names[2] = "BUTTONSTOPPICBUTTON";
	names[3] = "BUTTONRELOADPICBUTTON";
	names[4] = "BUTTONHOMEPICBUTTON";
	names[5] = "BUTTONGOPICBUTTON";
	// 2-bitmap button
	names[6] = "BUTTONNEWTABPICBUTTON";
	
	for( int i = 0; i < 7; i++ )
	{
		// either 48x16 or 32x16 behaviour
		int limit = 0;
		if( i <= 5 )
			limit = 2;
		else
			limit = 1;
		
		for( int j = 0; j <= limit; j++ )
		{
			// extract partial bitmaps
			
			smallbmp = new BBitmap( BRect( 0,0,15,15 ), B_RGB32 );
									
			unsigned char* p1 = ( unsigned char* )win->bitmaps[i]->Bits();
			unsigned char* p2 = ( unsigned char* )smallbmp->Bits();
			
			for( int k = 0; k < 16; k++ )
			{	
				memcpy(
					( void* )( p2 + ( k * 64 ) ),
					( const void* )( p1 + ( ( j + ( ( limit + 1 ) * k ) ) * 64 ) ),
					64 );
			}
																	
			tempview->BeginPicture( new BPicture );
			tempview->DrawBitmap( smallbmp );
			if( j == 0 ) onpic = tempview->EndPicture();
			if( j == 1 ) activepic = tempview->EndPicture();
			if( j == 2 ) offpic = tempview->EndPicture();
		}
		
		BMessage* msg = NULL;
		if( i == 0 ) msg = new BMessage( BUTTON_BACK );
		if( i == 1 ) msg = new BMessage( BUTTON_FORWARD );
		if( i == 2 ) msg = new BMessage( BUTTON_STOP );
		if( i == 3 ) msg = new BMessage( BUTTON_RELOAD );
		if( i == 4 ) msg = new BMessage( BUTTON_HOME );
		if( i == 5 ) msg = new BMessage( URL_OPEN );
		if( i == 6 ) msg = new BMessage( TAB_ADD );
		
		// calculate the position rect
		if( i > 0 )
		{
			if( i != 5 )
			{
				posrect.left += 25;
				posrect.right = posrect.left + 15;
			}
		}
				
		// create the BPictureButton
		if( i == 5 )
		{
			buttons[i] = new BPictureButton(
				BRect(
					urlview->Frame().right + 5,
					posrect.top,
					urlview->Frame().right + 20,
					posrect.bottom ),
				names[i],
				onpic,
				activepic,
				msg,
				B_ONE_STATE_BUTTON, B_FOLLOW_RIGHT, B_WILL_DRAW );
			buttons[i]->SetDisabledOff( offpic );
			AddChild( buttons[i] );
			//buttons[i]->SetEnabled( false );
		}
		else
		{
			buttons[i] = new BPictureButton(
				posrect,
				names[i],
				onpic,
				activepic,
				msg,
				B_ONE_STATE_BUTTON, 0, B_WILL_DRAW );
			if( i != 6 )
				buttons[i]->SetDisabledOff( offpic );
			AddChild( buttons[i] );
			//if( i != 6 )
				//buttons[i]->SetEnabled( false );
		}
		
		// reset the bitmaps and pics
		smallbmp = NULL;
		onpic = NULL;
		activepic = NULL;
		offpic = NULL;
	}
	
	// remove the tempview
	RemoveChild( tempview );
	delete tempview;
}
