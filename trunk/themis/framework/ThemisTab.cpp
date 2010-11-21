/* ThemisTab.cpp */

// Standard C/C++ headers
#include <stdio.h>

// Standard C++ headers
#include <iostream>
#include <string>

// BeOS headers
#include <GraphicsDefs.h>
#include <StringView.h>
#include <String.h>
#include <Bitmap.h>
#include <Message.h>

// Themis headers
#include "PrefsDefs.h"
#include "commondefs.h"
#include "ThemisIcons.h"
#include "ThemisTabView.h"
#include "ThemisTab.h"

ThemisTab :: ThemisTab(BView* view)
		  : BTab(view) {

	fSiteID = 0;
	
	fLabel = new BString( "(untitled)" );
		
	int8 histdepth;
	AppSettings->FindInt8( "TabHistoryDepth", &histdepth );
	fHistory = new TabHistory( histdepth );
	
	fFavIcon = new BBitmap( BRect( 0, 0, 15, 15 ), B_RGB32 );
	memcpy( fFavIcon->Bits(), icon_document_empty_hex, 1024 );
}

ThemisTab :: ~ThemisTab() {
	delete fLabel;
	delete fHistory;
	delete fFavIcon;
}

void ThemisTab :: DrawTab(ThemisTabView* owner,
						  BRect frame,
						  tab_position position,
						  bool full) {
	//cout << "ThemisTab::DrawTab()" << endl;
		
	rgb_color hi = owner->HighColor();
	rgb_color lo = owner->LowColor();
	
	// define the colors
	rgb_color active_tab_color = ui_color( B_PANEL_BACKGROUND_COLOR );
	
					
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
			BPoint( frame.left + 1, frame.bottom - 2 ), kColorDarkBorder );
		owner->AddLine( BPoint( frame.left + 1, frame.bottom - 1 ),
			BPoint( frame.left + 2, frame.bottom - 2 ), kColorShadow );
		
		// draw the active tabs background
		owner->SetLowColor( active_tab_color );
		owner->FillRect( fillrect, B_SOLID_LOW );
		
		// the upper border of the 'thick' line between tabs and site content
		// which is not drawn in the active tab, but must be drawn in activetabcolor
		owner->AddLine( BPoint( frame.left + 2, frame.bottom - 1 ),
			BPoint( frame.right - 2, frame.bottom - 1), active_tab_color );
		// and the shadow of that line, which cant be seen in this tab, too
		owner->AddLine( BPoint( frame.left + 1, frame.bottom ),
			BPoint( frame.right, frame.bottom ), active_tab_color );
		// except the little pixel of the shadow at the left bottom corner
		owner->AddLine( BPoint( frame.left, frame.bottom ),
			BPoint( frame.left, frame.bottom ), kColorShadow );
	}
	else	// an inactive tab
	{
		// draw the inactive tabs background
		owner->SetLowColor( kColorInactiveTab );
		owner->FillRect( fillrect, B_SOLID_LOW );
		
		// the black line 'under' the inactive tab
		owner->AddLine( BPoint( frame.left, frame.bottom - 1 ),
			BPoint( frame.right, frame.bottom - 1), kColorDarkBorder );
		// and the 'shadow'
		owner->AddLine( BPoint( frame.left , frame.bottom ),
			BPoint( frame.right , frame.bottom ), kColorShadow );
	}
		
	// Left wall -- always drawn
	owner->AddLine( BPoint( frame.left + 1, frame.bottom - 2 ),
		BPoint( frame.left + 1, frame.top + 2 ), kColorDarkBorder );
	owner->AddLine( BPoint( frame.left + 2, frame.bottom - 2 ),
		BPoint( frame.left + 2, frame.top + 2 ), kColorShadow );
	
	// Top-left corner -- always drawn
	owner->AddLine( BPoint( frame.left + 2, frame.top + 1 ),
		BPoint( frame.left + 2, frame.top + 1 ), kColorDarkBorder );
			
	// Top -- always drawn
	owner->AddLine( BPoint( frame.left + 3, frame.top ),
		BPoint( frame.right - 3, frame.top ), kColorDarkBorder );
	owner->AddLine( BPoint( frame.left + 3, frame.top + 1 ),
		BPoint( frame.right - 3, frame.top + 1 ), kColorShadow );	
	
	// Top-right corner -- always drawn
	owner->AddLine( BPoint( frame.right - 2, frame.top + 1 ),
		BPoint( frame.right - 2, frame.top + 1 ), kColorDarkBorder );
	
	// Right wall -- always drawn.
	owner->AddLine( BPoint( frame.right - 1, frame.top + 2 ),
		BPoint( frame.right - 1, frame.bottom - 2 ), kColorDarkBorder );
	owner->AddLine( BPoint( frame.right - 2, frame.top + 2 ),
		BPoint( frame.right - 2, frame.bottom - 2 ), kColorShadow );
	
	// Bottom-right corner, only visible if the tab
	// is either frontmost or the rightmost tab.
	if( full )
	{
		owner->AddLine( BPoint( frame.right - 1, frame.bottom - 2 ),
			BPoint( frame.right, frame.bottom - 1 ), kColorDarkBorder );
		owner->AddLine( BPoint( frame.right - 2, frame.bottom - 2 ),
			BPoint( frame.right - 1, frame.bottom - 1 ), kColorShadow );
	}
	
	owner->EndLineArray();
	
	// Draw the label and the fav-icon by calling DrawLabel()
//	if( position != B_TAB_ANY )
//	{
//		AppSettings->FindInt32( "ActiveTabColor", &convert.value );
//		float value = ( convert.rgb.red + convert.rgb.green + convert.rgb.blue ) / ( 3.0 * 255.0 );
//		if( value > 0.5 )
//			owner->SetHighColor( 0, 0, 0, 255 );
//		else
//			owner->SetHighColor( 255,255,255,255 );
//	}
//	else
//	{
//		AppSettings->FindInt32( "InactiveTabColor", &convert.value );
//		float value = ( convert.rgb.red + convert.rgb.green + convert.rgb.blue ) / ( 3.0 * 255.0 );
//		if( value > 0.5 )
//			owner->SetHighColor( 0, 0, 0, 255 );
//		else
//			owner->SetHighColor( 255,255,255,255 );
//	}

	owner->SetHighColor( 0, 0, 0, 255 );
	
	DrawLabel( owner, frame );
	owner->SetHighColor( hi );
	owner->SetLowColor( lo );
} 

void ThemisTab :: DrawLabel(ThemisTabView* owner, BRect frame) {
	//cout << "ThemisTab::DrawLabel()" << endl;

	/* calculate truncate_length */
	float truncate_length = (( ThemisTabView* )owner)->tab_width - 25 - 5;
		
	/* the frame for the FavIcon */
	BRect iconframe = frame;
	iconframe.left = frame.left + 5;
	iconframe.top = frame.top + 3;
	iconframe.right = iconframe.left + 15;
	iconframe.bottom = iconframe.top + 15;
	
	/* the startpoint for the label */
	BPoint labelpoint( frame.left + 25, frame.bottom - 6 );
	
	/* Draw the label */
	owner->TruncateString( fLabel, B_TRUNCATE_END, truncate_length );
	owner->DrawString(
		fLabel->String(),
		labelpoint,
		NULL );

	/* Draw the FavIcon */
	owner->SetDrawingMode( B_OP_ALPHA );
	owner->DrawBitmap( fFavIcon, iconframe );
	owner->SetDrawingMode( B_OP_COPY );	

}

BBitmap * ThemisTab :: GetFavIcon() {

	return fFavIcon;
}

TabHistory * ThemisTab :: GetHistory() {

	return fHistory;
}

int32 ThemisTab :: GetSiteID() {

	return fSiteID;
}

void ThemisTab :: SetFavIcon(BBitmap* bmp) {

	if (bmp) {
		memcpy(fFavIcon->Bits(), bmp->Bits(), 1024);
	}
}

void ThemisTab :: SetLabel(const char* label) {

	fLabel->SetTo( label );
}

void ThemisTab :: SetSiteID(int32 id) {

	fSiteID = id;
}
