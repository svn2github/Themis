/* ThemisTabView.cpp */

// Standard C headers
#include <stdio.h>
#include <math.h>

// Standard C++ headers
#include <iostream>

// BeOS headers
#include <GraphicsDefs.h>
#include <TranslationUtils.h>
#include <PopUpMenu.h>
#include <MenuItem.h>
#include <Message.h>
#include <Entry.h>
#include <Node.h>
#include <Path.h>
#include <String.h>
#include <Messenger.h>
#include <Window.h>

// Themis headers
#include "win.h"
#include "commondefs.h"
#include "plugclass.h"
#include "ThemisTab.h"
#include "ThemisNavView.h"	// ThemisPictureButton
#include "app.h"
#include "SiteHandler.h"
#include "ThemisUrlView.h"
#include "ThemisStatusView.h"
#include "ThemisUrlPopUpWindow.h"
#include "../common/PrefsDefs.h"
#include "ThemisTabView.h"
#include "TPictureButton.h"

ThemisTabView::ThemisTabView(
	BRect frame,
	const char* name,
	button_width width,
	uint32 resizingmode,
	uint32 flags )
	: BTabView( frame, name,
		width,
		resizingmode,
		flags )
{
	tab_width = 150.0;
	SetTabHeight( 25.0 );
}

void
ThemisTabView::AttachedToWindow()
{
	SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );
	
	CreateCloseTabViewButton();
	
	// ShowTabsAtStartup	
	bool show_tabs = false;
	AppSettings->FindBool( kPrefsShowTabsAtStartup, &show_tabs );
	if( show_tabs == true )
		SetNormalTabView();
	else
		SetFakeSingleView();
	
	ContainerView()->AddFilter( new ContainerViewMessageFilter( ( Win* )Window() ) );
}

void
ThemisTabView::Draw( BRect updaterect )
{
	//cout << "ThemisTabView::Draw()" << endl;
	
	// draw the background of the tabs
	
	updaterect = Bounds();
	
	rgb_color lo = LowColor();
	SetLowColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );
	FillRect( updaterect, B_SOLID_LOW );
	SetLowColor( lo );
	
	DrawBox( DrawTabs() );
	
	if( CountChildren() > 0 )
	{
		BView* child = NULL;
		for( int32 i = 0; i < CountChildren(); i++ )
		{
			child = ChildAt( i );
			child->Draw( child->Bounds() );
		}
	}
}

void
ThemisTabView::DrawBox( BRect selTabRect )
{
	//cout << "ThemisTabView::DrawBox()" << endl;

	// we do nothing here
}

BRect
ThemisTabView::DrawTabs( void )
{
	//cout << "ThemisTabView::DrawTabs()" << endl;
	
	// get the number of tabs
	int32 count = CountTabs();
	//cout << "ThemisTabView::Draw() : " << count << " tabs to draw" << endl;
	
	// get the selected tab
	int32 selection = Selection();
	
	BRect rect;
	
	// we want a modified tab
	rect.left = 0.0;
	rect.top = 2.0;
	rect.right = tab_width;
	rect.bottom = TabHeight() - 2;
	
	// if we got sth to draw
	if( count >= 1 )
	{
		ThemisTab* temptab;
		int li = 0;
		
		// calculate the number of tabs which will be drawn..
		// some tabs may be out of window when you resized the window smaller
		// and tabs already have had their minimum of 25 pix in width
		// those tabs would be either out of the window or be displayed under
		// the closetabview-button .. so we dont draw them...
		while( ( count * tab_width ) > ( Bounds().right - 22 ) )
			count--;
				
		for( li = 0; li <= count - 1; li++ )
		{
			temptab = ( ThemisTab* )TabAt( li );
			
			if( temptab != NULL )
			{
				if( li > 0 )
				{
					rect.left = ( li ) * tab_width;
					rect.right = ( li + 1 ) * tab_width;
				}	
				
				if( li == selection )
				{
					temptab->DrawTab( this, rect, B_TAB_FRONT, true );
					//cout << "ThemisTabView::DrawTabs(): tab " << li << " drawn( selected )" << endl;
				}
				else
				{
					temptab->DrawTab( this, rect, B_TAB_ANY, false );
					//cout << "ThemisTabView::DrawTabs(): tab " << li << " drawn" << endl;
				}
			}
		}
	}
	
	// now draw the 'line' between tabs bottom and site-content
	rgb_color hi = HighColor();
	
	BRect brect = Bounds();
	BRect linerect;
	linerect.left = brect.left;
	linerect.top = rect.bottom + 1;
	linerect.right = brect.right;
	linerect.bottom = linerect.top + 1;
	
	// some color
	rgb_color active_tab_color = ui_color( B_PANEL_BACKGROUND_COLOR );
		
	SetHighColor( active_tab_color );
	FillRect( linerect, B_SOLID_HIGH );
	
	// and stroke a black line from left to right at bottom
	SetHighColor( kColorDarkBorder );
	StrokeLine( BPoint( brect.left, linerect.bottom + 1 ),
		BPoint( brect.right, linerect.bottom + 1 ), B_SOLID_HIGH );
	
	// we still need a part of the black upper line
	// ( the black one from rightmost tab till windows right side )
	// the rest of the line is already drawn by the B_TAB_ANY tabs
	BPoint startpoint;
	startpoint.x = count * tab_width;
	startpoint.y = linerect.top - 2;
	StrokeLine( startpoint, BPoint( linerect.right, linerect.top - 2 ),
		B_SOLID_HIGH );
	// the shadow below the upper black line
	SetHighColor( kColorShadow );
	startpoint.y += 1;
	StrokeLine( startpoint, BPoint( linerect.right, linerect.top - 1 ),
		B_SOLID_HIGH );
	
	SetHighColor( hi );
	
	return rect;
}

void
ThemisTabView::MakeFocus( bool focus )
{
	// i never want the focus on the tabs,
	// but on the tabs targetview
	
	if( TabAt( Selection() )->View() != NULL )
	{	
		Window()->CurrentFocus()->MakeFocus( false );
		TabAt( Selection() )->View()->MakeFocus( true );
	}
}

void
ThemisTabView::MouseDown( BPoint point )
{
	// at first check, if the urlpopupwindow is still open
	// if yes, close it and return.
	Win* win = ( Win* )Window();
	if(win->CloseUrlPopUpWindow()) {
		return;
	}
		
	uint32 buttons;
	//cout << "ThemisTabView::MouseDown(): point.x: " << point.x << endl;
	
	// stupid to get the point once again, but who cares :D
	GetMouse( &point, &buttons, true );
	
	// get the number of tabs for calculation of the maximum
	// BPoint.x value possible
	int32 count = CountTabs();
	//cout << "ThemisTabView::MouseDown() : " << count << " tabs" << endl;
			
	// lets calculate which tab we have pointed at
	float xpos = point.x;
	int32 tabindex = 0;
	
	while( xpos > 0 )
	{
		if( xpos < tab_width )
			break;
		xpos = xpos - tab_width;
		tabindex++;				
	}
	//cout << "ThemisTabView::MouseDown() : tab pointed at: " << tabindex << endl;
			
	switch( buttons )
	{
		case B_PRIMARY_MOUSE_BUTTON :
		{
			//cout << "ThemisTabView::MouseDown() : B_PRIMARY_MOUSE_BUTTON" << endl;
			
			// if out of tab
			if( point.x > count * tab_width )
			{
				//cout << "ThemisTabView::MouseDown() : point.x not in tab-region" << endl;
				
				// find the doubleclick which adds a new tab
				BMessage* msg = Window()->CurrentMessage();
				uint32 clicks = msg->FindInt32( "clicks" );
				
				if( lastbutton == buttons && clicks > 1 )
				{
					BMessenger* target = new BMessenger( NULL, Looper() );
					target->SendMessage( TAB_ADD );
				}
				lastbutton = buttons;
				
				break;
			}
			
			// select the tab and draw it as B_TAB_FRONT/full
			Select( tabindex );
			
			// make either the targetview the new focus
			// or the urlview when the tab is empty
			if( TabAt( tabindex )->View() != NULL )
			{
				if( Window()->CurrentFocus() != NULL )
					Window()->CurrentFocus()->MakeFocus( false );
				TabAt( tabindex )->View()->MakeFocus( true );
			}
			else
			{
				if( Window()->CurrentFocus() != NULL )
					Window()->CurrentFocus()->MakeFocus( false );
				( ( Win* )Window() )->GetNavView()->urlview->TextView()->MakeFocus( true );
			}
			break;
		}
		case B_SECONDARY_MOUSE_BUTTON :
		{
			//cout << "ThemisTabView::MouseDown() : B_SECONDARY_MOUSE_DOWN" << endl;
			
			if( point.x > count * tab_width )
				break;
						
			BMenuItem* selected = NULL;
			BMessage* cotmsg = new BMessage( CLOSE_OTHER_TABS );
			cotmsg->AddInt32( "tabindex", tabindex );
			
			BPopUpMenu* popupmenu = new BPopUpMenu( "TABPOPUPMENU", true, false, B_ITEMS_IN_COLUMN );
			BMenuItem* addtab = new BMenuItem( "Add new Tab", new BMessage( TAB_ADD ), 0, 0 );
			BMenuItem* closethistab = new BMenuItem( "Close this Tab", new BMessage( TAB_CLOSE ), 0, 0 );
			BMenuItem* closeothertabs = new BMenuItem( "Close other Tabs", cotmsg, 0, 0 );
					
			popupmenu->AddItem( addtab );
			popupmenu->AddSeparatorItem();
			popupmenu->AddItem( closethistab );
			popupmenu->AddSeparatorItem();
			popupmenu->AddItem( closeothertabs );
			
			selected = popupmenu->Go( ConvertToScreen( point ) );
			
			if( selected )
			{
				// if the message is not TAB_CLOSE
				if( selected->Message()->what != TAB_CLOSE )
				{
					BMessenger* target = new BMessenger( NULL, Looper() );
					target->SendMessage( selected->Message() );
					break;
				}
				// if the message is TAB_CLOSE, we go on to B_TERTIARY_MOUSE_BUTTON
			}
			else
				break;
		}
		case B_TERTIARY_MOUSE_BUTTON :
		{
			//cout << "ThemisTabView::MouseDown() : B_TERTIARY_MOUSE_DOWN" << endl;
			
			if( point.x > count * tab_width )
				break;
						
			if( CountTabs() > 1 )
			{
				ThemisTab* remtab = ( ThemisTab* )RemoveTab( tabindex );
				
				/* Generate a broadcast to tell the UrlHandler that it can remove
				 * the UrlEntry for the given ID. */
				
				if( remtab->GetSiteID() > 0 );
				{
					BMessage* msg = new BMessage( SH_DOC_CLOSED );
					msg->AddInt32( "view_id", remtab->GetSiteID() );
					msg->AddInt32( "command", COMMAND_INFO );
				
					( ( Win* )Window() )->Broadcast( MS_TARGET_SITEHANDLER, msg );
					delete msg;
				}
				
				// tab isnt deleted by RemoveTab()
				delete ( remtab );
				
				// if the newtab button is disabled, and no more tabs are
				// out of range, enable the button
				if( ( CountTabs() * tab_width ) <= ( Bounds().right - 22 ) )
					( ( Win* )Window() )->GetNavView()->buttons[4]->SetMode( 0 );
								
				// calculate new ( bigger ) size of tabs and draw again
				DynamicTabs( false );
				//Draw( Bounds() ); 
				Select( Selection() );
								
				break;
			}
			
			// if its the last tab
			SetFakeSingleView();
				
			break;
		}
		default :
			break;
	}
}


void
ThemisTabView::Select( int32 tabindex )
{
	printf( "ThemisTabView::Select( %ld )\n", tabindex );
	
	/* Hinder tab selection when urlpopupwindow is open. */
	if (((Win *)Window())->GetUrlPopUpWindow() != NULL)
		return;
	
	/* Get a pointer to the tab. We need to query it for the view's ID. */
	ThemisTab* tab = ( ThemisTab* )TabAt( tabindex );
	if( !tab )
		return;	
	
	int32 site_id;
	site_id = tab->GetSiteID();
	
	SiteHandler* sh = ( ( App* )be_app )->GetSiteHandler();
	if( sh )
	{
		BString wtitle( "Themis" );
		BString ttitle( "" );
		BString url( "" );
		BString stext( "" );
		int lprog = 100;
		
		if( sh->EntryValid( site_id ) )
		{
//			printf( "  Entry valid.\n" );	
			
			/* Tab title */
			ttitle.Append( sh->GetTitleFor( site_id ) );
			
			/* Window title */
			wtitle.Append( " - " );
			wtitle.Append( ttitle.String() );
			
			/* NavView URL */
			url.SetTo( sh->GetUrlFor( site_id ) );
			
			/* StatusText and loading progress */
			stext.SetTo( sh->GetStatusTextFor( site_id ) );
			lprog = sh->GetLoadingProgressFor( site_id );
		}
		else	// no UrlEntry
		{
			printf( "  No UrlEntry.\n" );
			
			/* Tab title */
			ttitle.Append( "(untitled)" );
			
			/* Window title */
			// Append nothing to the wtitle for non-existant pages.
			
			/* NavView URL */
			url.SetTo( "" );
			
			/* StatusText and loading progress */
			// Do nothing with these values for empty tabs.
		}
		
		/* Set the values */
		Win* win = ( Win* )Window();

		win->SetTitle( wtitle.String() );
		tab->SetLabel( ttitle.String() );
		win->GetNavView()->urlview->SetText( url.String() );
		win->GetNavView()->urlview->SetFavIcon( tab->GetFavIcon() );
		win->SetLoadingInfo(lprog, stext.String());
			
	}
	
	/* Handle the possible resizing. */
	
	if( tab->View() )
	{
		BView* rview = tab->View();
		if( rview )
		{
			BRect cviewrect = ContainerView()->Bounds();
			BRect rviewrect = rview->Bounds();
	
			if( rviewrect != cviewrect )
			{
				rview->ResizeTo( cviewrect.right, cviewrect.bottom );
			}
		}		
	}
	
	
//	FakeSite* tempview = ( FakeSite* )TabAt( tabindex )->View();
//		
//	// draw the appropriate url in the urlview ( site_title for the moment )
//	// ( there is a "bug" at the moment: if we have no startup page specified
//	// in the prefs, we have an empty tab ( '(untitled)' )...
//	// and nothing like 'about:blank' is shown in the urlline
//	// if we click the tab, 'about:blank' is shown...
//	// this could be a bug but isnt really one as i dont wanna get rid of the
//	// 'about:blank' first and then type the url when i start the browser )
//	
//	// we need to make sure that we have tabs, because Select() is called
//	// on construction of the tabview before a tab is added	
//	if( CountTabs() > 0 )
//	{
//		BString window_title( "Themis" );
//		
//		if( tempview != NULL )
//		{
//		
//			/* some temporary removals */
//		
//			// set urlview-text
////			((Win*)Window())->navview->urlview->SetText( tempview->site_title.String() );
//			
//			// set the urlview-icon
////			((Win*)Window())->navview->urlview->SetFavIcon( tempview->site_fav_icon );
//			
//			// set window-title-text
////			window_title << " - " << tempview->site_title.String();
//			
//			Window()->SetTitle( window_title.String() );
//			
//			// set up the statusview
////			( ( Win* )Window() )->statusview->SetValues(
////				tempview->GetDocBarProgress(),
////				tempview->GetDocBarText(),
////				tempview->GetImgBarProgress(),
////				tempview->GetImgBarText(),
////				tempview->GetStatusText(),
////				tempview->GetSecureState(),
////				tempview->GetCookieState() );
//		}
//		else
//		{
//			// we could also set "" here ( better for usability, cause we dont
//			// have to get rid of 'about:blank' everytime )
//			((Win*)Window())->navview->urlview->SetText( "about:blank" );
//			
//			// set the urlview-icon
//			((Win*)Window())->navview->urlview->SetFavIcon( NULL );
//			
//			// set window-title-text
//			Window()->SetTitle( window_title.String() );
//			
//			// set up the statusview
//			( ( Win* )Window() )->statusview->SetValues(
//				100,
//				"",
//				100,
//				"",
//				"",
//				false,
//				false );
//		}
//	}
//	
//	// now check, if the containerview bounds and the selected tabs view bounds
//	// are the same, if not, the window size had been changed, and the view
//	// needs to be resized
//	// ( i think its the best way of performance saving, as we only resize the
//	// selected tabs view right before its selected instead of resizing every
//	// targetview on each frameresize event -> geez, that'd be really cpu-intensive :D 
//	// i think the user has an understanding for the little lag before we switch )
//	
//	if( tempview != NULL )
//	{
//		BRect cviewrect = ContainerView()->Bounds();
//		BRect tviewrect = tempview->Bounds();
//	
//		if( tviewrect != cviewrect )
//		{
//			// we should call sth like RenderView::Resize( cviewrect ) later
//			tempview->ResizeTo( cviewrect.right, cviewrect.bottom );
//		}
//	}
	
	BTabView::Select( tabindex );
	
	// as the methods name says, set the nav buttons according the tabs history
	SetNavButtonsByTabHistory();
}

void
ThemisTabView::SetFocusTab( int32 tab, bool focused )
{
	//cout << "ThemisTabView::SetFocusTab()" << endl;
	
	// i do nothing here atm...
	// needed to override this, as this function would have
	// drawn those blue lines under the tablabel if it has
	// focus...those lines are just ugly, and i'd have to
	// reimplement this whole function, as those lines
	// were of BTab standard width and not our tab length
	// after all, i think we can forego this function atm
	// as it is very uncommon to navigate through the
	// tabs by keyboard
	
	// ps: this function can be left out totally, as we never
	// let the tabview get the focus ...
	// see ThemisTabView::MakeFocus(..)
}

void
ThemisTabView::CreateCloseTabViewButton()
{
	BRect rect = Bounds();
	
	// the small 16x16 bitmap for on, active or off
	BBitmap* smallbmp = NULL;
	
	// the pictures
	BPicture* onpic = NULL;
	BPicture* overpic = NULL;
	BPicture* activepic = NULL;
		
	// the view in which we will draw the Pictures
	BView *tempview = new BView( BRect( 0,0,15,15 ), "TEMPVIEW", B_FOLLOW_NONE, B_WILL_DRAW );
	tempview->SetDrawingMode( B_OP_ALPHA );
	AddChild( tempview );
	
	Win* win = ( Win* )Window();
	
	// 3-pic bitmap -> limit = 2
	int limit = 2;	
	for( int j = 0; j <= limit; j++ )
	{
		// extract partial bitmaps
	
		smallbmp = new BBitmap( BRect( 0,0,15,15 ), B_RGB32 );
		
		unsigned char* p1 = ( unsigned char* )win->GetBitmap(7)->Bits();
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
	}
			
	// create the message which will be sent to the window
	BMessage* closemsg = new BMessage( CLOSE_OTHER_TABS );
	closemsg->AddBool( "close_last_tab", true );
	
	// create the BPictureButton
	close_tabview_button = new TPictureButton(
		BRect(
			rect.right - 20,
			rect.top + 3,
			rect.right - 20 + 15,
			rect.top + 3 + 15 ), 
		"CLOSETABVIEWPICBUTTON",
		onpic,
		overpic,
		activepic,
		NULL,
		closemsg,
		B_FOLLOW_RIGHT,
		B_WILL_DRAW );
	
	// reset the bitmaps and pics
	smallbmp = NULL;
	onpic = NULL;
	overpic = NULL;
	activepic = NULL;
		
	// remove the tempview
	RemoveChild( tempview );
	delete tempview;
}

void
ThemisTabView::DynamicTabs( bool newtab )
{
	if( fake_single_view == true )
		return;
	
	int32 count = CountTabs();
	
	// newtab only important when adding a new tab to tabview
	// as it will increase count by one and affect the new tab_width
	if( newtab == true )
		count = count + 1 ;
	
	BRect rect = Bounds();
	float total_width = count * tab_width;
	floor( total_width );
	
	float difference = rect.right - total_width;
	
	//cout << "-----------" << endl;
	//cout << "rect.right: " << rect.right << endl;
	//cout << "tabs: " << count << endl;
	//cout << "tab_width: " << tab_width << endl;
	//cout << "total_width: " << total_width << endl;
	//cout << "difference: " << difference << endl;
	
	if( difference > 0 )
	{
		//cout << "ThemisTabView::DynamicTabs(): resizing tabs" << endl;
		
		difference -= 30;
		
		float change_width = difference / count;
		floor( change_width );
		//cout << "change_width: " << change_width << endl;
		tab_width += change_width;
				
		if( tab_width > 150.0 )
			tab_width = 150.0;
		if( tab_width < 25.0 )
			tab_width = 25.0;
	}
	else
	{
		//cout << "ThemisTabView::DynamicTabs(): downsizing tabs" << endl;
		
		difference = difference * ( -1 );
		
		difference += 30;
		
		float rem_width = difference / count;
		floor( rem_width );
		//cout << "rem_width: " << rem_width << endl;
		tab_width -= rem_width;	
				
		if( tab_width < 25.0 )
			tab_width = 25.0;
		
	}
	
	//cout << "ThemisTabView::DynamicTabs(): new tab_width = " << tab_width << endl;
}

void
ThemisTabView::SetFakeSingleView()
{
	//cout << "ThemisTabView::SetFakeSingleView()" << endl;
	
	// give the containerview a white background
	// and make it as big as the entire tabview system
	BRect rect = Bounds();
	
	BView* contview = ContainerView();
	contview->SetViewColor( 255,255,255,0 );
	contview->MoveTo( rect.left, rect.top );
	contview->ResizeTo( rect.right , rect.bottom );
	
	// showing and hiding did not work perfectly
	// so i remove it
	if( FindView( "CLOSETABVIEWPICBUTTON" ) )
	{
		// clean up button states..
		close_tabview_button->SetMode( 0, true );
		
		RemoveChild( close_tabview_button );
	}	
			
	fake_single_view = true;
}

void ThemisTabView::SetNavButtonsByTabHistory() {
//	printf( "ThemisTabView::SetNavButtonsByTabHistory()\n" );
	ThemisTab* tab = (ThemisTab*)TabAt(Selection());
	
	ThemisNavView* nv = ((Win*)Window())->GetNavView();
	
//	printf("CurrentPosition: %d\n", tab->GetHistory()->GetCurrentPosition());
	tab->GetHistory()->PrintHistory();
	
	if (tab->GetHistory()->GetCurrentPosition() == 0) {
		if (tab->GetHistory()->GetEntryCount() > 1) {
//			printf("TABVIEW: enabling back, disabling fwd button if needed.\n");
			nv->buttons[0]->SetMode(0);
			nv->buttons[1]->SetMode(3);
		}
		else {
//			printf( "TABVIEW: only one ( or no )history item. disabling back and fwd if needed.\n" );
			nv->buttons[0]->SetMode(3);
			nv->buttons[1]->SetMode(3);
		}
	}
	else {
		if (tab->GetHistory()->GetCurrentPosition() == (tab->GetHistory()->GetEntryCount() - 1)) {
//			printf("TABVIEW: at history end. disable back, enable fwd if needed.\n");
			nv->buttons[0]->SetMode(3);
			nv->buttons[1]->SetMode(0);
		}
		else {
//			printf("TABVIEW: in middle of history. enabling back and fwd if needed.\n");
			nv->buttons[0]->SetMode(0);
			nv->buttons[1]->SetMode(0);
		}
	}
}

void
ThemisTabView::SetNormalTabView()
{
	//cout << "ThemisTabView::SetNormalTabView()" << endl;
	
	// give the containerview a white background
	// and set/reset it to standard size
	BRect rect = Bounds();	
				
	BView* contview = ContainerView();
	contview->SetViewColor( 255,255,255,0 );
	contview->MoveTo( rect.left, TabHeight() + 2 );
	contview->ResizeTo( rect.right, rect.bottom - TabHeight() - 2 );
	
	if( !FindView( "CLOSETABVIEWPICBUTTON" ) )
	{
		close_tabview_button->MoveTo(
			rect.right - 20,
			rect.top + 3 );
		close_tabview_button->SetMode( 0 );
		AddChild( close_tabview_button );
	}	
		
	fake_single_view = false;
}

void ThemisTabView :: MessageReceived(BMessage * aMessage) {
	
	switch (aMessage->what) {
		case B_SIMPLE_DATA: {
			// Get the ref that is dropped.
			entry_ref ref;
			aMessage->FindRef("refs", &ref);
			BEntry entry(&ref);
			BPath path;
			entry.GetPath(&path);
			BNode entryNode(&entry);
			char attributeValue[B_MIME_TYPE_LENGTH];
			ssize_t attributeSize = entryNode.ReadAttr("BEOS:TYPE", B_STRING_TYPE, 0, attributeValue, sizeof(attributeValue) - 1);
			if (attributeSize > 0) {
				BString mimeString = attributeValue;
				if (mimeString == "application/x-vnd.Be-bookmark") {
					attributeSize = entryNode.ReadAttr("META:url", B_STRING_TYPE, 0, attributeValue, sizeof(attributeValue) - 1);
					if (attributeSize > 0) {
						BMessage message(URL_OPEN);
						message.AddString("url_to_open", attributeValue);
						BMessenger messenger(NULL, Window());
						messenger.SendMessage(&message);
					}
				}
				else if (mimeString == "text/html") {
					BString filePath = "file://";
					filePath += path.Path();
					BMessage message(URL_OPEN);
					message.AddString("url_to_open", filePath);
					BMessenger messenger(NULL, Window());
					messenger.SendMessage(&message);
				}
			}
			break;
		}
		default: {
			BView::MessageReceived(aMessage);
		}
	};
	
}

/////////////////////////////////////
// ContainerViewMessageFilter
/////////////////////////////////////

ContainerViewMessageFilter::ContainerViewMessageFilter( Win* win )
	: BMessageFilter( B_ANY_DELIVERY, B_ANY_SOURCE )
{
	window = win;
}

filter_result
ContainerViewMessageFilter::Filter( BMessage *msg, BHandler **target )
{
	filter_result result( B_DISPATCH_MESSAGE );
	
	switch( msg->what )
	{
		case B_MOUSE_DOWN :
		{
			if (window->CloseUrlPopUpWindow())
				result = B_SKIP_MESSAGE;
			break;
		}
		default :
			break;
	}
	return result;
}
