/*
Copyright (c) 2000 Z3R0 One. All Rights Reserved. 

Permission is hereby granted, free of charge, to any person 
obtaining a copy of this software and associated documentation 
files (the "Software"), to deal in the Software without 
restriction, including without limitation the rights to use, 
copy, modify, merge, publish, distribute, sublicense, and/or 
sell copies of the Software, and to permit persons to whom 
the Software is furnished to do so, subject to the following 
conditions: 

   The above copyright notice and this permission notice 
   shall be included in all copies or substantial portions 
   of the Software. 

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY 
KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS 
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR 
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Original Author & Project Manager: Z3R0 One (z3r0_one@yahoo.com)
Project Start Date: October 18, 2000
*/

#include <Deskbar.h>
#include <Directory.h>
#include <MenuItem.h>
#include <Screen.h>
#include <TranslationKit.h>

#include <stdlib.h>
#include <ctype.h>

#include "app.h"
#include "../common/commondefs.h"
#include "protocol_plugin.h"
//#include "AppDefs.h"
#include "iostream.h"
#include "ThemisIcons.h"
#include "ThemisTab.h"
#include "win.h"

extern plugman *PluginManager;
extern BMessage *AppSettings;

Win::Win(
	BRect frame,
	const char *title,
	window_type type,
	uint32 flags,
	uint32 wspace )
    : BWindow( frame, title, type, flags, wspace ), MessageSystem()
{
	MsgSysRegister(this);
	// size limits
	BScreen Screen;
	SetSizeLimits( 300, Screen.Frame().right, 200, Screen.Frame().bottom );
	
	protocol=0;
	startup=true;
	urlpopupwindow = NULL;
	fNextWindow = NULL;
	fUniqueID = ( ( App* )be_app )->GetNewUniqueID();
	fOldFrame = Frame();
	fMaximized = false;
	fQuitConfirmed = false;

	LoadInterfaceGraphics();
	
	BRect rect;
	rect = Bounds();
	
	// lets add a menubar
	menubar = new BMenuBar( BRect(0,0,0,0), "MENUBAR" );
	AddChild( menubar );
	
	// menu messages
	BMessage* tabaddmsg = new BMessage( TAB_ADD );
	tabaddmsg->AddBool( "force_non_hidden", true );
		
	// filemenu
	filemenu = new BMenu( "File" );
	menubar->AddItem( filemenu );
	BMenuItem* newwinitem = new BMenuItem( "New Window", new BMessage( WINDOW_NEW ), 'N');
	filemenu->AddItem( newwinitem );
	BMenuItem* newtabitem = new BMenuItem( "New Tab", tabaddmsg, 'T' );
	filemenu->AddItem( newtabitem );
	BMenuItem* aboutitem = new BMenuItem( "About Themis", new BMessage( B_ABOUT_REQUESTED ), 'A', B_SHIFT_KEY );
	filemenu->AddItem( aboutitem );
	BMenuItem* quitwentry = new BMenuItem( "Quit Window", new BMessage( B_QUIT_REQUESTED ), 'W' );
	filemenu->AddItem( quitwentry );
	BMenuItem* quitentry = new BMenuItem( "Quit Themis", new BMessage( B_QUIT_REQUESTED ), 'Q' );
	filemenu->AddItem( quitentry );
	quitentry->SetTarget( this );
	// optionsmenu
	optionsmenu=new BMenu( "Options" );
	menubar->AddItem(optionsmenu);
	BMenuItem* prefsentry = new BMenuItem( "Preferences", new BMessage( SHOW_PREFERENCES ), 'P' );
	prefsentry->SetTarget( be_app );
	optionsmenu->AddItem( prefsentry );
	
		
	// now we need the navigation view
	navview = new ThemisNavView(
		BRect( rect.left, menubar->Bounds().Height() + 1,
			rect.right,	menubar->Bounds().Height() + 31 ) );
	AddChild( navview );
	
	// and finally the statusview at the bottom
	statusview = new ThemisStatusView(
		BRect(
			rect.left,
			rect.bottom - 13,	// heigth of the lower-right doc-view corner
			rect.right,			// - 13 taken out because of dano decors
			rect.bottom ) );
	AddChild( statusview );
	
	// the tabview-system comes last as we need the height of the statusview
	CreateTabView();
	
	// make the urlview the focusview for fast pasting :D
	if( CurrentFocus() != NULL )
		CurrentFocus()->MakeFocus( false );
	navview->urlview->TextView()->MakeFocus( true );
/*
	// probably removable as stated by raymond
	BMessage *info=new BMessage(AddInitInfo);
	info->AddPointer("main_menu_bar",menubar);
	info->AddPointer("file_menu",filemenu);
	info->AddPointer("options_menu",optionsmenu);
	info->AddPointer("window",this);
//	info->AddPointer("parser",Parser);
	BMessenger *iimsgr=new BMessenger(PluginManager,NULL,NULL);

	BMessage reply;	
	iimsgr->SendMessage(info,&reply);
	info->PrintToStream();
	printf("Window has tried to init: %ld\n",reply.what);
		
	reply.PrintToStream();
			
	delete iimsgr;
	delete info;
*/
}
Win::~Win(){
	MsgSysUnregister(this);
}

bool Win::QuitRequested() {
 	cout << "Win::QuitRequested" << endl;
 	if( fQuitConfirmed == false )
	{
		BMessage* closemsg = new BMessage( WINDOW_CLOSE );
		closemsg->AddPointer( "win_to_close", this );
		be_app_messenger.SendMessage( closemsg );
		printf( "returning false\n" );
		return false;
	}
	else
	{
		if( urlpopupwindow != NULL )
		{
			urlpopupwindow->Lock();
			urlpopupwindow->Quit();
			urlpopupwindow = 0;
		}
		AppSettings->ReplaceRect( "WindowRect", Frame() );
		printf( "returning true\n" );
		return true;
	}
}
void Win::MessageReceived(BMessage *msg) {
	switch(msg->what) {
		case B_ABOUT_REQUESTED: {
			be_app_messenger.SendMessage(B_ABOUT_REQUESTED);
		}break;
		case B_QUIT_REQUESTED: {
			PostMessage(B_QUIT_REQUESTED);
		}break;
		case PlugInLoaded: {
			printf("Win PlugInLoaded\n");		
			protocol_plugin *pobj=NULL;
			msg->PrintToStream();
			msg->FindPointer("plugin",(void**)&pobj);
			printf("pobj: %p\n",pobj);
			if (pobj!=NULL) {
				printf("Window: plugin loaded %c%c%c%c\n",(int)pobj->PlugID()>>24,(int)pobj->PlugID()>>16,(int)pobj->PlugID()>>8,(int)pobj->PlugID());
				pobj->Window=this;
				pobj->AddMenuItems(optionsmenu);
			} else {
				printf("Win: pobj was null\n");
				
			}
			
		}break;
		case B_MOUSE_WHEEL_CHANGED :
		{
			//cout << "B_MOUSE_WHEEL_CHANGED" << endl;
			
			// need to do the scrolling of the urlpopup here
			// as the urlpopupwindow never gets focus and thus
			// never get the mouse wheel messages
			if( urlpopupwindow != NULL )
			{
				if( urlpopupwindow->vscroll != NULL )
				{
					float value = msg->FindFloat( "be:wheel_delta_y" );
					value *= 10;
					
					urlpopupwindow->Lock();
					urlpopupwindow->urlpopupview->ulv->ScrollBy( 0, value );
					urlpopupwindow->Unlock();
				}
			}
			// i dunno where the scroll-recognition of the rendered site shall
			// happen.. if here, then it should go into here later :D
			
		}break;
		case CLOSE_OTHER_TABS :
		{
			// this function is used for the tab-pop-up-menu function
			// "Close other Tabs" and also for the closetabview_button
			cout << "CLOSE_OTHER_TABS received" << endl;
			
			Lock();
			
			// if the newtab button is disabled, enable it again
			if( navview->buttons[4]->IsEnabled() == false )
				navview->buttons[4]->SetEnabled( true );
			
			uint32 current_tab = 0;
			uint32 count = tabview->CountTabs();
			
			if( msg->HasInt32( "tabindex" ) )
				current_tab = msg->FindInt32( "tabindex" );
			else
				current_tab = tabview->Selection();
			
			while( count > 1 )
			{
				if( count - 1 > current_tab )
				{
					tabview->RemoveTab( count - 1 );
				}
				else
				{
					current_tab = current_tab - 1;
					tabview->RemoveTab( current_tab );
				}
				
				tabview->DynamicTabs( false );
				
				count = tabview->CountTabs();
			}
			
			// used by the closetabview button
			if( msg->HasBool( "close_last_tab" ) )
				if( msg->FindBool( "close_last_tab" ) == true )
					tabview->SetFakeSingleView();
			
			Unlock();
			
			
		}break;
		case CLOSE_URLPOPUP :
		{
			cout << "CLOSE_URLPOPUP received" << endl;
			if( urlpopupwindow )
			{
				urlpopupwindow->Lock();
				urlpopupwindow->Quit();
				urlpopupwindow = NULL;
			}			
				
		}break;
		case RE_INIT_INTERFACE :
		{
			//cout << "RE_INIT_INTERFACE" << endl;
			ReInitInterface();
			break;
		}
		case TAB_ADD :
		{
			cout << "TAB_ADD received" << endl;
			
			// dissallow adding of new tabs, if they wouldnt fit in the
			// window anymore, and disable newtab button
			if( tabview->tab_width <= 30 )
			{
				// calculate if still one tab would fit and not cover
				// the closetabview button partially
				float width = ( tabview->CountTabs() + 1 ) * 25;
				
				if( Bounds().right - width <= 22 )
				{
					navview->buttons[4]->SetEnabled( false );
					navview->buttons[4]->SetValue( B_CONTROL_OFF );
					break;
				}
				// if we won't cover the button, go on...
			}
						
			// if the prefs are not set to sth like "open new tabs hidden"
			// we pass hidden = false to AddNewTab
			// this selects the last tab ( the new one )
			bool hidden = true;
			AppSettings->FindBool( "OpenTabsInBackground", &hidden );
			// OPTION+T shortcut for new tab should open tabs non hidden
			bool force_non_hidden = false;
			msg->FindBool( "force_non_hidden", &force_non_hidden );
			if( force_non_hidden == true )
				hidden = false;
			AddNewTab( hidden );
			
			if( msg->HasString( "url_to_open" ) )
			{
				BMessage* urlopenmsg = new BMessage( URL_OPEN );
				urlopenmsg->AddString( "url_to_open", msg->FindString( "url_to_open" ) );
				urlopenmsg->AddInt32( "tab_to_open_in", tabview->CountTabs() - 1 );
				urlopenmsg->AddBool( "hidden", hidden );
				
				BMessenger* target = new BMessenger( this );
				
				target->SendMessage( urlopenmsg );
			}				
			break;
		}
		case URL_LOADING :
		{
 			cout << "URL_LOADING received" << endl;
			
			FakeSite* tmpsite = NULL;
			if( msg->HasInt16( "tab_uid" ) )
			{
				tmpsite = GetViewPointer(
					msg->FindInt16( "tab_uid" ),
					msg->FindInt16( "view_uid" ) );
				
				if( tmpsite != NULL )
					cout << "URL_LOADING: found viewpointer" << endl;
				else
				{
					cout << "no view found -> stopped loading" << endl;
					/////////////////
					// tell the http_plugin(?) to stop loading the site..
					/////////////////
					break;
				}
			}
			
			int64 delta=0;
			int64 contentlen=0;
			bool req_done = false;
			bool secure = false;
			bool cookies_disabled = false; // not implemented atm
						
			msg->FindInt64("size-delta",&delta);
			
			if (msg->HasInt64("content-length"))
				msg->FindInt64("content-length",&contentlen);

			msg->FindBool( "request_done", &req_done );
			msg->FindBool( "secure", &secure );
			
			BString statstr( "Transfering data from " );
			statstr.Append( tmpsite->site_title.String() );
			
			if( contentlen == 0 )
			{
				if( req_done == false )
				{
					tmpsite->SetInfo(
						50,
						false,	// delta = false
						"loading",
						50,
						false,	// delta = false
						"loading",
						statstr.String(),
						secure,
						cookies_disabled );
				}
				else
				{
						tmpsite->SetInfo(
						100,
						false,	// delta = false
						"",
						100,
						false,	// delta = false
						"",
						"Done.",
						secure,
						cookies_disabled );
				}
			}
			else
			{
				// generate the progressbar texts
				BString str1;
				str1 << (int)(((float)delta/(float)contentlen)*100) << "%";
				
				if( req_done == false )
				{
					tmpsite->SetInfo(
						(int)(((float)delta/(float)contentlen)*100),
						true,
						str1.String(),
						(int)(((float)delta/(float)contentlen)*100),
						true,
						str1.String(),
						statstr.String(),
						secure,
						cookies_disabled );
				}
				else
				{
					tmpsite->SetInfo(
						100,
						false,	// delta = false
						"",
						100,
						false,	// delta = false
						"",
						"Done.",
						secure,
						cookies_disabled );
				}
			}
			
			// if the viewpointer points to the currently active view,
			// update the statusview
			if( tmpsite == tabview->TabAt( tabview->Selection() )->View() )
			{
				//cout << "tmpsite == current active view tab" << endl;
							
				statusview->SetValues(
					tmpsite->GetDocBarProgress(),
					tmpsite->GetDocBarText(),
					tmpsite->GetImgBarProgress(),
					tmpsite->GetImgBarText(),
					tmpsite->GetStatusText(),
					tmpsite->GetSecureState(),
					tmpsite->GetCookieState() );
			}
			break;
		}
		case URL_OPEN :
		{
			cout << "URL_OPEN received" << endl;
			
			// if the urlpopupwindow exists
			//if( urlpopupwindow != NULL )
			//	PostMessage( CLOSE_URLPOPUP );
			
			// the above doesnt work as fast as expected...i am hindering the
			// tab-selection by mouse when the urlpopupwindow is open
			// the above wouldnt have closed the urlpopupwindow as needed,
			// and as the code further down under uses ThemisTabView::Select()
			// i need to make sure we get rid of the urlpopupwindow right now
			if( urlpopupwindow  )
			{	
				urlpopupwindow->Lock();
				urlpopupwindow->Quit();
				urlpopupwindow = NULL;
			}			
						
			// get the url
			BString url;
			if( msg->HasString( "url_to_open" ) )
			{
				url = msg->FindString( "url_to_open" );
			}
			else
			{
				url = navview->urlview->Text();
			}
			cout << "URL_OPEN: url_to_open: " << url.String() << endl;
			
			// stop, if there is no url, or about:blank
			if( url.Length() == 0 )
				break;
			if( strncmp( url.String(), "about:blank", 11 ) == 0 )
				break;
			
			// create the fakesite ( later the renderview )
			FakeSite* fakesite = new FakeSite(
				( tabview->ContainerView() )->Bounds(),
				url.String(), ( ( App* )be_app )->GetNewUniqueID(), this );
			
			uint32 selection = tabview->Selection();
			int16 tab_uid = 0;
			int16 view_uid = fakesite->UniqueID();
						
			if( msg->HasInt32( "tab_to_open_in" ) )
			{
				int32 tab_index = msg->FindInt32( "tab_to_open_in" );
				
				tabview->TabAt( msg->FindInt32( "tab_to_open_in" ) )->SetView( fakesite );
				tab_uid = ( ( ThemisTab* )tabview->TabAt( tab_index ) )->UniqueID();
			}
			else
			{
				tabview->TabAt( selection )->SetView( fakesite );
				tab_uid = ( ( ThemisTab* )tabview->TabAt( selection ) )->UniqueID();
			}
			
			if( msg->FindBool( "hidden" ) == true )
				tabview->DrawTabs();
			else
				tabview->Select( selection );
						
			if( CurrentFocus() != NULL )
				CurrentFocus()->MakeFocus( false );
			tabview->TabAt( selection )->View()->MakeFocus( true );
			char *usepass=NULL;
			char *workurl=NULL;
			BString urlS=url.String();
			int len=strlen(url.String());
			char *url_=new char[len+1];
			workurl=new char[len+1];
			memset(url_,0,len+1);
			memset(workurl,0,len+1);
			strcpy(url_,url.String());
			strcpy(workurl,url_);
			protocol=HTTPPlugin;
			if (urlS.Length()>0) {
				int32 pos=urlS.FindFirst("://");
				char *at=strchr(workurl,'@');
				char *colon=strstr(workurl,"://");
				if (at!=NULL) {
					colon=strstr(url_,"://");
					at=strchr(url_,'@');
					if (colon!=NULL) {
						int32 len2=at-(colon+3);
						usepass=new char[len2+1];
						memset(usepass,0,len2);
						strncpy(usepass,colon+3,len2);
						memset(workurl,0,len+1);
						strncpy(workurl,url_,(colon-url_)+3);
						strcat(workurl,at+1);
						colon=strstr(workurl,"://");
					} else {
						int32 len2=at-url_;
						char *usepass=new char[len2+1];
						memset(usepass,0,len);
						strncpy(usepass,url_,len2);
						memset(workurl,0,len+1);
						strcpy(workurl,at+1);
						colon=NULL;
					}
					
				}
				
				if (colon!=NULL) {
					pos=colon-workurl;
					char *protostr=new char[pos+1];
					memset(protostr,0,pos+1);
					strncpy(protostr,url_,pos);
					for (int i=0; i<pos;i++)
						protostr[i]=tolower(protostr[i]);
					protocol=strtoval(protostr);
					delete protostr;
					printf("Looking for \"%c%c%c%c\" protocol addon.\n",protocol>>24,protocol>>16,protocol>>8,protocol);
				}
			}
			delete url_;
			ProtocolPlugClass *pobj=(ProtocolPlugClass*)PluginManager->FindPlugin(protocol);
			printf("protocol: %ld\npobj: %p\n",protocol,pobj);
			if (urlS.Length()==0) {
				return;
			}
			url=workurl;
			if ((urlS.FindFirst("://")==B_ERROR) || (urlS.FindFirst("://")>=6)) {
				urlS.Prepend("http://");
				printf("url: %s\n",urlS.String());
				printf("workurl: %s\n",workurl);
				delete workurl;
				workurl=new char[urlS.Length()+1];
				memset(workurl,0,urlS.Length()+1);
				urlS.CopyInto(workurl,0,urlS.Length());
			}

				printf( "Win: creating info message\n" );			
//			if (pobj!=NULL) {
				BMessage *info=new BMessage;
//				info->AddPointer("tcp_layer_ptr",TCP);
//				info->AddPointer("top_view",tabview->TabAt( selection )->View());
//				info->AddPointer("window",this);
				info->AddInt16( "window_uid", UniqueID() );
				info->AddInt16( "tab_uid", tab_uid );
				info->AddInt16( "view_uid", view_uid );
//				info->AddPointer("parser",Parser);
				info->AddPointer("plug_manager",PluginManager);
				info->AddPointer("main_menu_bar",menubar);
				info->AddPointer("file_menu",filemenu);
				info->AddPointer("options_menu",optionsmenu);
				info->AddString("target_url",workurl);
				delete workurl;
//				info->AddInt32("action",LoadingNewPage);
				if (usepass!=NULL) {
					
					info->AddString("username:password",usepass);
					memset(usepass,0,strlen(usepass)+1);
					delete usepass;
				}
				
				printf("info: %p\n",info);
				info->PrintToStream();
				printf("Win: telling cache that a new page is being loaded.\n");
				BMessage *bcast=new BMessage;
				BMessage *lnp=new BMessage(LoadingNewPage);
				lnp->AddInt32("command",COMMAND_INFO);
				bcast->AddMessage("message",lnp);
				Broadcast(MS_TARGET_ALL,lnp);
				delete lnp;
				delete bcast;
				
				bcast=new BMessage(BroadcastMessage);
				info->AddInt32("command",COMMAND_RETRIEVE);
				bcast->AddMessage("message",info);
				bcast->AddInt32("targets",TARGET_PROTOCOL);
				bcast->AddInt32("source",TARGET_VIEW);
			
//				BMessenger *msgr=new BMessenger(NULL,PluginManager,NULL);
//				msgr->SendMessage(bcast);
//				delete msgr;
				
				printf("Win: sending request broadcast.\n");
				Broadcast(MS_TARGET_PROTOCOL,info);
				printf("Win: done with request broadcast\n");
			
			//	PluginManager->Broadcast(TARGET_VIEW,TARGET_PROTOCOL,bcast);
				delete bcast;
				delete info;
			
//				pobj->GetURL(info);
//			}
			
		}break;
		case URL_TYPED :
		{
			cout << "URL_TYPED received" << endl;
			
			bool show_all = false;
			msg->FindBool( "show_all", &show_all );
			
			bool prefs_show_type_ahead = false;
			AppSettings->FindBool( "ShowTypeAhead", &prefs_show_type_ahead );
									
			if( ( prefs_show_type_ahead == true ) || ( show_all == true ) )
			{
				if( show_all == true )
					UrlTypedHandler( true );
				else
					UrlTypedHandler( false );
			}
			
			
		}break;
		case WINDOW_NEW :
		{
			// resend the message to the app
			be_app_messenger.SendMessage( msg );
			break;
		}
		default: {
			BWindow::MessageReceived(msg);
		}
	}
//	Unlock();
}
uint32 Win::BroadcastTarget() {
	printf("Win\n");
	return MS_TARGET_WINDOW;
}
void Win::WindowActivated(bool active) {
	if (startup) {
		startup=false;
		printf("WindowActivated\n");
		
	}
	if( active == false )
	{
		if( urlpopupwindow != NULL )
		{
			urlpopupwindow->Lock();
			urlpopupwindow->Quit();
			urlpopupwindow = NULL;
		}
	}
	BWindow::WindowActivated(active);
}
void Win::WorkspacesChanged(uint32 oldws, uint32 newws) 
{
	//we don't really care what workspace we're running in, however, we need to
	//reset the size limits to match.
	BScreen Screen;
	SetSizeLimits(300,Screen.Frame().right,200,Screen.Frame().bottom);
}
void
Win::FrameMoved( BPoint origin )
{
	BWindow::FrameMoved( origin );
	
	if( urlpopupwindow != NULL )
	{
		BPoint point( navview->ConvertToScreen( navview->urlview->Frame().LeftBottom() ) );
		urlpopupwindow->MoveTo( point );
	}
}

void
Win::FrameResized( float width, float height)
{
	// calculate new tab width
	tabview->DynamicTabs( false );
	
	// enable or disable the newtab button
	if( ( tabview->CountTabs() * tabview->tab_width ) <= ( Bounds().right - 22 ) )
		navview->buttons[4]->SetEnabled( true );
	else
		navview->buttons[4]->SetEnabled( false );
	
	// resize urlpopupwindow
	if( urlpopupwindow != NULL )
	{
		urlpopupwindow->Lock();
		urlpopupwindow->ResizeToPrefered();
		urlpopupwindow->Unlock();
	}
}

void
Win::Zoom( BPoint origin, float width, float height )
{
	bool IM = false;
	AppSettings->FindBool( "IntelligentMaximize", &IM );
	
	if( IM == true )
	{
		if( fMaximized == false )
		{
			fOldFrame = Frame();
			
			BScreen screen;
			BDeskbar deskbar;
			BRect dbframe = deskbar.Frame();
		
			switch( deskbar.Location() )
			{
				case B_DESKBAR_TOP :
				{
					origin.y += dbframe.Height() + 2;
					break;
				}
				case B_DESKBAR_BOTTOM :
				{
					height -= dbframe.Height() + 2;
					break;
				}
				case B_DESKBAR_LEFT_BOTTOM :
				{
					origin.x += dbframe.Width() + 2;
					width -= dbframe.right + 2;
					break;
				}
				case B_DESKBAR_RIGHT_BOTTOM :
				{
					width = dbframe.left - origin.x - 7;
					break;
				}
				case B_DESKBAR_LEFT_TOP :
				{
					origin.x += dbframe.Width() + 2;
					width -= dbframe.right + 2;
					break;
				}
				case B_DESKBAR_RIGHT_TOP :
				{
					width = dbframe.left - origin.x - 7;
					break;
				}
			}
						
			fMaximized = true;
			
			BWindow::Zoom( origin, width, height );
		}
		else
		{
			origin = fOldFrame.LeftTop();
			width = fOldFrame.right - fOldFrame.left;
			height = fOldFrame.bottom - fOldFrame.top;
			
			fMaximized = false;
			
			BWindow::Zoom( origin, width, height );
		}
	}
	else
		BWindow::Zoom( origin, width, height );
}

void
Win::AddNewTab( bool hidden )
{
	//cout << "Win::AddNewTab()" << endl;
	
	Lock();
	
	// needed already here as DynamicTabs would stop if true
	// but we cannot call SetNormalTabView() here
	tabview->fake_single_view = false;
		
	tabview->DynamicTabs( true );
	
	ThemisTab* newtab = new ThemisTab( NULL, ( ( App* )be_app )->GetNewUniqueID() );
	
	tabview->AddTab( NULL, newtab );
	
	if( hidden == false )
		tabview->Select( tabview->CountTabs() - 1 );
	
	tabview->Draw( tabview->Bounds() );
	
	if( tabview->CountTabs() > 1 )
		tabview->SetNormalTabView();
		
	Unlock();
}

void
Win::CreateTabView()
{
	//cout << "Win::CreateTabView()" << endl;
	BRect rect = Bounds();
	
	tabview = new ThemisTabView(
		BRect(
			rect.left,
			rect.top + menubar->Bounds().Height() + 1 + navview->Bounds().Height() + 1,
			rect.right,
			rect.bottom - statusview->Bounds().Height() - 1 ),
		"THEMISTABVIEW", B_WIDTH_AS_USUAL, B_FOLLOW_ALL,
		B_FULL_UPDATE_ON_RESIZE |
		B_WILL_DRAW |
		B_NAVIGABLE_JUMP |
		B_FRAME_EVENTS |
		B_NAVIGABLE );
	AddChild( tabview );
	
	// add the first tab
	BMessenger* target = new BMessenger( this );
	target->SendMessage( TAB_ADD );
}

void
Win::CreateUrlPopUpWindow()
{
	if( urlpopupwindow == NULL )
	{
		BRect frame( navview->urlview->Frame() );
		frame = navview->ConvertToScreen( frame );
		
		BRect wframe( frame );
		wframe.top = frame.bottom;
		wframe.bottom = wframe.top + 30;
		
		urlpopupwindow = new ThemisUrlPopUpWindow(
			this,
			wframe );
		
		urlpopupwindow->Run();
		urlpopupwindow->Show();
		
		// make the urlview the focusview otherwise
		// the urlpopup wouldnt be navigable by keyb
		if( CurrentFocus() != NULL )
			CurrentFocus()->MakeFocus( false );
	 	navview->urlview->textview->MakeFocus( true );
	}
}

FakeSite*
Win::GetViewPointer( int16 tab_uid, int16 view_uid )
{
	cout << "Win::GetViewPointer() : looking for tab_uid: "
		<< tab_uid << " view_uid: " << view_uid << endl;
	
	ThemisTab* tmptab;
	FakeSite* tmpview;
	
	int32 count = tabview->CountTabs();
	
	for( int i=count-1; i >= 0; i-- )
	{
		tmptab = ( ThemisTab* )tabview->TabAt( i );
		if( tmptab != NULL )
		{
			if( tmptab->UniqueID() == tab_uid )
			{
				tmpview = ( FakeSite* )tmptab->View();
				if( tmpview != NULL )
				{
					if( tmpview->UniqueID() == view_uid )
					{
						return tmpview;
					}
				}
			}	
		}
	}
	
	cout << "Win::GetViewPointer() : no view found!" << endl;
	return NULL;
}

void
Win::LoadInterfaceGraphics()
{
	//cout << "Win::LoadInterfaceGraphics()" << endl;
	
	bool dir_exists = true;
	BString dirstr( "/boot/home/config/settings/Themis/interface/" );
	BDirectory dir( dirstr.String() );
	if( dir.InitCheck() != B_OK )
	{
		dir_exists = false;
		//cout << "/boot/home/config/settings/Themis/interface/ does not exist" << endl;
	}
	
	// load the icons either from bitmap-file or if not present, from internal
	// icon hexdumps located in "ThemisIcons.h"
	
	// initialize all bitmaps with NULL
	for( int i=0; i < 10; i++ )
		bitmaps[i] = NULL;
	
	// the array with file-names
	const char* names[9];
	// 4-pic bitmap
	names[0] = "button_back.png";
	names[1] = "button_forward.png";
	names[2] = "button_stop.png";
	names[3] = "button_home.png";
	names[4] = "button_newtab.png";
	// 3-pic bitmap
	names[5] = "button_reload.png";
	names[6] = "button_go.png";
	names[7] = "button_closetabview.png";
	// 1-pic bitmap
	names[8] = "icon_document.png";
	names[9] = "icon_document_empty.png";
	
	// the array with pointers to the hexdumps
	const unsigned char* hexp[10];
	// 4-pic bitmap
	hexp[0] = button_back_hex;
	hexp[1] = button_forward_hex;
	hexp[2] = button_stop_hex;
	hexp[3] = button_home_hex;
	hexp[4] = button_newtab_hex;
	// 3-pic bitmap
	hexp[5] = button_reload_hex;
	hexp[6] = button_go_hex;
	hexp[7] = button_closetabview_hex;
	// 1-pic bitmap
	hexp[8] = icon_document_hex;
	hexp[9] = icon_document_empty_hex;
	
	// a tempstring :D
	// ( i had to move this below the 2 char arrays .. otherwise i got a
	// seqm viol. and tstr was initialized with hexp[9] ..
	// dunno if i did some very lousy coding mistakes or if sth is playing me a trick )
	BString tstr( dirstr.String() );
		
	for( int i = 0; i < 10; i++ )
	{
		//cout << "bitmap " << i << ": ";
		if( dir_exists == true )
		{
			//cout << "loading from file" << endl;
			// load from file
			tstr.SetTo( dirstr.String() );
			if( ( bitmaps[i] = BTranslationUtils::GetBitmapFile( ( tstr += names[i] ).String() ) ) == 0 )
				cout << "loading from bitmap-file failed" << endl;
		}
		if( bitmaps[i] == NULL )
		{
			// load from hexdump
			//cout << "loading from hexdump" << endl;
			if( i <= 4 )
			{
				bitmaps[i] = new BBitmap( BRect( 0,0,63,15 ), B_RGB32 );
				memcpy( bitmaps[i]->Bits(), hexp[i], 4096 );
			}
			if( i >= 5 && i <= 7 )
			{
				bitmaps[i] = new BBitmap( BRect( 0,0,47,15 ), B_RGB32 );
				memcpy( bitmaps[i]->Bits(), hexp[i], 3072 );
			}
			if( i == 8 || i == 9 )
			{
				bitmaps[i] = new BBitmap( BRect( 0,0,15,15 ), B_RGB32 );
				memcpy( bitmaps[i]->Bits(), hexp[i], 1024 );
			}
		}
	}
}

Win*
Win::NextWindow()
{
	//cout << "Win::NextWindow()" << endl;
	if( fNextWindow != NULL )
	{
		//cout << "fNextWindow != NULL" << endl;
		return fNextWindow;
	}
	else
	{
		//cout << "returning NULL" << endl;	
		return NULL;
	}
}

void
Win::ReInitInterface()
{
	//cout << "Win:ReInitInterface()" << endl;
	
	union int32torgb convert;
	AppSettings->FindInt32( "PanelColor", &convert.value );
	
	if( CountChildren() > 0 )
	{
		for( int32 i = 0; i < CountChildren(); i++ )
		{
			BView* child = ChildAt( i );
			//set the new viewcolor, except for menubar
			if( strncmp( child->Name(), "MENUBAR", 7 ) != 0 )
				child->SetViewColor( convert.rgb );
			child->Draw( child->Bounds() );
		}
	}
}

void
Win::SetNextWindow( Win* nextwin )
{
	fNextWindow = nextwin;
}

void
Win::SetQuitConfirmed( bool state )
{
	fQuitConfirmed = state;
}

int16
Win::UniqueID()
{
	return fUniqueID;
}

void
Win::UrlTypedHandler( bool show_all )
{
	
	/////////////////
	// get the urls from somewhere .. i dunno from where :D
	// ( i have set up a const char* array here for convenience )
	////////////////
	
	const char* urls[18];
	urls[0] = "foo.foo.bar";
	urls[1] = "http://foo.foo.bar";
	urls[2] = "foo.foo.com";
	urls[3] = "foo.foo.de";
	urls[4] = "www.bebits.com";
	urls[5] = "http://www.bebits.com";
	urls[6] = "http://bebits.com";
	urls[7] = "www.beosjournal.org";
	urls[8] = "www.beunited.org";
	urls[9] = "www.bewicked.tk";
	urls[10] = "www.themisbrowser.org";
	urls[11] = "www.yellowtab.com";
	urls[12] = "www.xentronix.com";
	urls[13] = "http://www.xentronix.com";
	urls[14] = "ftp://ftp.be.com";
	urls[15] = "bebits.com";
	urls[16] = "beosjournal.org";
	urls[17] = "bewicked.tk";
			
	// create the BList with the matching urls
	BList* list = new BList();
	
	int i = 0;
	BString typed_url;					// the typed url
	BString cached_url;					// the cached url
	BString cached_url_proto( "" );		// protocol of the cached url
	
	if( show_all == true )
		typed_url.SetTo( "" );
	else
	{
		typed_url.SetTo( navview->urlview->Text() );
		typed_url.ToLower();
	}
	
	//cout << "type_durl: " << typed_url.String() << " length: " << typed_url.Length() << endl;
	
	while( i < 18 )
	{
		cached_url.SetTo( urls[i] );
		//cout << "-----------" << cached_url.String() << "--------" << endl;
		
		if( typed_url.Length() != 0 )
		{
			//cout << "compare strings: " << cached_url.String() << " <> " << typed_url.String() << endl;
			// if the typed url matches beginning of cached url, add it
			if( strncmp( cached_url.String(), typed_url.String(), typed_url.Length() ) == 0 )
			{
				//cout << "both urls fit -> adding url to list" << endl;
				list->AddItem( new BStringItem( cached_url.String() ) );
			}
			else
			{
				//cout << "urls don't fit" << endl;
				// if the urls dont match, take away the protocol of the cached url
				if( cached_url.FindFirst( "://" ) > 0 )
				{
					//cout << "removing proto of cached url" << endl;
					cached_url.MoveInto( cached_url_proto, 0, cached_url.FindFirst( "://" ) + 3 );
					//cout << "cached_url_proto: " << cached_url_proto.String() << endl;
					//cout << "cached_url: " << cached_url.String() << endl;
				}
				
				// if the urls fit now
				if( strncmp( cached_url.String(), typed_url.String(), typed_url.Length() ) == 0 )
				{
					//cout << "urls fit after removing protocol -> adding url to list" << endl;
					// add the missing proto again
					if( cached_url_proto.Length() != 0 )
						cached_url.Prepend( cached_url_proto );
						
					list->AddItem( new BStringItem( cached_url.String() ) );
				}
				else
				{
					//cout << "urls don't fit after removing protocol" << endl;
					// if they still don't fit, remove 'www.' from cached url
					if( cached_url.FindFirst( "www." ) == 0 )
					{
						//cout << "removing 'www.' from cached url" << endl;
						cached_url.Remove( 0, 4 );
						//cout << "cached_url: " << cached_url.String() << endl;
					}
					
					// check if they finally fit
					if( strncmp( cached_url.String(), typed_url.String(), typed_url.Length() ) == 0 )
					{
						//cout << "urls finally match without proto and 'www.' -> adding to url to list" << endl;
						// add missing 'www.' and proto
						cached_url.Prepend( "www." );
						
						if( cached_url_proto.Length() != 0 )
							cached_url.Prepend( cached_url_proto );
												
						list->AddItem( new BStringItem( cached_url.String() ) );
					}
				}
				cached_url_proto.SetTo( "" );
			}
		}
		else
		{
			list->AddItem( new BStringItem( cached_url.String() ) );
		}
		i++;
	}
	
	// add the urlpopupwindow if needed
	if( list->CountItems() > 0 )
	{
		CreateUrlPopUpWindow();
		// add the list
		urlpopupwindow->Lock();
		urlpopupwindow->ListToDisplay( list );
		urlpopupwindow->Unlock();
	}
	else
	{
		if( urlpopupwindow != NULL )
		{
			urlpopupwindow->Lock();
			urlpopupwindow->Quit();
			urlpopupwindow = NULL;
		}
	}
}
status_t Win::ReceiveBroadcast(BMessage *message) 
{
	printf( "Win::ReceiveBroadcast()\n" );
	message->PrintToStream();
	uint32 command=0;
	message->FindInt32("command",(int32*)&command);

	int16 win_uid = -1;
	if( message->HasInt16( "window_uid" ) )
	{
		message->FindInt16( "window_uid", &win_uid );
	}
					
	switch(command)
	{
		case COMMAND_INFO :
		{
			switch( message->what )
			{
				case ReturnedData :
				{
					if( win_uid == UniqueID() )
					{
						int16 tab_uid = 0;
						int16 view_uid = 0;
						int64 contentlength=0;
						int64 bytes_received=0;
						int64 delta=0;
						bool req_done = false;
						bool secure = false;	
						
						if( message->HasInt64( "content-length" ) )
							message->FindInt64( "content-length", &contentlength );
						
						message->FindInt16( "tab_uid", &tab_uid );
						message->FindInt16( "view_uid", &view_uid );
						message->FindInt64( "bytes-received", &bytes_received);
						message->FindInt64( "size-delta", &delta);
						message->FindBool( "request_done", &req_done );
						message->FindBool( "secure", &secure );
						
						BMessenger msgr( NULL, this, NULL);
						BMessage *msg = new BMessage( URL_LOADING );
						msg->AddBool( "request_done", req_done ); 
						msg->AddBool( "secure", secure );
						msg->AddInt16( "tab_uid", tab_uid );
						msg->AddInt16( "view_uid", view_uid );
						msg->AddInt64( "bytes-received", bytes_received );
						msg->AddInt64( "size-delta", delta );
						
						if( contentlength != 0 )	// non-chunked transfer mode
							msg->AddInt64("content-length",contentlength);
						
						msgr.SendMessage(msg);
						delete msg;
					}
					break;
				} // case ReturnedData
			} // switch( message->what )
			break;
		} // case COMMAND_INFO :
	} // switch( command )
}
