/* ThemisStatusView.cpp */

// BeOS headers
#include <Box.h>

// C/C++ headers
#include <iostream>

// myheaders
#include "ThemisStatusView.h"
#include "win.h"

ThemisStatusView::ThemisStatusView(
	BRect rect,
	const rgb_color* arr )
	: BView(
		rect,
		"THEMISSTATUSVIEW",
		B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM,
		B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE )
{
	fBackgroundColor = arr[0];
	fWhiteColor = arr[3];
	fDarkGrayColor = arr[4];
	
	fStatusText.SetTo( "" );
	
	fDocBmp = new BBitmap( BRect( 0,0,6,9 ), B_RGB32 );
	memcpy( fDocBmp->Bits(), pbar_doc_icon, 280 );
	fImgBmp = new BBitmap( BRect( 0,0,6,9 ), B_RGB32 );
	memcpy( fImgBmp->Bits(), pbar_img_icon, 280 );
}

ThemisStatusView::~ThemisStatusView( void )
{
}

void
ThemisStatusView::AttachedToWindow( void )
{
	SetViewColor( fBackgroundColor );
	
	BRect rect = Bounds();
	
	// the image progress bar
	//rect.left = fDocumentBar->Frame().right + 16;
	rect.top += 2;
	rect.right -= 20;
	rect.left = rect.right - 60;
		
	fImageBar = new ThemisProgressBar(
		rect,
		"IMGPROGRESSBAR",
		B_FOLLOW_RIGHT | B_FOLLOW_TOP,
		B_WILL_DRAW,
		( ( Win* )Window() )->fColorArray );
	fImageBar->SetValue( 100, "" );
	AddChild( fImageBar );
	
	// the document progress bar
	rect.right = fImageBar->Frame().left - 16;
	rect.left = rect.right - 60;
		
	fDocumentBar = new ThemisProgressBar(
		rect,
		"DOCPROGRESSBAR",
		B_FOLLOW_RIGHT | B_FOLLOW_TOP,
		B_WILL_DRAW,
		( ( Win* )Window() )->fColorArray  );
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
	
	//cout << "ThemisStatusView::Draw()" << endl;
	rgb_color lo = LowColor();
	
	// the top 'shadow'
	SetLowColor( fDarkGrayColor );
	StrokeLine(
		BPoint( updaterect.left, updaterect.top ),
		BPoint( updaterect.right, updaterect.top ),
		B_SOLID_LOW );
	
	SetLowColor( 240, 240, 240, 255 );
	StrokeLine(
		BPoint( updaterect.left, updaterect.top + 1 ),
		BPoint( updaterect.right, updaterect.top + 1 ),
		B_SOLID_LOW );
	
	// fill the whole area
	SetLowColor( fBackgroundColor );
	FillRect(
		BRect(
			updaterect.left,
			updaterect.top + 2,
			updaterect.right,
			updaterect.bottom ),
		B_SOLID_LOW );
	
	// the left part
	
	TruncateString( &fStatusText, B_TRUNCATE_END, Bounds().right / 2 );
	SetFont( be_plain_font );
	SetFontSize( 10.0 );
	DrawString(
		fStatusText.String(),
		BPoint( updaterect.left + 5, updaterect.bottom - 2 ) );
	
	
	// the right part
	
	DrawBitmap( fDocBmp, BPoint( fDocumentBar->Frame().left - 10, updaterect.top + 3 ) );
	DrawBitmap( fImgBmp, BPoint( fImageBar->Frame().left - 10, updaterect.top + 3 ) );
	
	BPoint sep_pt_top( fDocumentBar->Frame().left - 15, Bounds().top + 2 );
	BPoint sep_pt_bot( fDocumentBar->Frame().left - 15, Bounds().bottom );
	
	// separator left to bars
	SetLowColor( fDarkGrayColor );
	StrokeLine( sep_pt_top, sep_pt_bot, B_SOLID_LOW );
	SetLowColor( fWhiteColor );
	sep_pt_top.x +=1;
	sep_pt_bot.x +=1;
	StrokeLine( sep_pt_top, sep_pt_bot, B_SOLID_LOW );
	
	// separator left to cookie icon
	sep_pt_top.x -=15;
	sep_pt_bot.x -=15;
	SetLowColor( fDarkGrayColor );
	StrokeLine( sep_pt_top, sep_pt_bot, B_SOLID_LOW );
	SetLowColor( fWhiteColor );
	sep_pt_top.x +=1;
	sep_pt_bot.x +=1;
	StrokeLine( sep_pt_top, sep_pt_bot, B_SOLID_LOW );
	
	// separator left to secure icon
	sep_pt_top.x -=15;
	sep_pt_bot.x -=15;
	SetLowColor( fDarkGrayColor );
	StrokeLine( sep_pt_top, sep_pt_bot, B_SOLID_LOW );
	SetLowColor( fWhiteColor );
	sep_pt_top.x +=1;
	sep_pt_bot.x +=1;
	StrokeLine( sep_pt_top, sep_pt_bot, B_SOLID_LOW );
	
	
	SetLowColor( lo );
}

void
ThemisStatusView::SetValues(
	int doc_progress,
	const char* doc_text,
	int img_progress,
	const char* img_text,
	const char* status_text )
{
	fDocumentBar->SetValue( doc_progress, doc_text );
	fImageBar->SetValue( img_progress, img_text );
	
	// needs to be changed later..
	// for now i was to lazy to change the statustext
	// to "" or "done" in the demo URL_LOADING, so we check here
	if( doc_progress < 100 || img_progress < 100 )
		fStatusText.SetTo( status_text );
	else
		fStatusText.SetTo( "" );
	
	Draw( Bounds() );
}


/////////////////////////////////////
// ThemisProgressBar
/////////////////////////////////////

ThemisProgressBar::ThemisProgressBar(
	BRect rect,
	const char* name,
	uint32 resizingmode,
	uint32 flags,
	const rgb_color* arr )
	: BView(
		rect,
		name,
		resizingmode,
		flags )
{
	fBackgroundColor = arr[0];
	fWhiteColor = arr[3];
	fDarkGrayColor = arr[4];
	fInterfaceColor = arr[5];
	
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

	SetFont( be_plain_font );
	SetFontSize( 10.0 );
	
	if( fProgress < 100 )
	{
		// draw the outer rectangle
		SetHighColor( fDarkGrayColor );
		StrokeRect( updaterect, B_SOLID_HIGH );
	
		if( fProgress > 0 )
		{
			// draw the progressbar
			BRect progrect( updaterect );
			progrect.InsetBy( 0, 1 );
			progrect.left += 1;
			progrect.right = fProgress * ( ( updaterect.right - 1 ) / 100 );
			
			SetHighColor( fInterfaceColor );
			FillRect( progrect, B_SOLID_HIGH );
			
			if( fProgress < 100 )
			{
				// fill the rest of the bar with white
				progrect.left = progrect.right;
				progrect.right = updaterect.right -1;
				SetHighColor( fWhiteColor );
				FillRect( progrect, B_SOLID_HIGH );
			}
		}
		else
		{
			BRect whiterect( updaterect );
			whiterect.InsetBy( 1, 1 );
			SetHighColor( fWhiteColor );
			FillRect( whiterect, B_SOLID_HIGH );
		}
	
		SetHighColor( 0, 0, 0, 255 );
	
		SetDrawingMode( B_OP_MIN );
		
		float strwidth = StringWidth( fBarText.String() );
		DrawString( fBarText.String(), BPoint(
			( updaterect.right / 2 ) - ( strwidth / 2 ),
			updaterect.bottom - 1 ) );
		
		SetDrawingMode( B_OP_OVER );
	}
	else	// progress == 100
	{
		// erase the whole area
		SetHighColor( fBackgroundColor );
		FillRect( updaterect, B_SOLID_HIGH );
		
		float strwidth = StringWidth( fBarText.String() );
		SetHighColor( 0, 0, 0, 255 );
		DrawString( fBarText.String(), BPoint(
			( updaterect.right / 2 ) - ( strwidth / 2 ),
			updaterect.bottom - 1 ) );
	}	
	
	SetHighColor( hi );
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
