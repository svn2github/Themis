/* ThemisNavView.cpp */

// BeOS headers
#include <Button.h>
#include <Bitmap.h>
#include <Picture.h>
#include <String.h>

// Standard C++ headers
#include <iostream>

// Themis headers
#include "ThemisIcons.h"
#include "ThemisUrlView.h"
#include "ThemisUrlPopUpWindow.h"
#include "win.h"
#include "../common/commondefs.h"
#include "ThemisNavView.h"

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
	
	SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );
	
	// create the urlview first( we need its frame for go button later )
	urlview = new ThemisUrlView(
		BRect(
			rect.left + 150,
			rect.top + 4,
			rect.right - 25,
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
	// compromise for now
	// make the updaterect the Bounds rectangle
	updaterect = Bounds();
	
	rgb_color lo = LowColor();
	rgb_color panel_color = ui_color( B_PANEL_BACKGROUND_COLOR );
	
	SetLowColor( panel_color );	
	FillRect( updaterect, B_SOLID_LOW );
	
	SetLowColor( kColorDarkBorder );
	StrokeLine(
		BPoint( updaterect.left, updaterect.bottom ),
		BPoint( updaterect.right, updaterect.bottom ),
		B_SOLID_LOW );
	
	SetLowColor( kColorLightBorder );
	StrokeLine(
		BPoint( updaterect.left, updaterect.bottom - 1 ),
		BPoint( updaterect.right, updaterect.bottom - 1 ),
		B_SOLID_LOW );
	
	if( CountChildren() > 0 )
	{
		BView* child = NULL;
		for( int32 i = 0; i < CountChildren(); i++ )
		{
			child = ChildAt( i );
			child->Draw( child->Bounds() );
		}
	}
	
	SetLowColor( lo );
}

void
ThemisNavView::MouseDown( BPoint point )
{
	// check, if the urlpopupwindow is still open
	// if yes, close it and return.
	Win* win = ( Win* )Window();
	if (!win->CloseUrlPopUpWindow()) {
		BView::MouseDown(point);
	}
}

void
ThemisNavView::CreateInterfaceButtons()
{
	//cout << "ThemisNavView::CreateInterfaceButton()" << endl;
	
	// the small 16x16 bitmap for on, over, active or off
	BBitmap* smallbmp = NULL;
	
	// the pictures
	BPicture* onpic = NULL;
	BPicture* overpic = NULL;
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
	// 4-pic button
	names[0] = "BUTTONBACKPICBUTTON";
	names[1] = "BUTTONFORWARDPICBUTTON";
	names[2] = "BUTTONSTOPPICBUTTON";
	names[3] = "BUTTONHOMEPICBUTTON";
	names[4] = "BUTTONNEWTABPICBUTTON";
	// 3-pic button
	names[5] = "BUTTONRELOADPICBUTTON";
	names[6] = "BUTTONGOPICBUTTON";

	for( int i = 0; i < 7; i++ )
	{
		// either 64x16 or 48x16 behaviour
		int limit = 0;
		if( i <= 4 )
			limit = 3;
		else
			limit = 2;
		
		for( int j = 0; j <= limit; j++ )
		{
			// extract partial bitmaps
			
			smallbmp = new BBitmap( BRect( 0,0,15,15 ), B_RGB32 );
									
			unsigned char* p1 = ( unsigned char* )win->GetBitmap(i)->Bits();
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
			if( j == 1 ) overpic = tempview->EndPicture();
			if( j == 2 ) activepic = tempview->EndPicture();
			if( j == 3 ) offpic = tempview->EndPicture();
			
			delete smallbmp;
		}
		
		BMessage* msg = NULL;
		if( i == 0 ) msg = new BMessage( BUTTON_BACK );
		if( i == 1 ) msg = new BMessage( BUTTON_FORWARD );
		if( i == 2 ) msg = new BMessage( BUTTON_STOP );
		if( i == 3 ) msg = new BMessage( BUTTON_HOME );
		if( i == 4 ) msg = new BMessage( TAB_ADD );
		if( i == 5 ) msg = new BMessage( BUTTON_RELOAD ); // URL_OPEN ??
		if( i == 6 ) msg = new BMessage( URL_OPEN );
		
		// calculate the position rect
		switch( i )
		{
			case 0 : // back
			case 1 : // fwd
			case 2 : { posrect.left = 5 + ( i * 25 ); break; }		// stop
			case 3 : { posrect.left = 5 + ( 4 * 25 ); break; }		// home
			case 4 : { posrect.left = 5 + ( 5 * 25 ); break; }		// newtab
			case 5 : { posrect.left = 5 + ( 3 * 25 ); break; }		// reload
			case 6 : { posrect.left = urlview->Frame().right + 5; break; }	// go
		}
		posrect.right = posrect.left + 15;
		
		// create the BPictureButton
		buttons[i] = new TPictureButton(
			posrect,
			names[i],
			onpic,
			overpic,
			activepic,
			offpic,
			msg,
			0,
			B_WILL_DRAW );
		
		if( i == 6 )
			buttons[i]->SetResizingMode( buttons[i]->ResizingMode() | B_FOLLOW_RIGHT );
		
		// set initial behaviour for back, fwd  and stop button ( disabled )
		switch( i )
		{
			case 0 :
			case 1 :
			case 2 : buttons[i]->SetMode( 3 ); break;
		}
					
		AddChild( buttons[i] );
		
		// reset the bitmaps and pics
		smallbmp = NULL;
		onpic = NULL;
		overpic = NULL;
		activepic = NULL;
		offpic = NULL;
	}
	
	// remove the tempview
	RemoveChild( tempview );
	delete tempview;
}
