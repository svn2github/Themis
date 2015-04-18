/* ThemisStatusView.cpp */


// Standard C++ headers
#include <iostream>

// BeOS headers
#include <Box.h>

// Themis headers
#include "commondefs.h"
#include "ThemisStatusView.h"

ThemisStatusView::ThemisStatusView(
	BRect rect )
	: BView(
		rect,
		"THEMISSTATUSVIEW",
		B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM,
		B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE )
{
	fStatusText.SetTo( "" );
	fSecure = false;
	fCookiesDisabled = false;
	
	fDocBmp = new BBitmap( BRect( 0,0,6,9 ), B_RGB32 );
	memcpy( fDocBmp->Bits(), pbar_doc_icon, 280 );
	fImgBmp = new BBitmap( BRect( 0,0,6,9 ), B_RGB32 );
	memcpy( fImgBmp->Bits(), pbar_img_icon, 280 );
	fSecureBmp = new BBitmap( BRect( 0,0,6,9 ), B_RGB32 );
	memcpy( fSecureBmp->Bits(), pbar_secure_icon, 280 );
	fInsecureBmp = new BBitmap( BRect( 0,0,6,9 ), B_RGB32 );
	memcpy( fInsecureBmp->Bits(), pbar_insecure_icon, 280 );
	fCookieBmp = new BBitmap( BRect( 0,0,6,9 ), B_RGB32 );
	memcpy( fCookieBmp->Bits(), pbar_cookie_icon, 280 );
	fCookieDisabledBmp = new BBitmap( BRect( 0,0,6,9 ), B_RGB32 );
	memcpy( fCookieDisabledBmp->Bits(), pbar_cookie_disabled_icon, 280 );
}

ThemisStatusView::~ThemisStatusView( void )
{
	delete fDocBmp;
	delete fImgBmp;
	delete fSecureBmp;
	delete fInsecureBmp;
	delete fCookieBmp;
	delete fCookieDisabledBmp;
}

void
ThemisStatusView::AttachedToWindow( void )
{
	SetViewColor(B_TRANSPARENT_COLOR);
	
	BRect rect = Bounds();
	
	// the image progress bar
	rect.top += 2;
	rect.right -= 20;
	rect.left = rect.right - 60;
		
	fImageBar = new ThemisProgressBar(
		rect,
		"IMGPROGRESSBAR",
		B_FOLLOW_RIGHT | B_FOLLOW_TOP,
		B_WILL_DRAW );
	fImageBar->SetValue( 100, "" );
	AddChild( fImageBar );
	
	// the document progress bar
	rect.right = fImageBar->Frame().left - 16;
	rect.left = rect.right - 60;
		
	fDocumentBar = new ThemisProgressBar(
		rect,
		"DOCPROGRESSBAR",
		B_FOLLOW_RIGHT | B_FOLLOW_TOP,
		B_WILL_DRAW );
	fDocumentBar->SetValue( 100, "" );
	AddChild( fDocumentBar );
		
	BView::AttachedToWindow();
}

void
ThemisStatusView::Draw( BRect updaterect )
{
	// compromise for now
	// make the updaterect the Bounds rectangle
	updaterect = Bounds();
	
	rgb_color lo = LowColor();
	
	rgb_color panel_color = ui_color( B_PANEL_BACKGROUND_COLOR );
	
	// the top 'shadow'
	SetLowColor( 169,169,169,255 );
	StrokeLine(
		BPoint( updaterect.left, updaterect.top ),
		BPoint( updaterect.right, updaterect.top ),
		B_SOLID_LOW );
	
	SetLowColor( kColorShadow );
	StrokeLine(
		BPoint( updaterect.left, updaterect.top + 1 ),
		BPoint( updaterect.right, updaterect.top + 1 ),
		B_SOLID_LOW );
	
	// fill the whole area
	SetLowColor( panel_color );
	FillRect(
		BRect(
			updaterect.left,
			updaterect.top + 2,
			updaterect.right,
			updaterect.bottom ),
		B_SOLID_LOW );
	
	// the right part
	
	DrawBitmap( fDocBmp, BPoint( fDocumentBar->Frame().left - 10, updaterect.top + 3 ) );
	DrawBitmap( fImgBmp, BPoint( fImageBar->Frame().left - 10, updaterect.top + 3 ) );
	
	if( fCookiesDisabled == false )
		DrawBitmap( fCookieBmp, BPoint( fDocumentBar->Frame().left - 25, updaterect.top + 3 ) );
	else
		DrawBitmap( fCookieDisabledBmp, BPoint( fDocumentBar->Frame().left - 25, updaterect.top + 3 ) );
	
	if( fSecure == false )
		DrawBitmap( fInsecureBmp, BPoint( fDocumentBar->Frame().left - 39, updaterect.top + 3 ) );
	else
		DrawBitmap( fSecureBmp, BPoint( fDocumentBar->Frame().left - 39, updaterect.top + 3 ) );
	
	BPoint sep_pt_top( fDocumentBar->Frame().left - 15, Bounds().top + 2 );
	BPoint sep_pt_bot( fDocumentBar->Frame().left - 15, Bounds().bottom - 1 );
	
	// separator left to bars
	SetLowColor( kColorLightBorder );
	StrokeLine( sep_pt_top, sep_pt_bot, B_SOLID_LOW );
	SetLowColor( kColorShadow );
	sep_pt_top.x +=1;
	sep_pt_bot.x +=1;
	StrokeLine( sep_pt_top, sep_pt_bot, B_SOLID_LOW );
	
	// separator left to cookie icon
	sep_pt_top.x -=15;
	sep_pt_bot.x -=15;
	SetLowColor( kColorLightBorder );
	StrokeLine( sep_pt_top, sep_pt_bot, B_SOLID_LOW );
	SetLowColor( kColorShadow );
	sep_pt_top.x +=1;
	sep_pt_bot.x +=1;
	StrokeLine( sep_pt_top, sep_pt_bot, B_SOLID_LOW );
	
	// separator left to secure icon
	sep_pt_top.x -=15;
	sep_pt_bot.x -=15;
	SetLowColor( kColorLightBorder );
	StrokeLine( sep_pt_top, sep_pt_bot, B_SOLID_LOW );
	SetLowColor( kColorShadow );
	sep_pt_top.x +=1;
	sep_pt_bot.x +=1;
	StrokeLine( sep_pt_top, sep_pt_bot, B_SOLID_LOW );
	
	// the left part
	
	TruncateString( &fStatusText, B_TRUNCATE_END, sep_pt_top.x - 10 );
	SetFont( be_plain_font );
	SetFontSize( 10.0 );
	DrawString(
		fStatusText.String(),
		BPoint( updaterect.left + 5, updaterect.bottom - 2 ) );
	
	// redraw everything else	
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
ThemisStatusView::SetLoadingInfo(
	int doc_progress,
	const char* status_text )
{
	fDocumentBar->SetValue( doc_progress, "" );
	if (fStatusText.Compare(status_text)) {
		fStatusText.SetTo(status_text);
		Draw(Bounds());
	}
}

void
ThemisStatusView::SetValues(
	int doc_progress,
	const char* doc_text,
	int img_progress,
	const char* img_text,
	const char* status_text,
	bool sec,
	bool cook_dis )
{
	//cout << "ThemisStatusView::SetValues()" << endl;
	
	fDocumentBar->SetValue( doc_progress, doc_text );
	fImageBar->SetValue( img_progress, img_text );
	fStatusText.SetTo( status_text );
	fSecure = sec;
	fCookiesDisabled = cook_dis;
	
	Draw( Bounds() );
}


/////////////////////////////////////
// ThemisProgressBar
/////////////////////////////////////

ThemisProgressBar::ThemisProgressBar(
	BRect rect,
	const char* name,
	uint32 resizingmode,
	uint32 flags )
	: BView(
		rect,
		name,
		resizingmode,
		flags )
{
	fProgress = 0;
	fBarText.SetTo( "" );
}

void
ThemisProgressBar::Draw( BRect updaterect )
{
	//cout << "ThemisProgressBar::Draw()" << endl;
	
	// compromise for now
	// make the updaterect the Bounds rectangle
	updaterect = Bounds();
	
	rgb_color hi = HighColor();
	rgb_color lo = LowColor();

	SetFont( be_plain_font );
	SetFontSize( 10.0 );
	
	// define the colors
	rgb_color panel_color = ui_color( B_PANEL_BACKGROUND_COLOR );
	
	// draw the outer rectangle
	SetHighColor( kColorLightBorder );
	StrokeRect( updaterect, B_SOLID_HIGH );
	
	if( fProgress > 0 )
	{
		// draw the progressbar
		BRect progrect( updaterect );
		progrect.InsetBy( 0, 1 );
		progrect.left += 1;
		progrect.right = fProgress * ( ( updaterect.right - 1 ) / 100 );
		
		if( fProgress < 100 )
		{
			SetLowColor( kColorTheme );
			FillRect( progrect, B_SOLID_LOW );
			// fill the rest of the bar with white
			progrect.left = progrect.right;
			progrect.right = updaterect.right -1;
			SetHighColor( 255,255,255,255 );
			FillRect( progrect, B_SOLID_HIGH );
		}
		else	// fProgress == 100
		{
			SetLowColor( panel_color );
			BRect fillrect( updaterect );
			fillrect.InsetBy( 1, 1 );
			FillRect( fillrect, B_SOLID_LOW );
		}
	}
	else	// fProgress == 0
	{
		BRect whiterect( updaterect );
		whiterect.InsetBy( 1, 1 );
		SetLowColor( 255,255,255,255 );
		FillRect( whiterect, B_SOLID_LOW );
	}
			
	if( fBarText.Length() > 0 )
	{
//		float value = ( kColorTheme.red + kColorTheme.green + kColorTheme.blue ) / ( 3.0 * 255.0 );
//		if( value > 0.5 )
//			SetHighColor( 0, 0, 0, 255 );
//		else
//			SetHighColor( 255,255,255,255 );
		
		SetHighColor( 0, 0, 0, 255 );
		
		float strwidth = StringWidth( fBarText.String() );
		DrawString( fBarText.String(), BPoint(
			( updaterect.right / 2 ) - ( strwidth / 2 ),
			updaterect.bottom - 1 ) );
	}
		
	SetHighColor( hi );
	SetLowColor( lo );
}

void
ThemisProgressBar::Reset()
{
	fProgress = 0;
	fBarText = "";
	
	Draw( Bounds() );
}

void
ThemisProgressBar::SetValue( int value, const char* bartext )
{
	fProgress = value;
	fBarText.SetTo( bartext );
	
	Draw( Bounds() );
}

void
ThemisProgressBar :: AttachedToWindow() {
	
	SetViewColor(B_TRANSPARENT_COLOR);
}
