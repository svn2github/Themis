/*
Copyright (c) 2002 Raymond "Z3R0 One" Rodgers. All Rights Reserved. 

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

Original Author & Project Manager: Raymond "Z3R0 One" Rodgers (z3r0_one@yahoo.com)
Project Start Date: October 18, 2000
*/
#include <Alert.h>
#include <Directory.h>
#include <Screen.h>
#include <storage/FindDirectory.h>
#include <String.h>
#include "app.h"
#include "ThemisTab.h"
#ifndef NEWNET
#include "tcplayer.h"
tcplayer *TCP;
#else
#include <Path.h>
using namespace _Themis_Networking_;
#endif

plugman *PluginManager;
BMessage *AppSettings;

App::App(
	const char* appsig )
	: BApplication( appsig ), MessageSystem("Themis App Object")
{
	fQR_called = 0;
	fAboutWin = NULL;
	fPrefsWin = NULL;
	fIDCounter = 0;
	fUrlHandler = NULL;
	fLocker = new BLocker();
	
#ifndef NEWNET
	TCP=new tcplayer;
	TCP->Start();
#else
	TCPMan=new TCPManager;
	TCPMan->Start();
#endif

	fMessageDaemon=new MessageDaemon();
	MsgSysRegister(this);

	AppSettings=new BMessage;
	LoadSettings();

	app_info ai;
	GetAppInfo(&ai);
	entry_ref appdirref;
	BEntry *ent=new BEntry(&ai.ref);
	ent->GetParent(ent);
	ent->GetRef(&appdirref);
	delete ent;
	PluginManager=new plugman(appdirref);

	// init the GlobalHistory, and fill it with the settings data (if available)
	printf( "APP Getting GlobalHistoryData\n" );
	int8 ghdepth, freeurlcount;
	AppSettings->FindInt8( "GlobalHistoryDepthInDays", &ghdepth );
	AppSettings->FindInt8( "GlobalHistoryFreeUrlCount", &freeurlcount );
	fGlobalHistory = new GlobalHistory( ghdepth, freeurlcount );
	if( AppSettings->HasMessage( "GlobalHistoryData" ) )
	{
		BMessage* datamsg = new BMessage;
		AppSettings->FindMessage( "GlobalHistoryData", datamsg );
		if( datamsg != NULL )
			fGlobalHistory->Init( datamsg );
	}
	
	// init the first window
	BRect r;
	BScreen screen;
	AppSettings->FindRect( "WindowRect", &r );
	// if any of the sides is out of screen, reset the rect
	if( r.right > screen.Frame().right ||
			r.bottom > screen.Frame().bottom ||
			r.left < screen.Frame().left ||
			r.top < screen.Frame().top )
	{
		r.Set( 150, 100, 620, 460 );
		AppSettings->ReplaceRect( "WindowRect", r );
	}
	fFirstWindow = new Win(r,"Themis",B_DOCUMENT_WINDOW,B_ASYNCHRONOUS_CONTROLS,B_CURRENT_WORKSPACE);
	PluginManager->Window=fFirstWindow;
	BMessenger *msgr=new BMessenger(PluginManager,NULL,NULL);
	BMessage *msg=new BMessage(AddInitInfo);
#ifndef NEWNET
	msg->AddPointer("tcp_layer_ptr",TCP);
#else
	msg->AddPointer("tcp_manager",TCPMan);
#endif
	msg->AddPointer("settings_message_ptr",&AppSettings);
	{
		BMessage reply;	
		msgr->SendMessage(msg,&reply);
		if (reply.what==B_ERROR)
			printf("Problem setting plug-in initialization info.\n");
	}
	delete msgr;
	delete msg;
}

App::~App(){
	printf("app destructor\n");
	
	if( fLocker )
		delete fLocker;
	
	MsgSysUnregister(this);
	if (!fQR_called)
		QuitRequested();
	// delete GlobalHistory before AppSettings gets deleted
	delete fGlobalHistory;
	SaveSettings();
#ifdef NEWNET
	delete TCPMan;
	TCPMan=NULL;
#endif
	if (AppSettings!=NULL) {
		
		delete AppSettings;
		AppSettings=NULL;
	}
	printf("~app end\n");
	delete fMessageDaemon;
}

void App::AboutRequested()
{
	if (fAboutWin==NULL) {
		fAboutWin=new aboutwin(BRect(200,200,800,500),"About Themis",B_TITLED_WINDOW,B_ASYNCHRONOUS_CONTROLS|B_NOT_MINIMIZABLE);
	} else {
		fAboutWin->Activate(true);
	}
}

uint32 App::BroadcastTarget() 
{
	printf("App\n");
	return MS_TARGET_APPLICATION;
}

status_t App::ReceiveBroadcast(BMessage *msg) 
{
	printf("App::ReceiveBroadcast\n");
	return B_OK;
}

status_t App::BroadcastReply(BMessage *msg)
{
	return B_OK;
}

void App::Pulse() 
{
}

bool App::QuitRequested(){
	
	printf( "App::QuitRequested()\n" );
	printf("Message System Members:\n");
	int32 msgmem=CountMembers();
	for (int32 i=0; i<msgmem; i++)
		printf("\t%s\n",(GetMember(i))->MsgSysObjectName());
	SetPulseRate(0);
	atomic_add(&fQR_called,1);
	status_t stat;
	Win* w=NULL;
	BMessenger *msgr=NULL;
	thread_id th;
	printf( "CountWindows(): %d\n", CountWindows() );
	while (CountWindows()>0) {
		w=( Win* )WindowAt(0);
		if (w!=NULL) {
			printf("app: quit message target is Window \"%s\".\n",w->Title());
			w->SetQuitConfirmed( true );
			msgr=new BMessenger(NULL,w,NULL);
			th=w->Thread();
			msgr->SendMessage(B_QUIT_REQUESTED);
			printf("app: quit requested message sent, waiting for the window to die.\n");
			wait_for_thread(th,&stat);
			delete msgr;
		}
	}
	printf( "done closing windows\n" );
	
	// Delete the UrlHandler here, as it is unregistering from the cache system in its
	// destructor. This needs to be done before the PluginManager unloads all plugins.
	if( fUrlHandler != NULL )
		delete fUrlHandler;	
	
	if (PluginManager!=NULL) {
		msgr=new BMessenger(NULL,PluginManager,NULL);
		th=PluginManager->Thread();
		printf("App: Sending quit message to Plug-in Manager.\n");
		msgr->SendMessage(B_QUIT_REQUESTED);
		delete msgr;
		printf("App: Waiting for Plug-in manager to quit.\n");
		wait_for_thread(th,&stat);
		printf("Done.\n");
		PluginManager=NULL;
	}
#ifndef NEWNET
	if (TCP!=NULL) {
		printf( "telling TCP to quit\n" );
		TCP->lock->Lock();
		stat=TCP->Quit();
		TCP->lock->Unlock();
		delete TCP;
		TCP=NULL;
	}
#else
	if (TCPMan!=NULL) {
		printf("telling TCP Manager to quit\n");
		TCPMan->Quit();
	}
#endif
	
	printf( "end of App::QuitRequested()\n" );
	return true;
}

void App::MessageReceived(BMessage *msg){
	switch(msg->what){
		case ABOUTWIN_CLOSE :
		{
			printf( "APP ABOUTWIN_CLOSE\n" );
			fAboutWin = NULL;
			break;
		}
		case B_QUIT_REQUESTED :
		{
			printf( "APP B_QUIT_REQUESTED\n" );
			break;
		}
		case CLEAR_TG_HISTORY :
		{
			printf( "APP CLEAR_TG_HISTORY\n" );
			
			fGlobalHistory->Clear();			
			
			// tell every tab of every window to clear
			Win* win = FirstWindow();
			
			if( win == NULL )
				break;
			
			do
			{
				win->Lock();
				for( int32 i = 0; i < win->tabview->CountTabs(); i++ )
					( ( ThemisTab* )win->tabview->TabAt( i ) )->GetHistory()->Clear();
				win->tabview->SetNavButtonsByTabHistory();
				
				win->Unlock();
				win = win->NextWindow();
			}
			while( win != NULL );
			
			
			break;
		}
		case DTD_CHANGED :
		{
			printf( "APP DTD_CHANGED\n" );
			BMessage* chgmsg = new BMessage( DTD_CHANGED_PARSER );
			chgmsg->AddInt32( "command", COMMAND_INFO );
			Broadcast( MS_TARGET_PARSER, chgmsg );
			delete chgmsg;
			break;
		}
		case PREFSWIN_CLOSE :
		{
			printf( "APP PREFSWIN_CLOSE\n" );
			fPrefsWin = NULL;
			break;
		}
		case PREFSWIN_SHOW :
		{
			printf( "PREFSWIN_SHOW\n" );
			if( fPrefsWin == NULL)
			{
				BPoint point;
				AppSettings->FindPoint( "PrefsWindowPoint", &point );
				BRect rect;
				rect.left = point.x;
				rect.top = point.y;
				rect.right = rect.left + 500;
				rect.bottom = rect.top + 265;
				fPrefsWin = new prefswin(
					rect,
					"Preferences",
					B_TITLED_WINDOW_LOOK,
					B_NORMAL_WINDOW_FEEL,
					B_NOT_RESIZABLE | B_ASYNCHRONOUS_CONTROLS | B_NOT_MINIMIZABLE | B_NOT_ZOOMABLE );
			}
			else
				fPrefsWin->Activate(true);
				
			break;
		}
		case SAVE_APP_SETTINGS :
		{
			printf( "APP SAVE_APP_SETTINGS\n" );
			SaveSettings();
			break;
		}
		case WINDOW_CLOSE :
		{
			printf( "APP WINDOW_CLOSE\n" );
			Win* closewin;
			msg->FindPointer( "win_to_close", ( void** )&closewin );
			printf( "mainwindowcount: %d\n", GetMainWindowCount() );
			
			if( closewin == NULL )
			{
				printf( "closewin == NULL; aborting!\n" );
				break;
			}
						
			if( GetMainWindowCount() == 1 )
			{
				printf( "CountWindows() == 1\n" );
				closewin->SetQuitConfirmed( true );
				printf( "sending B_QUIT_REQUESTED to app\n" );
				be_app_messenger.SendMessage( B_QUIT_REQUESTED );
				break;
			}
			else
			{
				if( closewin == FirstWindow() )
				{
					printf( "closewin == FirstWindow()\n" );
					SetFirstWindow( closewin->NextWindow() );
				}
				else
				{
					// find the ancesstor window
					printf( "finding antecessor window\n" );
					Win* awin = FirstWindow();
				
					while( awin->NextWindow() != closewin )
					{
						awin = awin->NextWindow();
						if( awin == NULL )
						{	
							printf( "awin == NULL; aborting\n" );
							break;
						}
					}
					
					if( awin->NextWindow() == closewin )
						printf( "found antecessor window!\n" );
					else
					{
						printf( "error finding antecessor window; aborting!\n" );
						break;
					}
				
					// find the successor window
					printf( "finding successor window\n" );
					Win* swin = NULL;
					if( closewin->NextWindow() != NULL )
					{
						swin = closewin->NextWindow();
						printf( "found successor window!\n" );
					}
					else
						printf( "no successor window\n" );
				
					// combine antecessor win and successor win
					printf( "combining awin and swin\n" );
					
					if( swin != NULL )
						awin->SetNextWindow( swin );
					else
						awin->SetNextWindow( NULL );
				}
								
				// quit the to be closed window
				printf( "closing closewin\n" );
				closewin->SetQuitConfirmed( true );
				BMessenger msgr( closewin );
				msgr.SendMessage( B_QUIT_REQUESTED );
								
				break;
			}
		}
		case WINDOW_NEW :
		{
			printf( "APP WINDOW_NEW\n" );
			
			// find the last win
			printf( "finding last window\n" );
			Win* lastwin = FirstWindow();
			while( lastwin->NextWindow() != NULL )
			{
				lastwin = lastwin->NextWindow();
			}
			
			if( lastwin != NULL )
				printf( "found lastwin\n" );
			else
			{
				printf( "lastwin not found; aborting!\n" );
				break;
			}
									
			BRect r = lastwin->Frame();
			r.OffsetBySelf( 20.0, 20.0 );
			
			float minw = 0.0, minh = 0.0, maxw = 0.0, maxh = 0.0;
			FirstWindow()->GetSizeLimits( &minw, &maxw, &minh, &maxh );
			
			r.PrintToStream();
					
			// outside screen?
			BScreen screen;
			screen.Frame().PrintToStream();
			bool usingsettingsrect = false;
			if( r.right > screen.Frame().right - 30 )
			{
				// check if we can resize it smaller, but that its still above its minimum size
				if( minw < ( r.right - r.left ) )
					r.right = screen.Frame().right - 30;
				else
				{
					printf( "width: using settings rect\n" );
					AppSettings->FindRect( "WindowRect", &r );
					usingsettingsrect = true;
				}
			}
			if( usingsettingsrect == false )
			{
				if( r.bottom > screen.Frame().bottom - 30 )
				{
					if( minh < ( r.bottom - r.top ) )
						r.bottom = screen.Frame().bottom - 30;
					else
						AppSettings->FindRect( "WindowRect", &r );
				}
			}
				
			Win* newwin = new Win(r,"Themis",B_DOCUMENT_WINDOW,B_ASYNCHRONOUS_CONTROLS,B_CURRENT_WORKSPACE);
			
			lastwin->SetNextWindow( newwin );			
			
			if( lastwin->NextWindow() != NULL )
			{
				// get the current windows url ( for case 2 )
				BString currenturl;
				for( int32 i = 0; i < GetMainWindowCount(); i++ )
				{
					if( WindowAt( i )->IsActive() == true )
					{
						Win* win = ( Win* )WindowAt( i );
						currenturl.SetTo( win->navview->urlview->Text() );
						break;
					}
				}
								
				printf( "current url: %s\n", currenturl.String() );
				
				printf( "successfully added new win; showing now\n" );
				lastwin->NextWindow()->Show();
				
				// open with blank, home or current page
				int8 val;
				AppSettings->FindInt8( "NewWindowStartPage", &val );
				printf( "VALUE: %d\n", val );
				switch( val )
				{
					case 0 : break;	// blank, do nothing
					case 1 : // homepage
					{
						BMessenger msgr( FirstWindow() );
						BMessage* homepage = new BMessage( URL_OPEN );
						BString string;
						AppSettings->FindString( "HomePage", &string );
						homepage->AddString( "url_to_open", string.String() );
						msgr.SendMessage( homepage );
						delete homepage;
						break;
					}
					case 2 : // current windows page
					{
						BMessenger msgr( lastwin->NextWindow() );
						BMessage* currentpage = new BMessage( URL_OPEN );
						currentpage->AddString( "url_to_open", currenturl.String() );
						msgr.SendMessage( currentpage );
						delete currentpage;						
						break;
					}
				}
				
			}
			else
				printf( "not successfully added window\n" );
						
			break;
		}
		default:{
			BApplication::MessageReceived(msg);
		}
	}
}

void App::RefsReceived(BMessage *refs){
	printf("RefsReceived\n");
	//we need to tell the currently active window and tab to load the
	//the file from disk, assuming that the file protocol add-on is in
	//memory...
}

void App::ReadyToRun(){
	printf( "APP READYTORUN\n" );

	fUrlHandler = new UrlHandler();

	PluginManager->BuildRoster(true);
	fFirstWindow->Show();
	//SetPulseRate(2000000);
	
	// if we have a homepage specified in prefs
	BMessenger msgr( FirstWindow() );
	BMessage* homepage = new BMessage( URL_OPEN );
	BString string;
	AppSettings->FindString( "HomePage", &string );
	homepage->AddString( "url_to_open", string.String() );
	msgr.SendMessage( homepage );
}

void App::ArgvReceived(int32 argc, char **argv){
}

void App::InitSettings(char *settings_path) {
	printf( "App::InitSettings()\n" );
	if (settings_path!=NULL) {
		AppSettings->AddString("settings_directory",settings_path);
		BString name(settings_path);
		name+="/Themis Settings";
		AppSettings->AddString("settings_file",name.String());
	} else {
		AppSettings->AddString("settings_directory","/boot/home/config/settings/Themis/");
		AppSettings->AddString("settings_file","/boot/home/config/settings/Themis/Themis Settings");
	}
	
	// add the remaining prefs
	
	BRect rect( 100,100,650,450 );
	AppSettings->AddRect( "WindowRect", rect );
	BPoint point( 200, 200 );
	AppSettings->AddPoint( "PrefsWindowPoint", point );
		
	// window
	AppSettings->AddString( "HomePage", BString( "about:blank" ) );
	AppSettings->AddBool( "IntelligentMaximize", false );
	AppSettings->AddBool( "ShowTypeAhead", false );
	AppSettings->AddInt8( "NewWindowStartPage", 0 );
	
	// tabs
	AppSettings->AddBool( "ShowTabsAtStartup", false );
	AppSettings->AddBool( "OpenTabsInBackground", false );
	AppSettings->AddInt8( "TabHistoryDepth", 10 );
	
	// fonts
	
	// colors
	union int32torgb convert;
//	convert.rgb = ui_color( B_MENU_BACKGROUND_COLOR );
//	AppSettings->AddInt32( "MenuColor", convert.value );
	convert.rgb = ui_color( B_PANEL_BACKGROUND_COLOR );
	AppSettings->AddInt32( "PanelColor", convert.value );
	convert.rgb.red = 255;
	convert.rgb.green = 200;
	convert.rgb.blue = 0;
	AppSettings->AddInt32( "ThemeColor", convert.value );
	convert.rgb = ui_color( B_PANEL_BACKGROUND_COLOR );
	AppSettings->AddInt32( "ActiveTabColor", convert.value );
	convert.rgb.red -=32;
	convert.rgb.green -=32;
	convert.rgb.blue -=32;
	AppSettings->AddInt32( "InactiveTabColor", convert.value );
	convert.rgb.red = 187;
	convert.rgb.green = 187;
	convert.rgb.blue = 187;
	AppSettings->AddInt32( "LightBorderColor", convert.value );
	convert.rgb.red = 51;
	convert.rgb.green = 51;
	convert.rgb.blue = 51;
	AppSettings->AddInt32( "DarkBorderColor", convert.value );
	convert.rgb.red = 240;
	convert.rgb.green = 240;
	convert.rgb.blue = 240;
	AppSettings->AddInt32( "ShadowColor", convert.value );
	
	// privacy
	AppSettings->AddInt8( "GlobalHistoryDepthInDays", 7 );
	AppSettings->AddInt8( "GlobalHistoryFreeUrlCount", 50 );
	
	// HTML Parser
	// set the DTDToUsePath to "none", as we may not find a DTD below
	AppSettings->AddString( "DTDToUsePath", "none" );
	
	// find a DTD
	BString dtddir;
	AppSettings->FindString( "settings_directory", &dtddir );
	dtddir.Append( "/dtd/" );
	printf( "DTD dir: %s\n", dtddir.String() );
				
	BDirectory* dir = new BDirectory( dtddir.String() );
	if( dir->InitCheck() != B_OK )
	{
		printf( "DTD directory (%s) not found!\n", dtddir.String() );
		printf( "Setting DTDToUsePath to \"none\"\n" );
		AppSettings->AddString( "DTDToUsePath", "none" );
	}
	else
	{
		BEntry entry;
		while( dir->GetNextEntry( &entry, false ) != B_ENTRY_NOT_FOUND )
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
				if( AppSettings->HasString( "DTDToUsePath" ) )
				{
					printf( "replacing DTDToUsePath with: %s\n", path.Path() );
					AppSettings->ReplaceString( "DTDToUsePath", path.Path() );
				}
				else
				{
					printf( "adding DTDToUsePath: %s\n", path.Path() );
					AppSettings->AddString( "DTDToUsePath", path.Path() );
				}
			}
		}
	}
	delete dir;
	// end: find a DTD
	
	AppSettings->PrintToStream();
}

GlobalHistory*
App::GetGlobalHistory()
{
	return fGlobalHistory;
}

int32
App::GetMainWindowCount()
{
	printf( "App::GetMainWindowCount()\n" );
	
	int32 count = 0;
	
	Win* win = FirstWindow();
	if( win == NULL )
		return 0;
	else
		count++;
	
	while( win->NextWindow() != NULL )
	{
		win = win->NextWindow();
		count++;
	}
	
	printf( "Number of main windows: %li\n", count );
	
	return count;
}

int32
App::GetNewID()
{
	fIDCounter += 1;
		
	// i innerly hope that nobody will ever reach the id limit,
	// who still has the first site open :)
	if( fIDCounter > 2000000000 )
	{
		fIDCounter = 0;
		BString astring;
		astring <<
		"Congratulations!\n\nYou are a hardcore Themis user!\n\n" <<
		"Unfortunately, you have reached the unique indexing limit. " <<
		"Themis could behave inconsistent from now on. You've been warned! :)";
		BAlert* alert = new BAlert(	"Long Time Usage", astring.String(), "OK" );
		alert->Go();
	}		
	
	return fIDCounter;
}

//TRenderView*
//App::GetRenderViewFor(
//	int32 id )
//{
//	/*
//	 * Cycle through every window, searching every tabview/tab for the given
//	 * ID, and return a pointer to this view. Otherwise, return NULL.
//	 */
//	
//	fLocker->Lock();
//	
//	
//	fLocker->Unlock();
//	return NULL;
//}

UrlHandler*
App::GetUrlHandler()
{
	if( fUrlHandler )
		return fUrlHandler;
	else
		return NULL;
}

Win*
App::FirstWindow()
{
	return fFirstWindow;
}

status_t App::LoadSettings() {
	printf( "App::LoadSettings()\n" );
	if (AppSettings!=NULL) {
		{
			BEntry ent("/boot/home/config/settings/Themis/dtd/",true);
			if (!ent.Exists())
				create_directory("/boot/home/config/settings/Themis/dtd/",0555);
		}
		status_t ret=B_OK;
		BPath path;
		if (find_directory(B_USER_SETTINGS_DIRECTORY,&path)==B_OK) {
			path.Append("Themis/",true);
			BEntry prefsent(path.Path(),true);
			if (prefsent.Exists()) {
				prefsent.GetPath(&path);
				path.Append("Themis Settings",false);
				prefsent.SetTo(path.Path(),true);
				if (prefsent.Exists()) {
					BFile *file=new BFile(&prefsent,B_READ_ONLY);
					ret=file->InitCheck();
					if (ret!=B_OK) {
						delete file;
						InitSettings();
						return ret;
					}
					
					file->Lock();
					ret|=AppSettings->Unflatten(file);
					file->Unlock();
					delete file;
					printf("Settings loaded.\n");
					// check for missing pref-vars
					printf( "Checking for missing pref-vars in AppSettings.\n" );
					if( !AppSettings->HasRect( "WindowRect" ) )
						AppSettings->AddRect( "WindowRect", BRect( 100,100,650,450 ) );
					if( !AppSettings->HasPoint( "PrefsWindowPoint" ) )
						AppSettings->AddPoint( "PrefsWindowPoint", BPoint( 200,200 ) );
					// window
					if( !AppSettings->HasString( "HomePage" ) )
						AppSettings->AddString( "HomePage", BString( "about:blank" ) );
					if( !AppSettings->HasBool( "IntelligentMaximize" ) )
						AppSettings->AddBool( "IntelligentMaximize", false );
					if( !AppSettings->HasBool( "ShowTypeAhead" ) )
						AppSettings->AddBool( "ShowTypeAhead", false );
					if( !AppSettings->HasInt8( "NewWindowStartPage" ) )
						AppSettings->AddInt8( "NewWindowStartPage", 0 );
					// tabs
					if( !AppSettings->HasBool( "ShowTabsAtStartup" ) )
						AppSettings->AddBool( "ShowTabsAtStartup", false );
					if( !AppSettings->HasBool( "OpenTabsInBackground" ) )
						AppSettings->AddBool( "OpenTabsInBackground", false );
					if( !AppSettings->HasInt8( "TabHistoryDepth" ) )
						AppSettings->AddInt8( "TabHistoryDepth", 10 );
					// fonts
					// colors
					union int32torgb convert;
//					if( !AppSettings->HasInt32( "MenuColor" ) )
//					{
//						convert.rgb = ui_color( B_MENU_BACKGROUND_COLOR );
//						AppSettings->AddInt32( "MenuColor", convert.value );
//					}
					if( !AppSettings->HasInt32( "PanelColor" ) )
					{
						convert.rgb = ui_color( B_PANEL_BACKGROUND_COLOR );
						AppSettings->AddInt32( "PanelColor", convert.value );
					}
					if( !AppSettings->HasInt32( "ThemeColor" ) )
					{
						convert.rgb.red = 255;
						convert.rgb.green = 200;
						convert.rgb.blue = 0;
						AppSettings->AddInt32( "ThemeColor", convert.value );
					}
					if( !AppSettings->HasInt32( "ActiveTabColor" ) )
					{
						convert.rgb = ui_color( B_PANEL_BACKGROUND_COLOR );
						AppSettings->AddInt32( "ActiveTabColor", convert.value );
					}
					if( !AppSettings->HasInt32( "InactiveTabColor" ) )
					{
						convert.rgb = ui_color( B_PANEL_BACKGROUND_COLOR );
						convert.rgb.red -=32;
						convert.rgb.green -=32;
						convert.rgb.blue -=32;
						AppSettings->AddInt32( "InactiveTabColor", convert.value );						
					}
					if( !AppSettings->HasInt32( "LightBorderColor" ) )
					{
						convert.rgb.red = 187;
						convert.rgb.green = 187;
						convert.rgb.blue = 187;
						AppSettings->AddInt32( "LightBorderColor", convert.value );
					}
					if( !AppSettings->HasInt32( "DarkBorderColor" ) )
					{
						convert.rgb.red = 51;
						convert.rgb.green = 51;
						convert.rgb.blue = 51;
						AppSettings->AddInt32( "DarkBorderColor", convert.value );
					}
					if( !AppSettings->HasInt32( "ShadowColor" ) )
					{
						convert.rgb.red = 240;
						convert.rgb.green = 240;
						convert.rgb.blue = 240;
						AppSettings->AddInt32( "ShadowColor", convert.value );
					}
					// privacy
					if( !AppSettings->HasInt8( "GlobalHistoryDepthInDays" ) )
						AppSettings->AddInt8( "GlobalHistoryDepthInDays", 7 );
					if( !AppSettings->HasInt8( "GlobalHistoryFreeUrlCount" ) )
						AppSettings->AddInt8( "GlobalHistoryFreeUrlCount", 50 );
					
					// HTML Parser
					// if we have no DTDToUsePath, or the DTDToUsePath is "none"
					// ( checking for "none" because somebody may have added a DTD in the mean time )
					if( !AppSettings->HasString( "DTDToUsePath" ) || strncmp( AppSettings->FindString( "DTDToUsePath" ), "none", 4 ) == 0 )
					{
						// set the DTDToUsePath to "none", as we may not find a DTD below
						if( !AppSettings->HasString( "DTDToUsePath" ) )
							AppSettings->AddString( "DTDToUsePath", "none" );
	
						// find a DTD
						BString dtddir;
						AppSettings->FindString( "settings_directory", &dtddir );
						dtddir.Append( "/dtd/" );
						printf( "DTD dir: %s\n", dtddir.String() );
			
						BDirectory* dir = new BDirectory( dtddir.String() );
						if( dir->InitCheck() != B_OK )
						{
							printf( "DTD directory (%s) not found!\n", dtddir.String() );
							printf( "Setting DTDToUsePath to \"none\"\n" );
							AppSettings->AddString( "DTDToUsePath", "none" );
						}
						else
						{
							BEntry entry;
							while( dir->GetNextEntry( &entry, false ) != B_ENTRY_NOT_FOUND )
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
									if( AppSettings->HasString( "DTDToUsePath" ) )
									{
										printf( "replacing DTDToUsePath with: %s\n", path.Path() );
										AppSettings->ReplaceString( "DTDToUsePath", path.Path() );
									}
									else
									{
										printf( "adding DTDToUsePath: %s\n", path.Path() );
										AppSettings->AddString( "DTDToUsePath", path.Path() );
									}
								}
							}
						}
						delete dir;
						// end: find a DTD
					}
						
					AppSettings->PrintToStream();
					return ret;
					
				} else {
					path.GetParent(&path);
					InitSettings((char*)path.Path());
					SaveSettings();
					return B_OK;
				}
				
			} else {
				create_directory(path.Path(),0777);
				InitSettings((char*)path.Path());
				SaveSettings();
				
				return B_OK;
				
			}
			
		} else {
		}
		return ret;
	}
	return B_ERROR;
	
}

status_t App::SaveSettings() {
	if (AppSettings!=NULL) {
		
		status_t ret=B_OK;
		
		BString fname,dname;
		AppSettings->FindString("settings_directory",&dname);
		AppSettings->FindString("settings_file",&fname);
		BEntry ent(dname.String());
		if (!ent.Exists()) {
			ret=create_directory(dname.String(),0777);
			if (ret!=B_OK) {
				printf("Couldn't create Themis settings directory.\n");
				return ret;
			}
			
		}
		BFile *file=new BFile(fname.String(),B_CREATE_FILE|B_ERASE_FILE|B_WRITE_ONLY);
		ret=file->InitCheck();
		if (ret!=B_OK) {
			printf("Unable to create Themis settings file\n");
			delete file;
			return ret;
		}
		
		file->Lock();
		ret=AppSettings->Flatten(file);
		file->Unlock();
		delete file;
		
		return ret;
	}
	return B_ERROR;
}

void
App::SetFirstWindow( Win* newfirst )
{
	fFirstWindow = newfirst;
}
