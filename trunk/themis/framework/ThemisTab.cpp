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
#include "ThemisIcons.h"

const rgb_color kWhite = {255,255,255,255}; 
const rgb_color kLightGray = {240,240,240,255};
const rgb_color kGray = {221,221,221,255}; 
const rgb_color kBlackColor = {51,51,51,255};

ThemisTab::ThemisTab( BView *view )
	: BTab( view )
{
}

void
ThemisTab::DrawTab( BView* owner, BRect frame,
	tab_position position, bool full )
{
	//cout << "ThemisTab::DrawTab()" << endl;
		
	rgb_color hi;
	rgb_color lo;
	
	// Save the original colors
	hi = owner->HighColor();
	lo = owner->LowColor();
		
	// Start a line array to draw the tab --
	// this is faster than drawing the lines
	// one by one.
	owner->BeginLineArray( 20 );
	
	// create the tab fillrect
	BRect fillrect = BRect( frame.left + 3, frame.top + 2,
			frame.right - 3, frame.bottom - 1 );
	
	// Do the bottom left corner, visible
	// only on the frontmost tab.
	if( position != B_TAB_ANY )
	{
		owner->AddLine( BPoint( frame.left, frame.bottom ),
			BPoint( frame.left + 1, frame.bottom - 1 ), kBlackColor );
		owner->AddLine( BPoint( frame.left + 1, frame.bottom ),
			BPoint( frame.left + 2, frame.bottom - 1 ), kWhite );
		owner->AddLine( BPoint( frame.left + 2, frame.bottom ),
			BPoint( frame.right - 2, frame.bottom ), kLightGray );
		
		// draw us lighter
		owner->SetLowColor( kLightGray );
		owner->FillRect( fillrect, B_SOLID_LOW );
		owner->SetLowColor( lo );
	}
	else
	{
		owner->AddLine( BPoint( frame.left, frame.bottom ),
			BPoint( frame.right, frame.bottom ), kBlackColor );
		
		// draw us darker
		owner->SetLowColor( kGray );
		owner->FillRect( fillrect, B_SOLID_LOW );
		owner->SetLowColor( lo );
	}
		
	// Left wall -- always drawn
	owner->AddLine( BPoint( frame.left + 1, frame.bottom - 1 ),
		BPoint( frame.left + 1, frame.top + 2 ), kBlackColor );
	owner->AddLine( BPoint( frame.left + 2, frame.bottom - 1 ),
		BPoint( frame.left + 2, frame.top + 2 ), kWhite );
	
	// Top-left corner -- always drawn
	owner->AddLine( BPoint( frame.left + 2, frame.top + 1 ),
		BPoint( frame.left + 2, frame.top + 1 ), kBlackColor );
			
	// Top -- always drawn
	owner->AddLine( BPoint( frame.left + 3, frame.top ),
		BPoint( frame.right - 3, frame.top ), kBlackColor );
	owner->AddLine( BPoint( frame.left + 3, frame.top + 1 ),
		BPoint( frame.right - 3, frame.top + 1 ), kWhite );	
	
	// Top-right corner -- always drawn
	owner->AddLine( BPoint( frame.right - 2, frame.top + 1 ),
		BPoint( frame.right - 2, frame.top + 1 ), kBlackColor );
	
	// Right wall -- always drawn.
	owner->AddLine( BPoint( frame.right - 1, frame.top + 2 ),
		BPoint( frame.right - 1, frame.bottom - 1 ), kBlackColor );
	owner->AddLine( BPoint( frame.right - 2, frame.top + 2 ),
		BPoint( frame.right - 2, frame.bottom - 1 ), kWhite );
	
	// Bottom-right corner, only visible if the tab
	// is either frontmost or the rightmost tab.
	if( full )
	{
		owner->AddLine( BPoint( frame.right - 1, frame.bottom - 1 ),
			BPoint( frame.right, frame.bottom ), kBlackColor );
		owner->AddLine( BPoint( frame.right - 2, frame.bottom - 1 ),
			BPoint( frame.right - 1, frame.bottom ), kWhite );
	}
	
	// Draw the label by calling DrawLabel()
	owner->SetHighColor( 0,0,0,255 );
	owner->SetLowColor( kGray );
	DrawLabel( owner, frame );
		
	owner->EndLineArray();
	
	owner->SetHighColor( hi );
	owner->SetLowColor( lo );
} 

void
ThemisTab::DrawLabel( BView* owner, BRect frame )
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
		memcpy( temp_fav_icon->Bits(), icon_document_empty_hex, 1024 );
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
