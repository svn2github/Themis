/* TabContentView.cpp */

// BeOS headers
#include <Box.h>
#include <Messenger.h>
#include <TranslationUtils.h>


// C/C++ headers
#include <iostream>
#include <string.h>
#include <iostream.h>

// myheaders
#include "appdefines.h"
#include "win.h"
#include "FakeSite.h"

FakeSite::FakeSite( BRect rect, const char* title, int16 uid, BWindow* win = NULL ) :
	BView( rect, "fakesite", B_FOLLOW_ALL, B_WILL_DRAW | B_NAVIGABLE )
{
	// the fav_site_icon
	// ( of course this should be reimplemented later with the real fav icons )
	site_fav_icon = new BBitmap( BRect( 0,0,15,15 ), B_RGB32 );
	
	mainwin = win;
	
	fUniqueID = uid;
	
	if( mainwin != NULL )
		// load the icon from mainwindow
		memcpy( site_fav_icon->Bits(), 
			( ( Win* )win )->bitmaps[8]->Bits(), 1024 );
	// well, why do we need a pointer to mainwindow? cant this icon loading
	// all be done in AttachedToWindow()??? well no! :D
	// if we set the icon later in AttachedToWindow() we are one tick behind.
	// means: the urlview-fav-icon wouldnt be displayed when calling
	// ThemisTabView::Select() the first time( on URL_OPEN )! If we click the
	// tab again, everything is fine.
	// the reason for this is, that the fakesite gets attached to the window
	// _after_ the call to ThemisTabView::Select() and then we wouldnt have
	// a valid fav_icon ( just white ) in ThemisTabView::Select() ... 
	
	// has to be removed later
	// ( like this whole class :D )
	if( strlen( title ) == 0 )
		site_title.SetTo( "no.url.given" );
	else
		site_title.SetTo( title );
		
	// info init
	fStatusText.SetTo( "" );
	fSecure = false;
	fCookiesDisabled = false;
	fDocProgress = 0;
	fDocText = "";
	fImgProgress = 0;
	fImgText = "";
}

FakeSite::~FakeSite( void )
{
}

void
FakeSite::AttachedToWindow( void )
{
	// just to have a visual border :D
	SetViewColor( 220,230,220,0 );
			
	BView::AttachedToWindow();
}

void
FakeSite::Draw( BRect updaterect )
{
	//cout << "FakeSite::Draw()" << endl;
	
	BString text( "this is the fakesite for " );
	text << "\"" << site_title.String() << "\"";
			
	SetFont( be_bold_font );
	DrawString( text.String(), BPoint( 20,20 ), NULL );
	
	text.RemoveAll( text );
	text << "this whole site is a link :D -> middle-mouse-button opens new tab ( hidden ) with \"link\"";
	DrawString( text.String(), BPoint( 20,40 ), NULL );
	
	if( IsFocus() == true )
	{
		text.RemoveAll( text );
		text << "view has focus";
		DrawString( text.String(), BPoint( 20,60 ), NULL );
	}
	
	BView::Draw( updaterect );
}

void
FakeSite::MouseDown( BPoint point )
{
	//cout << "FakeSite::MouseDown()" << endl;
	
	// at first check, if the urlpopupwindow is still open
	// if yes, close it and return.
	Win* win = ( Win* )mainwin;
	if( win->urlpopupwindow != NULL )
	{
		win->urlpopupwindow->Lock();
		win->urlpopupwindow->Quit();
		win->urlpopupwindow = 0;
		return;
	}
		
	uint32 buttons;
	GetMouse( &point, &buttons, true );
	
	BMessenger messagetarget( NULL, Looper() );
	
	switch( buttons )
	{
		case B_PRIMARY_MOUSE_BUTTON :
		{
			cout << "FakeSite::MouseDown(): B_PRIMARY_MOUSE_BUTTON" << endl;
						
			break;
		}
		case B_SECONDARY_MOUSE_BUTTON :
		{
			cout << "FakeSite::MouseDown(): B_SECONDARY_MOUSE_BUTTON" << endl;
											
			break;
		}
		case B_TERTIARY_MOUSE_BUTTON :
		{
			cout << "FakeSite::MouseDown(): B_TERTIARY_MOUSE_BUTTON" << endl;
			
			BMessage* newtabmsg = new BMessage( TAB_ADD );
			newtabmsg->AddString( "url_to_open", "the.middle.click.url" );
			
			messagetarget.SendMessage( newtabmsg );
						
			break;
		}
		default :
			break;
	}
	
}

bool
FakeSite::GetCookieState()
{
	return fCookiesDisabled;
}

int
FakeSite::GetDocBarProgress()
{
	return fDocProgress;
}

const char*
FakeSite::GetDocBarText()
{
	return fDocText;
}

int
FakeSite::GetImgBarProgress()
{
	return fImgProgress;
}

const char*
FakeSite::GetImgBarText()
{
	return fImgText;
}

bool
FakeSite::GetSecureState()
{
	return fSecure;
}

const char*
FakeSite::GetStatusText()
{
	return fStatusText.String();
}

void
FakeSite::SetInfo(
	int doc_progress,
	bool delta_doc,
	const char* doc_text,
	int img_progress,
	bool delta_img,
	const char* img_text,
	const char* statustext,
	bool sec,
	bool cook_dis )
{
	cout << "FakeSite::SetInfo()" << endl;
	
	if( delta_doc == true )
	{
		fDocProgress += doc_progress;
		if( fDocProgress >= 100 )
			fDocProgress = 100;
	}
	else
		fDocProgress = doc_progress;
	
	fDocText = ( char* )doc_text;

	if( delta_img == true )
	{
		fImgProgress += img_progress;
		if( fImgProgress >= 100 )
			fImgProgress = 100;
	}
	else
		fImgProgress = img_progress;
	
	fImgText = ( char* )img_text;
		
	fStatusText.SetTo( statustext );
	fSecure = sec;
	fCookiesDisabled = cook_dis;
}

int16
FakeSite::UniqueID()
{
	return fUniqueID;
}
