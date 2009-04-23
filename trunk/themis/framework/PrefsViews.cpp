/*
 * PrefsViews.cpp
 *
 * author: Michael Weirauch
 */

#include <interface/Button.h>
#include <interface/CheckBox.h>
#include <interface/MenuField.h>
#include <interface/MenuItem.h>
#include <interface/PopUpMenu.h>
#include <interface/RadioButton.h>
#include <interface/TextControl.h>
#include <interface/Window.h>
#include <storage/Directory.h>
#include <storage/Entry.h>
#include <storage/Path.h>
#include <support/String.h>

#include <stdio.h>

#include "commondefs.h"
#include "PrefsDefs.h"
#include "PrefsViews.h"


/*
 * BasePrefsView
 */


BasePrefsView::BasePrefsView(
	BRect frame,
	const char* name )
	: BView(
		frame,
		name,
		B_FOLLOW_ALL,
		0 )
{
}


void
BasePrefsView::AttachedToWindow()
{
	fMainBox = new BBox(
		Bounds(),
		"MainBox",
		B_FOLLOW_ALL );
	fMainBox->SetLabel( Name() );
	AddChild( fMainBox );
	fMainBox->SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );
}


/*
 * WindowPrefsView
 */


WindowPrefsView::WindowPrefsView(
	BRect frame,
	const char* name )
	: BasePrefsView(
		frame,
		name )
{
}


void
WindowPrefsView::AttachedToWindow()
{
	BasePrefsView::AttachedToWindow();
	
	/* homepage */
	BString str;
	AppSettings->FindString( kPrefsHomePage, &str );
	
	BRect rect = Bounds();
	rect.InsetBy( kItemSpacing, kItemSpacing );	
	rect.top += kBBoxExtraInset;
	
	BTextControl* homepage = new BTextControl(
		rect,
		"HOMEPAGE",
		"Home Page:",
		"",
		NULL );
	homepage->SetModificationMessage( new BMessage( HOMEPAGE_CHANGED ) );
	homepage->SetDivider( be_plain_font->StringWidth( "Home Page:" ) + kDividerSpacing );
	homepage->SetText( str.String() );
	fMainBox->AddChild( homepage );
	
	/* blank button */
	rect.top = homepage->Frame().bottom + kItemSpacing;
	BMessage* blankmsg = new BMessage( SET_BLANK_PAGE );
	blankmsg->AddPointer( "homepagecontrol", homepage );
	BButton* blankbtn = new BButton(
		rect,
		"BLANKBUTTON",
		"Use Blank Page",
		blankmsg,
		B_FOLLOW_ALL,
		B_WILL_DRAW );
	blankbtn->ResizeToPreferred();
	blankbtn->MoveTo(
		homepage->Frame().right - blankbtn->Bounds().Width(),
		blankbtn->Frame().top );
	fMainBox->AddChild( blankbtn );

	/* intelligent zoom */
	rect.left = Bounds().left + kItemSpacing;
	rect.top = blankbtn->Frame().bottom + kItemSpacing;
	BCheckBox* IZcbox = new BCheckBox(
		rect,
		"IZCBOX",
		"Intelligent Zoom",
		new BMessage( IZ_CHECKBOX ) );
	IZcbox->ResizeToPreferred();
	bool IntelligentZoom = false;
	AppSettings->FindBool( kPrefsIntelligentZoom, &IntelligentZoom );
	if( IntelligentZoom == false )
		IZcbox->SetValue( 0 );
	else
		IZcbox->SetValue( 1 );
	fMainBox->AddChild( IZcbox );
	
	/* type ahead */
	rect.top = IZcbox->Frame().bottom + kItemSpacing;
	BCheckBox* ShowTypeAheadbox = new BCheckBox(
		rect,
		"SHOWTYPEAHEADBOX",
		"Show Type Ahead Window",
		new BMessage( SHOWTYPEAHEAD_CHECKBOX ) );
	ShowTypeAheadbox->ResizeToPreferred();
	bool ShowTypeAhead = false;
	AppSettings->FindBool( kPrefsShowTypeAheadWindow, &ShowTypeAhead );
	ShowTypeAhead ? ShowTypeAheadbox->SetValue( 1 ) : ShowTypeAheadbox->SetValue( 0 );
	fMainBox->AddChild( ShowTypeAheadbox );
	
	/* show tabs */
	rect.top = ShowTypeAheadbox->Frame().bottom + kItemSpacing;
	BCheckBox* ShowTabscbox = new BCheckBox(
		rect,
		"SHOWTABSCBOX",
		"Show Tabs at Startup",
		new BMessage( SHOWTABS_CHECKBOX ) );
	ShowTabscbox->ResizeToPreferred();
	bool ShowTabs = false;
	AppSettings->FindBool( kPrefsShowTabsAtStartup, &ShowTabs );
	ShowTabs ? ShowTabscbox->SetValue( 1 ) : ShowTabscbox->SetValue( 0 );
	fMainBox->AddChild( ShowTabscbox );
					
	/* open tabs in background */
	rect.top = ShowTabscbox->Frame().bottom + kItemSpacing;
	BCheckBox* OpenTabsInBackgroundcbox = new BCheckBox(
		rect,
		"OPENTABSINBACKGROUNDCBOX",
		"Open new Tabs in Background",
		new BMessage( TABSBACKGROUND_CHECKBOX ) );
	OpenTabsInBackgroundcbox->ResizeToPreferred();
	bool OpenInBackground = false;
	AppSettings->FindBool( kPrefsOpenTabsInBackground, &OpenInBackground );
	OpenInBackground ? OpenTabsInBackgroundcbox->SetValue( 1 ) : OpenTabsInBackgroundcbox->SetValue( 0 );
	fMainBox->AddChild( OpenTabsInBackgroundcbox );
	
	/* open blank targets in tab (instead of new window) */
	rect.top = OpenTabsInBackgroundcbox->Frame().bottom + kItemSpacing;
	BCheckBox* OpenBlankTargetInTab_CBox = new BCheckBox(
		rect,
		"OPENBLANKTARGETINTAB",
		"Open \"_blank\" target in new Tab",
		new BMessage( TABSBLANK_CHECKBOX ) );
	OpenBlankTargetInTab_CBox->ResizeToPreferred();
	bool OpenBlankInTab = false;
	AppSettings->FindBool( kPrefsOpenBlankTargetInTab, &OpenBlankInTab );
	OpenBlankInTab ? OpenBlankTargetInTab_CBox->SetValue( 1 ) : OpenBlankTargetInTab_CBox->SetValue( 0 );
	fMainBox->AddChild( OpenBlankTargetInTab_CBox );
	
	/* new windows open with ... */
	rect = Bounds();
	rect.InsetBy( kItemSpacing, 0 );
	rect.top = OpenBlankTargetInTab_CBox->Frame().bottom + kItemSpacing;
	rect.bottom = rect.top + 100;
	BBox* newwinpagebox = new BBox( rect, "NEWWINPAGEBOX" );
	newwinpagebox->SetLabel( "New Windows open with ..." );
	fMainBox->AddChild( newwinpagebox );
	
	/* blank page */
	rect = newwinpagebox->Bounds();
	rect.InsetBy( kItemSpacing, kItemSpacing );
	rect.top += kBBoxExtraInset;
	BMessage* nwspmsg1 = new BMessage( NEW_WINDOW_START_PAGE );
	nwspmsg1->AddInt8( "newpagemode", 0 );		
	BRadioButton* rbtn1 = new BRadioButton( rect, "RBTN1", "Blank Page", nwspmsg1 );
	rbtn1->ResizeToPreferred();
	newwinpagebox->AddChild( rbtn1 );
	/* homepage */
	rect.top = rbtn1->Frame().bottom + kItemSpacing;
	BMessage* nwspmsg2 = new BMessage( NEW_WINDOW_START_PAGE );
	nwspmsg2->AddInt8( "newpagemode", 1 );		
	BRadioButton* rbtn2 = new BRadioButton( rect, "RBTN2", "Home Page", nwspmsg2 );
	rbtn2->ResizeToPreferred();
	newwinpagebox->AddChild( rbtn2 );
	/* current page */
	rect.top = rbtn2->Frame().bottom + kItemSpacing;
	BMessage* nwspmsg3 = new BMessage( NEW_WINDOW_START_PAGE );
	nwspmsg3->AddInt8( "newpagemode", 2 );		
	BRadioButton* rbtn3 = new BRadioButton( rect, "RBTN3", "Current Page", nwspmsg3 );
	rbtn3->ResizeToPreferred();
	newwinpagebox->AddChild( rbtn3 );
	
	/* resize the box */
	BPoint pt( rbtn3->Frame().LeftBottom() );
	pt.y += kItemSpacing;
	newwinpagebox->ConvertToParent( &pt );
	newwinpagebox->ResizeTo(
		newwinpagebox->Bounds().Width(),
		pt.y - newwinpagebox->Frame().top );

	int8 NewPageMode = 0;
	AppSettings->FindInt8( kPrefsNewWindowStartPage, &NewPageMode );
	/* select the correct radiobutton */
	switch( NewPageMode )
	{
		case 0 : { rbtn1->SetValue( B_CONTROL_ON ); break; }
		case 1 : { rbtn2->SetValue( B_CONTROL_ON ); break; }
		case 2 : { rbtn3->SetValue( B_CONTROL_ON ); break; }
	}
}


/*
 * NetworkPrefsView
 */


NetworkPrefsView::NetworkPrefsView(
	BRect frame,
	const char* name )
	: BasePrefsView(
		frame,
		name )
{
}


void
NetworkPrefsView::AttachedToWindow()
{
	BasePrefsView::AttachedToWindow();
}


/*
 * PrivacyPrefsView
 */


PrivacyPrefsView::PrivacyPrefsView(
	BRect frame,
	const char* name )
	: BasePrefsView(
		frame,
		name )
{
}


void
PrivacyPrefsView::AttachedToWindow()
{
	BasePrefsView::AttachedToWindow();
	
	BRect rect = fMainBox->Bounds();
	
	rect.InsetBy( kItemSpacing, kItemSpacing );
	rect.top += kBBoxExtraInset;
	
	/* history box */	
	BBox* HistoryBox = new BBox(
		rect,
		"HistoryBox" );
	HistoryBox->SetLabel( "History" );
	fMainBox->AddChild( HistoryBox );
	
	/* history depth */
	rect = HistoryBox->Bounds();
	rect.InsetBy( kItemSpacing, kItemSpacing );
	rect.top += kBBoxExtraInset;
	BTextControl* ghistdepth = new BTextControl(
		rect,
		"GLOBALHISTDEPTH",
		"History Depth [in Days, 1-127]:",
		"",
		NULL );
	BMessage* chmsg1 = new BMessage( HISTORY_CHANGED );
	chmsg1->AddInt8( "which", 0 );
	ghistdepth->SetModificationMessage( chmsg1 );
	char string[ 5 ];
	int8 days = 0;
	AppSettings->FindInt8( kPrefsGlobalHistoryDepthInDays, &days );
	sprintf( string, "%d", days );
	ghistdepth->SetText( string );
	ghistdepth->TextView()->AddFilter( new DigitOnlyMessageFilter() );
	HistoryBox->AddChild( ghistdepth );
	
	/* free urls */
	rect.top = ghistdepth->Frame().bottom + kItemSpacing;
	BTextControl* freeurlcount = new BTextControl(
		rect,
		"FREEURLCOUNT",
		"Number of free URLs [1-127]:",
		"",
		NULL );
	BMessage* chmsg2 = new BMessage( HISTORY_CHANGED );
	chmsg2->AddInt8( "which", 1 );
	freeurlcount->SetModificationMessage( chmsg2 );
	int8 count = 0;
	AppSettings->FindInt8( kPrefsGlobalHistoryFreeURLCount, &count );
	sprintf( string, "%d", count );
	freeurlcount->SetText( string );
	freeurlcount->TextView()->AddFilter( new DigitOnlyMessageFilter() );
	HistoryBox->AddChild( freeurlcount );
	
	/* tab history */
	rect.top = freeurlcount->Frame().bottom + kItemSpacing;
	BTextControl* tabhistdepth = new BTextControl(
		rect,
		"TABHISTDEPTH",
		"Tab History Depth [1-127]:",
		"",
		NULL );
	BMessage* chmsg3 = new BMessage( HISTORY_CHANGED );
	chmsg3->AddInt8( "which", 2 );
	tabhistdepth->SetModificationMessage( chmsg3 );
	AppSettings->FindInt8( kPrefsTabHistoryDepth, &count );				
	sprintf( string, "%d", count );
	tabhistdepth->SetText( string );
	tabhistdepth->TextView()->AddFilter( new DigitOnlyMessageFilter() );
	HistoryBox->AddChild( tabhistdepth );

	/* set the dividers, and resize the controls to be smaller */
	float maxwidth = be_plain_font->StringWidth( "History Depth [in Days, 1-127]:" );
	float width2 = be_plain_font->StringWidth( "Number of free URLs [1-127]:" );
	float width3 = be_plain_font->StringWidth( "Tab History Depth [1-127]:" );
	if( maxwidth < width2 )
		maxwidth = width2;
	if( maxwidth < width3 )
		maxwidth = width3;
	maxwidth += kDividerSpacing;
		
	ghistdepth->SetDivider( maxwidth );
	freeurlcount->SetDivider( maxwidth );
	tabhistdepth->SetDivider( maxwidth );
	
	ghistdepth->ResizeTo(
		ghistdepth->Divider() + 50,
		ghistdepth->Bounds().Height() );
	freeurlcount->ResizeTo(
		freeurlcount->Divider() + 50,
		freeurlcount->Bounds().Height() );
	tabhistdepth->ResizeTo(
		tabhistdepth->Divider() + 50,
		tabhistdepth->Bounds().Height() );

	/* clear button */
	BButton* clearHistbtn = new BButton(
		rect,
		"CLEARGHBUTTON",
		"Clear History",
		new BMessage( HISTORY_CLEAR ),
		B_FOLLOW_RIGHT | B_FOLLOW_TOP,
		B_WILL_DRAW );
	clearHistbtn->ResizeToPreferred();
	HistoryBox->AddChild( clearHistbtn );
	
	/* resize HistoryBox */
	BPoint pt( tabhistdepth->Frame().LeftBottom() );
	pt.y += kItemSpacing;
	HistoryBox->ConvertToParent( &pt );
	HistoryBox->ResizeTo(
		HistoryBox->Bounds().Width(),
		pt.y - HistoryBox->Frame().top );
	
	/* move button to right center */
	clearHistbtn->MoveTo(
		HistoryBox->Bounds().right - ( clearHistbtn->Bounds().Width() + kItemSpacing ),
		HistoryBox->Bounds().Height() / 2 - clearHistbtn->Bounds().Height() / 2 );
	
	/* cache box */
	rect = HistoryBox->Frame();
	rect.top = rect.bottom + kItemSpacing;
	rect.bottom = rect.top + 100;
	BBox* CacheBox = new BBox(
		rect, "CACHEBOX" );
	CacheBox->SetLabel( "Cache" );
	fMainBox->AddChild( CacheBox );

	/* cookie box */
	rect = CacheBox->Frame();
	rect.top = rect.bottom + kItemSpacing;
	rect.bottom = rect.top + 100;
	BBox* CookieBox = new BBox(
		rect, "COOKIEBOX" );
	CookieBox->SetLabel( "Cookies" );
	fMainBox->AddChild( CookieBox );

}


/*
 * HTMLParserPrefsView
 */


HTMLParserPrefsView::HTMLParserPrefsView(
	BRect frame,
	const char* name )
	: BasePrefsView(
		frame,
		name )
{
}


void
HTMLParserPrefsView::AttachedToWindow()
{
	BasePrefsView::AttachedToWindow();
	
	/* DTD selection */
	BPopUpMenu* popmenu = new BPopUpMenu( "No DTD selected or available!", true, true, B_ITEMS_IN_COLUMN );
					
	/* find a DTD */
	BString dtddir;
	AppSettings->FindString( kPrefsSettingsDirectory, &dtddir );
	dtddir.Append( "/dtd/" );
	printf( "DTD dir: %s\n", dtddir.String() );
			
	BDirectory dir( dtddir.String() );
	if( dir.InitCheck() != B_OK )
	{
		printf( "DTD directory (%s) not found!\n", dtddir.String() );
		printf( "Setting DTDToUsePath to \"none\"\n" );
		AppSettings->AddString( kPrefsActiveDTDPath, kNoDTDFoundString );
	}
	else
	{
		BString activeDTD;
		AppSettings->FindString( kPrefsActiveDTDPath, &activeDTD );
		
		BEntry entry;
		while( dir.GetNextEntry( &entry, false ) != B_ENTRY_NOT_FOUND )
		{
			BPath path;
			entry.GetPath( &path );
			char name[B_FILE_NAME_LENGTH];
			entry.GetName( name );
				
			BString nstring( name );
			printf( "----------------\n" );
			printf( "found file: %s\n", nstring.String() );
			if( nstring.IFindFirst( "DTD", nstring.Length() - 3 ) != B_ERROR )
			{
				printf( "found DTD file: %s\n", nstring.String() );
								
				/* add the file to the popupmenu */
				BMessage* msg = new BMessage( DTD_SELECTED );
				msg->AddString( "DTDFileString", path.Path() );
				BMenuItem* item = new BMenuItem( name, msg, 0, 0 );
				popmenu->AddItem( item );
								
				// if the path of the current file equals the one of the settings,
				// mark the item
				if( strcmp( activeDTD.String(), path.Path() ) == 0 )
				{
					printf( "DTD from settings found -> SetMarked( true )\n" );
					( popmenu->ItemAt( popmenu->CountItems() - 1 ) )->SetMarked( true );
				}
			}
		} // while
						
		// if we found some DTDs, but still no DTD is saved in the prefs,
		// or no DTD is selected: 
		// set the last found DTD in the prefs. we save it to the prefs,
		// because the user might not reselect a DTD in the list, which
		// would save the DTD.
		BMessage imsg( DTD_SELECTED );
		BMessenger msgr( NULL, Window()->Looper() );
		if( popmenu->CountItems() > 0 )
		{
			if( popmenu->FindMarked() == NULL )
			{
				printf( "no marked item found\n" );
				BMenuItem* item = popmenu->ItemAt( popmenu->CountItems() - 1 );
				item->SetMarked( true );
				// as we cannot invoke the item here, send the DTD_SELECTED message here
				BString dtdstring( dtddir.String() );
				dtdstring.Append( item->Label() );
				imsg.AddString( "DTDFileString", dtdstring.String() );
				msgr.SendMessage( &imsg );
			}
		}
		else
		{
			imsg.AddString( "DTDFileString", kNoDTDFoundString );
			msgr.SendMessage( &imsg );
		}
	}
	// end: find a DTD
		
	BRect rect = fMainBox->Bounds();
	rect.InsetBy( kItemSpacing, kItemSpacing );
	rect.top += kBBoxExtraInset;
					
	BMenuField* dtdmenufield = new BMenuField( rect, "DTDFIELD", "Document Type Definition:", popmenu, true, B_FOLLOW_TOP, B_WILL_DRAW);
	dtdmenufield->SetDivider( be_plain_font->StringWidth( "Document Type Definition:" ) + kItemSpacing );
	fMainBox->AddChild( dtdmenufield );
}

/*
 * CSSParserPrefsView
 */


CSSParserPrefsView::CSSParserPrefsView(
	BRect frame,
	const char* name )
	: BasePrefsView(
		frame,
		name )
{
}


void
CSSParserPrefsView::AttachedToWindow()
{
	BasePrefsView::AttachedToWindow();
	
	/* CSS file selection */
	BPopUpMenu* popmenu = new BPopUpMenu( "No CSS selected or available!", true, true, B_ITEMS_IN_COLUMN );
					
	/* find a CSS file */
	BString cssdir;
	AppSettings->FindString( kPrefsSettingsDirectory, &cssdir );
	cssdir.Append( "/css/" );
	printf( "CSS dir: %s\n", cssdir.String() );
			
	BDirectory dir( cssdir.String() );
	if( dir.InitCheck() != B_OK )
	{
		printf( "CSS directory (%s) not found!\n", cssdir.String() );
		printf( "Setting CSSToUsePath to \"none\"\n" );
		AppSettings->AddString( kPrefsActiveCSSPath, kNoCSSFoundString );
	}
	else
	{
		BString activeCSS;
		AppSettings->FindString( kPrefsActiveCSSPath, &activeCSS );
		
		BEntry entry;
		while( dir.GetNextEntry( &entry, false ) != B_ENTRY_NOT_FOUND )
		{
			BPath path;
			entry.GetPath( &path );
			char name[B_FILE_NAME_LENGTH];
			entry.GetName( name );
				
			BString nstring( name );
			printf( "----------------\n" );
			printf( "found CSS file: %s\n", nstring.String() );
							
			/* add the file to the popupmenu */
			BMessage* msg = new BMessage( CSS_SELECTED );
			msg->AddString( "CSSFileString", path.Path() );
			BMenuItem* item = new BMenuItem( name, msg, 0, 0 );
			popmenu->AddItem( item );
							
			// if the path of the current file equals the one of the settings,
			// mark the item
			if( strcmp( activeCSS.String(), path.Path() ) == 0 )
			{
				printf( "CSS from settings found -> SetMarked( true )\n" );
				( popmenu->ItemAt( popmenu->CountItems() - 1 ) )->SetMarked( true );
			}
		} // while
						
		// if we found some CSS files, but still no CSS file is saved in the prefs,
		// or no CSS file is selected: 
		// set the last found CSS file in the prefs. we save it to the prefs,
		// because the user might not reselect a CSS file in the list, which
		// would save the CSS file.
		BMessage imsg( CSS_SELECTED );
		BMessenger msgr( NULL, Window()->Looper() );
		if( popmenu->CountItems() > 0 )
		{
			if( popmenu->FindMarked() == NULL )
			{
				printf( "no marked item found\n" );
				BMenuItem* item = popmenu->ItemAt( popmenu->CountItems() - 1 );
				item->SetMarked( true );
				// as we cannot invoke the item here, send the CSS_SELECTED message here
				BString cssstring( cssdir.String() );
				cssstring.Append( item->Label() );
				imsg.AddString( "CSSFileString", cssstring.String() );
				msgr.SendMessage( &imsg );
			}
		}
		else
		{
			imsg.AddString( "CSSFileString", kNoCSSFoundString );
			msgr.SendMessage( &imsg );
		}
	}
	// end: find a CSS file
		
	BRect rect = fMainBox->Bounds();
	rect.InsetBy( kItemSpacing, kItemSpacing );
	rect.top += kBBoxExtraInset;
					
	BMenuField* cssmenufield = new BMenuField( rect, "CSSFIELD", "Cascading Style Sheet:", popmenu, true, B_FOLLOW_TOP, B_WILL_DRAW);
	cssmenufield->SetDivider( be_plain_font->StringWidth( "Cascading Style Sheet:" ) + kItemSpacing );
	fMainBox->AddChild( cssmenufield );
}


/*
 * RendererPrefsView
 */


RendererPrefsView::RendererPrefsView(
	BRect frame,
	const char* name )
	: BasePrefsView(
		frame,
		name )
{
}


void
RendererPrefsView::AttachedToWindow()
{
	BasePrefsView::AttachedToWindow();
}
