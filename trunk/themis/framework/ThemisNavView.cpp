/* ThemisNavView.cpp */

// BeOS headers
#include <Button.h>
#include <Bitmap.h>
#include <Picture.h>
#include <String.h>

// C/C++ headers
#include <iostream>

// myheaders
#include "ThemisNavView.h"
#include "ThemisIcons.h"
#include "win.h"
#include "app.h"

class ThemisPictureButton;

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
	
	union int32torgb convert;
	AppSettings->FindInt32( "PanelColor", &convert.value );
	SetViewColor( convert.rgb );
	
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
	
	union int32torgb convert;
	AppSettings->FindInt32( "PanelColor", &convert.value );
	SetLowColor( convert.rgb );	
	FillRect( updaterect, B_SOLID_LOW );
	
	// the 'shadow' at the bottom of navview
	AppSettings->FindInt32( "DarkBorderColor", &convert.value );
	SetLowColor( convert.rgb );
	StrokeLine(
		BPoint( updaterect.left, updaterect.bottom ),
		BPoint( updaterect.right, updaterect.bottom ),
		B_SOLID_LOW );
	
	AppSettings->FindInt32( "PanelColor", &convert.value );
	if( convert.rgb.red - 32 >= 0 )
		convert.rgb.red -= 32;
	if( convert.rgb.green - 32 >= 0 )
		convert.rgb.green -= 32;
	if( convert.rgb.blue - 32 >= 0 )
		convert.rgb.blue -= 32;
	SetLowColor( convert.rgb );
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
	if( win->urlpopupwindow != NULL )
	{
		win->urlpopupwindow->Lock();
		win->urlpopupwindow->Quit();
		win->urlpopupwindow = 0;
		return;
	}
	BView::MouseDown( point );
}

void
ThemisNavView::CreateInterfaceButtons()
{
	//cout << "ThemisNavView::CreateInterfaceButton()" << endl;
	
	// the small 16x16 bitmap for on, active or off
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
		buttons[i] = new ThemisPictureButton(
			posrect,
			names[i],
			onpic,
			activepic,
			msg,
			B_ONE_STATE_BUTTON, 0, B_WILL_DRAW );
		
		if( i == 6 )
			buttons[i]->SetResizingMode( buttons[i]->ResizingMode() | B_FOLLOW_RIGHT );
		buttons[i]->SetDisabledOn( overpic );
		if( i <= 4 )
			buttons[i]->SetDisabledOff( offpic );
		else
			buttons[i]->SetDisabledOff( overpic );
			// ^^ unfortunately we even need a disabled-off pic for
			// the buttons which may not be disabled .. 
			// imho i am not setting it to disabled-off when using
			// SetEnabled() and SetValue() in Mouse*() methods in ThemisPictureButton
			// but it still crashes when drawing...
			// will have to look at it later again
			
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

/////////////////////////////////////
// ThemisPictureButton
/////////////////////////////////////

ThemisPictureButton::ThemisPictureButton(
	BRect rect,
	const char* name,
	BPicture* off,
	BPicture* on,
	BMessage* message,
	uint32 behaviour,
	uint32 resizingmode,
	uint32 flags )
	: BPictureButton(
		rect,
		name,
		off,
		on,
		message,
		behaviour,
		resizingmode,
		flags )
{
	i_was_disabled_on_flag = false;
}

void
ThemisPictureButton::Draw( BRect updaterect )
{
	//cout << "ThemisPictureButton::Draw()" << endl;
			
	// just erase the whole area as old picture contents are not removed
	// before drawing the new picture of the button if the buttons state
	// changed...so we erase the whole button area..
	union int32torgb convert;
	AppSettings->FindInt32( "PanelColor", &convert.value );
	SetHighColor( convert.rgb );
	FillRect( Bounds(), B_SOLID_HIGH );
	
	// this is a little trick, to cheat the BPictureButton::Draw() function.
	// if we change the panelcolor, the draw function would have drawn a rect
	// in the bounds dimension in the _old_ panelcolor..
	SetViewColor( convert.rgb );
		
	BPictureButton::Draw( updaterect );
}

void
ThemisPictureButton::MouseDown( BPoint point )
{
	//cout << "ThemisPictureButton::MouseDown()" << endl;
	
	uint32 button;
	GetMouse( &point, &button, true );
	
	// we would need a third active state for the button, but this isnt possible;
	// i just use the disabled-on picture as a container for the mouse-over pic;
	// if the button is set to disabled-on( mouse-over), it cannot be pressed, so we reset its
	// state to enabled, and remember its state with a flag for MouseUp to display
	// the correct mouse-over pic after mouse got released
	if( button == B_PRIMARY_MOUSE_BUTTON )
	{	
		if( ( IsEnabled() == false ) && ( Value() == B_CONTROL_ON ) )
		{
			/*if( IsEnabled() == false ) cout << "button is disabled-";
			else cout << "button is enabled-";
			if( Value() == B_CONTROL_ON ) cout << "on" << endl;
			else cout << "off" << endl;*/
			
			//cout << "setting to enabled-on" << endl;
			SetEnabled( true );
			SetValue( B_CONTROL_ON );
			
			/*if( IsEnabled() == false ) cout << "button is disabled-";
			else cout << "button is enabled-";
			if( Value() == B_CONTROL_ON ) cout << "on" << endl;
			else cout << "off" << endl;*/
			
			i_was_disabled_on_flag = true;
		}
	}
	BPictureButton::MouseDown( point );
}

void
ThemisPictureButton::MouseMoved( BPoint point, uint32 transit, const BMessage* message )
{
	// if the button is really disabled -> do nothing
	if( IsEnabled() == false && Value() == B_CONTROL_OFF )
	{	
		//cout << "the button is _really_ disabled -> nothing done on mousemoved" << endl;
		return;
	}
	if( transit == B_ENTERED_VIEW )
	{
		/*cout << "ThemisPictureButton::MouseMoved() : B_ENTERED_VIEW" << endl;
		if( IsEnabled() == false ) cout << "button is disabled-";
		else cout << "button is enabled-";
		if( Value() == B_CONTROL_ON ) cout << "on" << endl;
		else cout << "off" << endl;
		
		cout << "setting to disabled-on ( mouse over )" << endl;*/
		SetValue( B_CONTROL_ON );
		SetEnabled( false );
	}
	if( transit == B_EXITED_VIEW )
	{
		/*cout << "ThemisPictureButton::MouseMoved() : B_EXITED_VIEW" << endl;
		if( IsEnabled() == false ) cout << "button is disabled-";
		else cout << "button is enabled-";
		if( Value() == B_CONTROL_ON ) cout << "on" << endl;
		else cout << "off" << endl;
		
		cout << "setting to enabled-off ( normal )" << endl;*/
		SetValue( B_CONTROL_OFF );
		SetEnabled( true );
		
		// fixes "press-out-in-release"-problem
		BPictureButton::MouseUp( BPoint( 0.0, 0.0 ) );
		// clean up
		i_was_disabled_on_flag = false;
	}
}

void
ThemisPictureButton::MouseUp( BPoint point )
{
	//cout << "ThemisPictureButton::MouseUp()" << endl;
	
	BPictureButton::MouseUp( point );
	
	if( i_was_disabled_on_flag == true )
	{
		/*if( IsEnabled() == false ) cout << "button is disabled-";
		else cout << "button is enabled-";
		if( Value() == B_CONTROL_ON ) cout << "on" << endl;
		else cout << "off" << endl;
			
		cout << "setting to disabled-on" << endl;*/
		SetEnabled( false );
		SetValue( B_CONTROL_ON );
				
		/*if( IsEnabled() == false ) cout << "button is disabled-";
		else cout << "button is enabled-";
		if( Value() == B_CONTROL_ON ) cout << "on" << endl;
		else cout << "off" << endl;*/
		
		i_was_disabled_on_flag = false;
	}
}
