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
#include "win.h"
#include "commondefs.h"
#include "protocol_plugin.h"
#include "AppDefs.h"
#include "iostream.h"
#include "ThemisTab.h"
#include <Directory.h>
#include <Screen.h>
#include <ctype.h>
#include "ThemisIcons.h"
#include <TranslationKit.h>

int WinH=800;
int WinW=600;

extern plugman *PluginManager;
extern BMessage *AppSettings;
Win::Win(BRect frame,const char *title,window_type type,uint32 flags,uint32 wspace)
    :BWindow(frame,title,type,flags,wspace) {
    protocol=0;
//  View=new winview(Bounds(),"mainview",B_FOLLOW_ALL,B_FRAME_EVENTS|B_WILL_DRAW|B_ASYNCHRONOUS_CONTROLS|B_NAVIGABLE_JUMP|B_FULL_UPDATE_ON_RESIZE);
	startup=true;
//  Parser=new HTMLParser;
//  Parser->View=View;
//  View->Parser=Parser;
//	AddShortcut('a',B_SHIFT_KEY,(new BMessage(B_ABOUT_REQUESTED)));
//	BScreen Screen;
	
//	SetSizeLimits(550,Screen.Frame().right,350,Screen.Frame().bottom);
//	AddChild(View);
	urlpopupwindow = NULL;
	
	LoadInterfaceGraphics();
		
	// size limits
	BScreen Screen;
	SetSizeLimits( 300, Screen.Frame().right, 200, Screen.Frame().bottom );
	
	BRect rect;
	rect = Bounds();
	
	// lets add a menubar
	menubar = new BMenuBar( BRect(0,0,0,0), "MENUBAR" );
	AddChild( menubar );
	
	filemenu = new BMenu( "File" );
	menubar->AddItem( filemenu );
	optionsmenu=new BMenu("Options");
	menubar->AddItem(optionsmenu);
		filemenu->AddItem((new BMenuItem("About Themis",(new BMessage(B_ABOUT_REQUESTED)),'a',B_SHIFT_KEY)));
	BMenuItem* quitentry = new BMenuItem( "Quit", new BMessage( B_QUIT_REQUESTED ), 'Q' );
	filemenu->AddItem( quitentry );
	quitentry->SetTarget( this );
		
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
			rect.right - 13, // width of the lower-right doc-view corner
			rect.bottom ) );
	AddChild( statusview );
	
	// the tabview-system comes last as we need the height of the statusview
	CreateTabView();
	
	// make the urlview the focusview for fast pasting :D
	if( CurrentFocus() != NULL )
		CurrentFocus()->MakeFocus( false );
	navview->urlview->TextView()->MakeFocus( true );
	BMessage *info=new BMessage(AddInitInfo);
				info->AddPointer("main_menu_bar",menubar);
				info->AddPointer("file_menu",filemenu);
				info->AddPointer("options_menu",optionsmenu);
				info->AddPointer("window",this);
//				info->AddPointer("parser",Parser);
				BMessenger *iimsgr=new BMessenger(PluginManager,NULL,NULL);
		{
		BMessage reply;	
		iimsgr->SendMessage(info,&reply);
		info->PrintToStream();
		printf("Window has tried to init: %ld\n",reply.what);
			
		reply.PrintToStream();
			
		}
		
		delete iimsgr;
		delete info;
}
bool Win::QuitRequested() {
//  BMessenger *msgr=new BMessenger(NULL,Parser,NULL);
 // msgr->SendMessage(B_QUIT_REQUESTED);
 // delete msgr;
 	if (AppSettings->HasRect("main_window_rect"))
		AppSettings->ReplaceRect("main_window_rect",Frame());
	else
		AppSettings->AddRect("main_window_rect",Frame());
	return true;
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
			if( urlpopupwindow  )
			{
				urlpopupwindow->Lock();
				urlpopupwindow->Quit();
				urlpopupwindow = NULL;
			}			
				
		}break;
		case TAB_ADD :
		{
			cout << "TAB_ADD received" << endl;
			
			// if the prefs are not set to sth like "open new tabs hidden"
			// we pass hidden = false to AddNewTab
			// this selects the last tab ( the new one )
			//bool hidden = false;
			bool hidden = true;
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
			
			
		}	break;
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
			
			// taken out for now
			// ( too lazy to type in a url for testing everytime )
			//if( url.Length() == 0 )
			//	break;
			
			// create the fakesite ( later the renderview )
			FakeSite* fakesite = new FakeSite(
				( tabview->ContainerView() )->Bounds(),
				url.String(), this );
			
			uint32 selection = tabview->Selection();
						
			if( msg->HasInt32( "tab_to_open_in" ) )
				tabview->TabAt( msg->FindInt32( "tab_to_open_in" ) )->SetView( fakesite );
			else
				tabview->TabAt( selection )->SetView( fakesite );
						
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
			
//			if (pobj!=NULL) {
				BMessage *info=new BMessage;
//				info->AddPointer("tcp_layer_ptr",TCP);
				info->AddPointer("top_view",tabview->TabAt( selection )->View());
				info->AddPointer("window",this);
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
//				PlugClass *cache=(PlugClass*)PluginManager->FindPlugin(CachePlugin);
//				if (cache!=NULL) {
					printf("telling cache that a new page is being loaded.\n");
					BMessage *bcast=new BMessage;
					BMessage *lnp=new BMessage(LoadingNewPage);
					lnp->AddInt32("command",COMMAND_INFO);
					bcast->AddMessage("message",lnp);
					delete lnp;
					PluginManager->Broadcast(TARGET_VIEW,ALL_TARGETS,bcast);
					delete bcast;
//					BMessenger *msgr=new BMessenger(cache->Handler(),NULL,NULL);
//					msgr->SendMessage(LoadingNewPage);
//					delete msgr;
//				}
				bcast=new BMessage(BroadcastMessage);
				info->AddInt32("command",COMMAND_RETRIEVE);
				bcast->AddMessage("message",info);
				bcast->AddInt32("targets",TARGET_PROTOCOL);
				bcast->AddInt32("source",TARGET_VIEW);
			
				BMessenger *msgr=new BMessenger(NULL,PluginManager,NULL);
				msgr->SendMessage(bcast);
				delete msgr;
			
			//	PluginManager->Broadcast(TARGET_VIEW,TARGET_PROTOCOL,bcast);
				delete bcast;
				delete info;
			
//				pobj->GetURL(info);
//			}
			
		}break;
		case URL_TYPED :
		{
			cout << "URL_TYPED received" << endl;
			
			// imaginary prefs
			bool prefs_open_type_ahead = true;
			
			if( prefs_open_type_ahead == true || msg->FindBool( "open" ) == true )
				UrlTypedHandler();
			
			
		}break;
		default: {
			BWindow::MessageReceived(msg);
		}
	}
//	Unlock();
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
	SetSizeLimits(550,Screen.Frame().right,350,Screen.Frame().bottom);
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
	
	// resize urlpopupwindow
	if( urlpopupwindow != NULL )
	{
		urlpopupwindow->Lock();
		urlpopupwindow->ResizeToPrefered();
		urlpopupwindow->Unlock();
	}
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
	
	ThemisTab* newtab = new ThemisTab( NULL );
	
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
	
	// if we have a sth like a startup page in the prefs
	//BMessage* firstsite = new BMessage( URL_OPEN );
	//firstsite->AddString( "url_to_open", "my.startup.site" );
	//target->SendMessage( firstsite );
	
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
		// it wont be navigable with keyboard, better
		// the urlpopup wouldnt be navigable by keyb
		if( CurrentFocus() != NULL )
			CurrentFocus()->MakeFocus( false );
		navview->urlview->textview->MakeFocus( true );
	}
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
	// icon hexdumpslocated in "ThemisIcons.h"
	// draw those bitmaps into BPictures and then create BPictureButtons

	// initialize all bitmaps with NULL
	for( int i=0; i < 10; i++ )
		bitmaps[i] = NULL;
	
	// the array with file-names
	const char* names[9];
	// 3-icon bitmap
	names[0] = "button_back.png";
	names[1] = "button_forward.png";
	names[2] = "button_stop.png";
	names[3] = "button_reload.png";
	names[4] = "button_home.png";
	names[5] = "button_go.png";
	// 2-icon bitmap
	names[6] = "button_newtab.png";
	names[7] = "button_closetabview.png";
	// 1-icon bitmap
	names[8] = "icon_document.png";
	names[9] = "icon_document_empty.png";
	
	// the array with pointers to the hexdumps
	const unsigned char* hexp[10];
	// 3-icon bitmap
	hexp[0] = button_back_hex;
	hexp[1] = button_forward_hex;
	hexp[2] = button_stop_hex;
	hexp[3] = button_reload_hex;
	hexp[4] = button_home_hex;
	hexp[5] = button_go_hex;
	// 2-icon bitmap
	hexp[6] = button_newtab_hex;
	hexp[7] = button_closetabview_hex;
	// 1-icon bitmap
	hexp[8] = icon_document_hex;
	hexp[9] = icon_document_empty_hex;
	
	// a tempstring :D
	// ( i had to move this below the 2 char arrays .. otherwise i got a
	// seqm viol. and tstr was initialized with hexp[9] ..
	// dunno if i did some very lousy coding mistakes or not )
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
			if( i <= 5 )
			{
				bitmaps[i] = new BBitmap( BRect( 0,0,47,15 ), B_RGB32 );
				memcpy( bitmaps[i]->Bits(), hexp[i], 3072 );
			}
			if( i == 6 || i == 7 )
			{
				bitmaps[i] = new BBitmap( BRect( 0,0,31,15 ), B_RGB32 );
				memcpy( bitmaps[i]->Bits(), hexp[i], 2048 );
			}
			if( i == 8 || i == 9 )
			{
				bitmaps[i] = new BBitmap( BRect( 0,0,15,15 ), B_RGB32 );
				memcpy( bitmaps[i]->Bits(), hexp[i], 1024 );
			}
		}
	}
}

void
Win::UrlTypedHandler()
{
	
	/////////////////
	// get the urls from somewhere .. i dunno from where :D
	// ( i have a const char* array set up here for convenience )
	////////////////
	
	const char* urls[15];
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
	
			
	// create the BList with the matching urls
	BList* list = new BList();
	
	////////////////
	// need to refine some things here for example if i type a protocol and stuff
	// but i am tired of coding for today
	// its far from being complete imho
	// but enough for now..think i'll send you the code right now raymond
	////////////////
		
	int i = 0;
	BString url;
	//BString proto;
	BString typedurl( navview->urlview->Text() );
	//cout << "typedurl: " << typedurl.String() << " length: " << typedurl.Length() << endl;
	while( i < 15 )
	{
		url.SetTo( urls[i] );
		if( typedurl.Length() != 0 )
		{
			if( strncmp( url.String(), typedurl.String(), typedurl.Length() ) == 0 )
			{
				if( url.Length() > typedurl.Length() )
					list->AddItem( new BStringItem( url.String() ) );
			}
		}
		else
		{
			list->AddItem( new BStringItem( url.String() ) );
		}
		i++;
	}
	
	// add the urlpopupwindow if needed
	if( list->CountItems() > 0 )
		CreateUrlPopUpWindow();
	else
	{
		if( urlpopupwindow != NULL )
		{
			urlpopupwindow->Lock();
			urlpopupwindow->Quit();
			urlpopupwindow = NULL;
		}
	}
	
	// add the list
	if( list->CountItems() > 0 )
	{
		urlpopupwindow->Lock();
		urlpopupwindow->ListToDisplay( list );
		urlpopupwindow->Unlock();
	}
}
