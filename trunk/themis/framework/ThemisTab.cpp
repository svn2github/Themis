/* ThemisTab.cpp */

// BeOS headers
#include <GraphicsDefs.h>
#include <StringView.h>
#include <String.h>
#include <Bitmap.h>

// C/C++ headers
#include <iostream>
#include <stdio.h>
#include <string.h>

// myheaders
#include "ThemisTab.h"
#include "ThemisTabView.h"
#include "FakeSite.h"
#include "win.h"
#include "app.h"

ThemisTab::ThemisTab( BView *view, int16 uid )
	: BTab( view )
{
	fUniqueID = uid;
}

void
ThemisTab::DrawTab(
	ThemisTabView* owner,
	BRect frame,
	tab_position position,
	bool full )
{
	//cout << "ThemisTab::DrawTab()" << endl;
		
	rgb_color hi = owner->HighColor();
	rgb_color lo = owner->LowColor();
	
	// define the colors
	union int32torgb convert;
	AppSettings->FindInt32( "DarkBorderColor", &convert.value );
	rgb_color darkbordercolor = convert.rgb;
	AppSettings->FindInt32( "ActiveTabColor", &convert.value );
	rgb_color activetabcolor = convert.rgb;
	AppSettings->FindInt32( "InactiveTabColor", &convert.value );
	rgb_color inactivetabcolor = convert.rgb;
	AppSettings->FindInt32( "ShadowColor", &convert.value );
	rgb_color shadowcolor = convert.rgb;
					
	// Start a line array to draw the tab --
	// this is faster then drawing the lines
	// one by one.
	owner->BeginLineArray( 30 );
	
	// create the tab fillrect
	BRect fillrect = BRect( frame.left + 3, frame.top + 2,
			frame.right - 3, frame.bottom - 2 );
	
	// The bottom left corner and other stuff
	// ( visible only on the frontmost tab )
	if( position != B_TAB_ANY )
	{
		owner->AddLine( BPoint( frame.left, frame.bottom - 1 ),
			BPoint( frame.left + 1, frame.bottom - 2 ), darkbordercolor );
		owner->AddLine( BPoint( frame.left + 1, frame.bottom - 1 ),
			BPoint( frame.left + 2, frame.bottom - 2 ), shadowcolor );
		
		// draw the active tabs background
		owner->SetLowColor( activetabcolor );
		owner->FillRect( fillrect, B_SOLID_LOW );
		
		// the upper border of the 'thick' line between tabs and site content
		// which is not drawn in the active tab, but must be drawn in activetabcolor
		owner->AddLine( BPoint( frame.left + 2, frame.bottom - 1 ),
			BPoint( frame.right - 2, frame.bottom - 1), activetabcolor );
		// and the shadow of that line, which cant be seen in this tab, too
		owner->AddLine( BPoint( frame.left + 1, frame.bottom ),
			BPoint( frame.right, frame.bottom ), activetabcolor );
		// except the little pixel of the shadow at the left bottom corner
		owner->AddLine( BPoint( frame.left, frame.bottom ),
			BPoint( frame.left, frame.bottom ), shadowcolor );
	}
	else	// an inactive tab
	{
		// draw the inactive tabs background
		owner->SetLowColor( inactivetabcolor );
		owner->FillRect( fillrect, B_SOLID_LOW );
		
		// the black line 'under' the inactive tab
		owner->AddLine( BPoint( frame.left, frame.bottom - 1 ),
			BPoint( frame.right, frame.bottom - 1), darkbordercolor );
		// and the 'shadow'
		owner->AddLine( BPoint( frame.left , frame.bottom ),
			BPoint( frame.right , frame.bottom ), shadowcolor );
	}
		
	// Left wall -- always drawn
	owner->AddLine( BPoint( frame.left + 1, frame.bottom - 2 ),
		BPoint( frame.left + 1, frame.top + 2 ), darkbordercolor );
	owner->AddLine( BPoint( frame.left + 2, frame.bottom - 2 ),
		BPoint( frame.left + 2, frame.top + 2 ), shadowcolor );
	
	// Top-left corner -- always drawn
	owner->AddLine( BPoint( frame.left + 2, frame.top + 1 ),
		BPoint( frame.left + 2, frame.top + 1 ), darkbordercolor );
			
	// Top -- always drawn
	owner->AddLine( BPoint( frame.left + 3, frame.top ),
		BPoint( frame.right - 3, frame.top ), darkbordercolor );
	owner->AddLine( BPoint( frame.left + 3, frame.top + 1 ),
		BPoint( frame.right - 3, frame.top + 1 ), shadowcolor );	
	
	// Top-right corner -- always drawn
	owner->AddLine( BPoint( frame.right - 2, frame.top + 1 ),
		BPoint( frame.right - 2, frame.top + 1 ), darkbordercolor );
	
	// Right wall -- always drawn.
	owner->AddLine( BPoint( frame.right - 1, frame.top + 2 ),
		BPoint( frame.right - 1, frame.bottom - 2 ), darkbordercolor );
	owner->AddLine( BPoint( frame.right - 2, frame.top + 2 ),
		BPoint( frame.right - 2, frame.bottom - 2 ), shadowcolor );
	
	// Bottom-right corner, only visible if the tab
	// is either frontmost or the rightmost tab.
	if( full )
	{
		owner->AddLine( BPoint( frame.right - 1, frame.bottom - 2 ),
			BPoint( frame.right, frame.bottom - 1 ), darkbordercolor );
		owner->AddLine( BPoint( frame.right - 2, frame.bottom - 2 ),
			BPoint( frame.right - 1, frame.bottom - 1 ), shadowcolor );
	}
	
	owner->EndLineArray();
	
	// Draw the label and the fav-icon by calling DrawLabel()
	if( position != B_TAB_ANY )
	{
		AppSettings->FindInt32( "ActiveTabColor", &convert.value );
		float value = ( convert.rgb.red + convert.rgb.green + convert.rgb.blue ) / ( 3.0 * 255.0 );
		if( value > 0.5 )
			owner->SetHighColor( 0, 0, 0, 255 );
		else
			owner->SetHighColor( 255,255,255,255 );
	}
	else
	{
		AppSettings->FindInt32( "InactiveTabColor", &convert.value );
		float value = ( convert.rgb.red + convert.rgb.green + convert.rgb.blue ) / ( 3.0 * 255.0 );
		if( value > 0.5 )
			owner->SetHighColor( 0, 0, 0, 255 );
		else
			owner->SetHighColor( 255,255,255,255 );
	}
	
	DrawLabel( owner, frame );
	owner->SetHighColor( hi );
	owner->SetLowColor( lo );
} 

void
ThemisTab::DrawLabel( ThemisTabView* owner, BRect frame )
{
	// i am drawing both, the label and the icon here, as it makes
	// no sense to me doing nearly the same in separate functions
	
	//cout << "ThemisTab::DrawLabel()" << endl;
	BString* label = NULL;
	FakeSite* tempview = ( FakeSite* )View();
	
	// calculate truncate_length
	float truncate_length = (( ThemisTabView* )owner)->tab_width - 25 - 5;
		
	// the frame for the fav_icon
	BRect iconframe = frame;
	iconframe.left = frame.left + 5;
	iconframe.top = frame.top + 3;
	iconframe.right = iconframe.left + 15;
	iconframe.bottom = iconframe.top + 15;
	
	// the startpoint for the label
	BPoint labelpoint( frame.left + 25, frame.bottom - 6 );
		
	if( tempview == NULL )
	{
		//cout << "ThemisTab::DrawLabel() : no target view" << endl;
		// below 8 no string is drawn ( i use 5 here to be safe )
		if( truncate_length > 5 )
		{
			label = new BString( "(untitled)" );
			owner->TruncateString( label, B_TRUNCATE_END, truncate_length );
		
			owner->SetFontSize( 10.0 );
			owner->DrawString(
				label->String(),
				labelpoint,
				NULL );
		}
		
		// draw the icon
		owner->SetDrawingMode( B_OP_ALPHA );
		BBitmap* temp_fav_icon = new BBitmap( BRect( 0,0,15,15 ), B_RGB32 );
		memcpy( temp_fav_icon->Bits(), ( ( Win* )owner->Window() )->bitmaps[9]->Bits(), 1024 );
		owner->DrawBitmap( temp_fav_icon, iconframe );
		delete temp_fav_icon;
		owner->SetDrawingMode( B_OP_COPY );
	}
	else
	{
		//cout << "ThemisTab::DrawLabel() : we have a target view" << endl;
		// below 8 no string is drawn ( i use 5 here to be safe )
		if( truncate_length > 5 )
		{
			label = new BString( tempview->site_title.String() );
			owner->TruncateString( label, B_TRUNCATE_END, truncate_length );
		
			owner->SetFontSize( 10.0 );
			owner->DrawString(
				label->String(),
				labelpoint,
				NULL );
		}
		
		// draw the icon
		owner->SetDrawingMode( B_OP_ALPHA );
		owner->DrawBitmap( tempview->site_fav_icon, iconframe );
		owner->SetDrawingMode( B_OP_COPY );
	}
	delete label;
	tempview = NULL;
}

int16
ThemisTab::UniqueID()
{
	return fUniqueID;
}
