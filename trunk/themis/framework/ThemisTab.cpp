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

ThemisTab::ThemisTab( BView *view, uint uid )
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
			
	// Start a line array to draw the tab --
	// this is faster then drawing the lines
	// one by one.
	owner->BeginLineArray( 20 );
	
	// create the tab fillrect
	BRect fillrect = BRect( frame.left + 3, frame.top + 2,
			frame.right - 3, frame.bottom - 1 );
	
	// The bottom left corner
	// ( visible only on the frontmost tab )
	if( position != B_TAB_ANY )
	{
		owner->AddLine( BPoint( frame.left, frame.bottom ),
			BPoint( frame.left + 1, frame.bottom - 1 ), owner->fBlackColor );
		owner->AddLine( BPoint( frame.left + 1, frame.bottom ),
			BPoint( frame.left + 2, frame.bottom - 1 ), owner->fWhiteColor );
	}
	else	// an inactive tab
	{
		// the black line 'under' the inactive tab
		owner->AddLine( BPoint( frame.left, frame.bottom ),
			BPoint( frame.right, frame.bottom ), owner->fBlackColor );
		
		// draw the inactive tabs background darker
		owner->SetHighColor( owner->fInactiveTabColor );
		owner->FillRect( fillrect, B_SOLID_HIGH );
	}
		
	// Left wall -- always drawn
	owner->AddLine( BPoint( frame.left + 1, frame.bottom - 1 ),
		BPoint( frame.left + 1, frame.top + 2 ), owner->fBlackColor );
	owner->AddLine( BPoint( frame.left + 2, frame.bottom - 1 ),
		BPoint( frame.left + 2, frame.top + 2 ), owner->fWhiteColor );
	
	// Top-left corner -- always drawn
	owner->AddLine( BPoint( frame.left + 2, frame.top + 1 ),
		BPoint( frame.left + 2, frame.top + 1 ), owner->fBlackColor );
			
	// Top -- always drawn
	owner->AddLine( BPoint( frame.left + 3, frame.top ),
		BPoint( frame.right - 3, frame.top ), owner->fBlackColor );
	owner->AddLine( BPoint( frame.left + 3, frame.top + 1 ),
		BPoint( frame.right - 3, frame.top + 1 ), owner->fWhiteColor );	
	
	// Top-right corner -- always drawn
	owner->AddLine( BPoint( frame.right - 2, frame.top + 1 ),
		BPoint( frame.right - 2, frame.top + 1 ), owner->fBlackColor );
	
	// Right wall -- always drawn.
	owner->AddLine( BPoint( frame.right - 1, frame.top + 2 ),
		BPoint( frame.right - 1, frame.bottom - 1 ), owner->fBlackColor );
	owner->AddLine( BPoint( frame.right - 2, frame.top + 2 ),
		BPoint( frame.right - 2, frame.bottom - 1 ), owner->fWhiteColor );
	
	// Bottom-right corner, only visible if the tab
	// is either frontmost or the rightmost tab.
	if( full )
	{
		owner->AddLine( BPoint( frame.right - 1, frame.bottom - 1 ),
			BPoint( frame.right, frame.bottom ), owner->fBlackColor );
		owner->AddLine( BPoint( frame.right - 2, frame.bottom - 1 ),
			BPoint( frame.right - 1, frame.bottom ), owner->fWhiteColor );
	}
	
	owner->EndLineArray();
	
	// Draw the label and the fav-icon by calling DrawLabel()
	owner->SetHighColor( 0, 0, 0, 255 );
	DrawLabel( owner, frame );
	owner->SetHighColor( hi );
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
	iconframe.top = frame.top + 2;
	iconframe.right = iconframe.left + 15;
	iconframe.bottom = iconframe.top + 15;
		
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
				BPoint( frame.left + 25, frame.bottom - 5 ),
				NULL );
		}
		
		// draw the icon
		owner->SetDrawingMode( B_OP_ALPHA );
		BBitmap* temp_fav_icon = new BBitmap( BRect( 0,0,15,15 ), B_RGB32 );
		memcpy( temp_fav_icon->Bits(), ( ( Win* )owner->Window() )->bitmaps[9]->Bits(), 1024 );
		owner->DrawBitmap( temp_fav_icon, iconframe );
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
				BPoint( frame.left + 25, frame.bottom - 5 ),
				NULL );
		}
		
		// draw the icon
		owner->SetDrawingMode( B_OP_ALPHA );
		owner->DrawBitmap( tempview->site_fav_icon, iconframe );
		owner->SetDrawingMode( B_OP_COPY );
	}
}

uint
ThemisTab::UniqueID()
{
	return fUniqueID;
}
