/* ThemisStatusView.cpp */

// BeOS headers
#include <Box.h>

// C/C++ headers
#include <iostream>

// myheaders
//#include "ThemisTVS.h"
#include "ThemisStatusView.h"

ThemisStatusView::ThemisStatusView( BRect rect ) :
	BView( rect, "THEMISSTATUSVIEW", B_FOLLOW_BOTTOM | B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW )
{
}

ThemisStatusView::~ThemisStatusView( void )
{
}

void
ThemisStatusView::AttachedToWindow( void )
{
	SetViewColor( 240,240,240,0 );
	
	BRect rect = Bounds();
	
	rect.top += 2;
	rect.bottom += 1;
	statustextview = new BStringView( rect, "STATUSTEXTVIEW", "statustext" );
	AddChild( statustextview );
	
	BView::AttachedToWindow();
}

void
ThemisStatusView::Draw( BRect updaterect )
{
	//cout << "ThemisStatusView::Draw()" << endl;
	rgb_color lo = LowColor();
	
	// due to optical reasons i couldnt use the same colors as in
	// the bottom of navview
	
	SetLowColor( 100,100,100,0 );
	StrokeLine(
		BPoint( updaterect.left, updaterect.top ),
		BPoint( updaterect.right, updaterect.top ),
		B_SOLID_LOW );
	
	SetLowColor( 255,255,255,0 );
	StrokeLine(
		BPoint( updaterect.left, updaterect.top + 1 ),
		BPoint( updaterect.right, updaterect.top + 1 ),
		B_SOLID_LOW );
	
	SetLowColor( 240,240,240,0 );
	FillRect(
		BRect(
			updaterect.left,
			updaterect.top + 2,
			updaterect.right,
			updaterect.bottom ),
		B_SOLID_LOW );
	
	SetLowColor( lo );
	
	BView::Draw( updaterect );
}
