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
#include "PrefsDefs.h"
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
	fSiteHandler = NULL;
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
	AppSettings->FindInt8( kPrefsGlobalHistoryDepthInDays, &ghdepth );
	AppSettings->FindInt8( kPrefsGlobalHistoryFreeURLCount, &freeurlcount );
	fGlobalHistory = new GlobalHistory( ghdepth, freeurlcount );
	if( AppSettings->HasMessage( kPrefsGlobalHistoryData ) )
	{
		BMessage* datamsg = new BMessage;
		AppSettings->FindMessage( kPrefsGlobalHistoryData, datamsg );
		if( datamsg != NULL )
			fGlobalHistory->Init( datamsg );
	}
	
	// init the first window
	BRect r;
	BRect screenrect;
	BScreen screen;
	screenrect = screen.Frame();
	AppSettings->FindRect( kPrefsMainWindowRect, &r );
	// if any of the sides is out of screen, reset the rect
	if( !screenrect.Contains( r ) )
//	if( r.right > screen.Frame().right ||
//			r.bottom > screen.Frame().bottom ||
//			r.left < screen.Frame().left ||
//			r.top < screen.Frame().top )
	{
		r.Set( 130, 130, 650, 450 );
		AppSettings->ReplaceRect( kPrefsMainWindowRect, r );
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
//	printf("App\n");
	return MS_TARGET_APPLICATION;
}

status_t App::ReceiveBroadcast(BMessage *msg) 
{
//	printf("App::ReceiveBroadcast\n");
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
	
	// Delete the SiteHandler here, as it is unregistering from the cache system in its
	// destructor. This needs to be done before the PluginManager unloads all plugins.
	if( fSiteHandler != NULL )
		delete fSiteHandler;	
	
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
	fMessageDaemon->Stop();
	
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
			SaveSettings();
			fPrefsWin = NULL;
			break;
		}
		case PREFSWIN_SHOW :
		{
			printf( "PREFSWIN_SHOW\n" );
			if( fPrefsWin == NULL)
			{
				BPoint point;
				AppSettings->FindPoint( kPrefsPrefWindowPoint, &point );
				BRect rect;
				rect.left = point.x;
				rect.top = point.y;
				rect.right = rect.left + kPrefsWinWidth;
				rect.bottom = rect.top + kPrefsWinHeight;
				/* check for out of bounds */
				BScreen screen;
				if( !screen.Frame().Contains( rect ) )
				{
					rect.OffsetTo(	200, 200 );
				}
				fPrefsWin = new PrefsWin(
					rect );
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
					AppSettings->FindRect( kPrefsMainWindowRect, &r );
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
						AppSettings->FindRect( kPrefsMainWindowRect, &r );
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
				AppSettings->FindInt8( kPrefsNewWindowStartPage, &val );
				printf( "VALUE: %d\n", val );
				switch( val )
				{
					case 0 : break;	// blank, do nothing
					case 1 : // homepage
					{
						BMessenger msgr( FirstWindow() );
						BMessage* homepage = new BMessage( URL_OPEN );
						BString string;
						AppSettings->FindString( kPrefsHomePage, &string );
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

	fSiteHandler = new SiteHandler();

	PluginManager->BuildRoster(true);
	fFirstWindow->Show();
	//SetPulseRate(2000000);
	
	// if we have a homepage specified in prefs
	BMessenger msgr( FirstWindow() );
	BMessage* homepage = new BMessage( URL_OPEN );
	BString string;
	AppSettings->FindString( kPrefsHomePage, &string );
	homepage->AddString( "url_to_open", string.String() );
	msgr.SendMessage( homepage );
}

void App::ArgvReceived(int32 argc, char **argv){
}


void
App::CheckSettings(
	char* settings_path )
{
	printf( "App::CheckSettings()\n" );
	
	if( settings_path != NULL)
	{
		if( !AppSettings->HasString( kPrefsSettingsDirectory ) )
			AppSettings->AddString( kPrefsSettingsDirectory, settings_path );
		else
			AppSettings->ReplaceString( kPrefsSettingsDirectory, settings_path );
		
		BString name( settings_path );
		name += "/ThemisSettings";	// changed naming. (without space)
		if( !AppSettings->HasString( kPrefsSettingsFilePath ) )
			AppSettings->AddString( kPrefsSettingsFilePath, name.String() );
		else
			AppSettings->ReplaceString( kPrefsSettingsFilePath, name.String() );
	}
	else
	{
		if( !AppSettings->HasString( kPrefsSettingsDirectory ) )
			AppSettings->AddString( kPrefsSettingsDirectory, "/boot/home/config/settings/Themis/" );
		if( !AppSettings->HasString( kPrefsSettingsFilePath ) )
			AppSettings->AddString( kPrefsSettingsFilePath, "/boot/home/config/settings/Themis/ThemisSettings" );
	}
	
	/* check if the settings message contains an entry. if not add it with its default value. */
	
	/* General */
	if( !AppSettings->HasRect( kPrefsMainWindowRect ) )
		AppSettings->AddRect( kPrefsMainWindowRect, BRect( 130, 130, 650, 450 ) );
	
	/* Prefs General */
	if( !AppSettings->HasInt32( kPrefsLastSelectedItem ) )
		AppSettings->AddInt32( kPrefsLastSelectedItem, 0 );
	if( !AppSettings->HasPoint( kPrefsPrefWindowPoint ) )
		AppSettings->AddPoint( kPrefsPrefWindowPoint, BPoint( 200, 200 ) );
	
	/* Prefs Window */
	if( !AppSettings->HasString( kPrefsHomePage ) )
		AppSettings->AddString( kPrefsHomePage, kAboutBlankPage );
	if( !AppSettings->HasBool( kPrefsIntelligentZoom ) )
		AppSettings->AddBool( kPrefsIntelligentZoom, false );
	if( !AppSettings->HasBool( kPrefsShowTypeAheadWindow ) )
		AppSettings->AddBool( kPrefsShowTypeAheadWindow, false );
	if( !AppSettings->HasBool( kPrefsShowTabsAtStartup ) )
		AppSettings->AddBool( kPrefsShowTabsAtStartup, false );
	if( !AppSettings->HasBool( kPrefsOpenTabsInBackground ) )
		AppSettings->AddBool( kPrefsOpenTabsInBackground, false );
	if( !AppSettings->HasInt8( kPrefsNewWindowStartPage ) )
		AppSettings->AddInt8( kPrefsNewWindowStartPage, 0 );
	if( !AppSettings->HasBool( kPrefsOpenBlankTargetInTab ) )
		AppSettings->AddBool( kPrefsOpenBlankTargetInTab, false );
	
	/* Prefs Privacy */
	if( !AppSettings->HasInt8( kPrefsGlobalHistoryDepthInDays ) )
		AppSettings->AddInt8( kPrefsGlobalHistoryDepthInDays, 9 );
	if( !AppSettings->HasInt8( kPrefsGlobalHistoryFreeURLCount ) )
		AppSettings->AddInt8( kPrefsGlobalHistoryFreeURLCount, 50 );
	if( !AppSettings->HasInt8( kPrefsTabHistoryDepth ) )
		AppSettings->AddInt8( kPrefsTabHistoryDepth, 10 );
	
	/* Prefs Parser */
	if( !AppSettings->HasString( kPrefsDTDDirectory ) )
	{
		BString dir;
		AppSettings->FindString( kPrefsSettingsDirectory, &dir );
		dir.Append( "/dtd" );
		AppSettings->AddString( kPrefsDTDDirectory, dir.String() );
	}
	if(	!AppSettings->HasString( kPrefsActiveDTDPath ) ||
			strcmp( AppSettings->FindString( kPrefsActiveDTDPath ), kNoDTDFoundString ) == 0 )
	{
		/* set our default */
		AppSettings->AddString( kPrefsActiveDTDPath, kNoDTDFoundString );
		
		/* find a DTD */
		BString dtddir;
		AppSettings->FindString( kPrefsSettingsDirectory, &dtddir );
		dtddir.Append( "/dtd/" );
		printf( "DTD dir: %s\n", dtddir.String() );
		
		BDirectory dir( dtddir.String() );		
		if( dir.InitCheck() != B_OK )
		{
			printf( "DTD directory (%s) not found!\n", dtddir.String() );
			printf( "Setting kPrefsActiveDTDPath to \"none\"\n" );
			AppSettings->AddString( kPrefsActiveDTDPath, kNoDTDFoundString );
		}
		else
		{
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
					if( AppSettings->HasString( kPrefsActiveDTDPath ) )
					{
						printf( "replacing kPrefsActiveDTDPath with: %s\n", path.Path() );
						AppSettings->ReplaceString( kPrefsActiveDTDPath, path.Path() );
					}
					else
					{
						printf( "adding kPrefsActiveDTDPath: %s\n", path.Path() );
						AppSettings->AddString( kPrefsActiveDTDPath, path.Path() );
					}
				}
			}
		}
		/* end: find a DTD */
	}
	
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

SiteHandler*
App::GetSiteHandler()
{
	if( fSiteHandler )
		return fSiteHandler;
	else
		return NULL;
}

Win*
App::FirstWindow()
{
	return fFirstWindow;
}

status_t
App::LoadSettings()
{
	printf( "App::LoadSettings()\n" );
	if( AppSettings != NULL )
	{
		/* check if dtd dir exists, if not, create it */
		{
			BEntry ent("/boot/home/config/settings/Themis/dtd/",true);
			if (!ent.Exists())
				create_directory("/boot/home/config/settings/Themis/dtd/",0555);
		}
		
		status_t ret=B_OK;
		BPath path;
		if (find_directory(B_USER_SETTINGS_DIRECTORY,&path)==B_OK)
		{
			path.Append("Themis/",true);
			BEntry prefsent(path.Path(),true);
			if (prefsent.Exists())
			{
				prefsent.GetPath(&path);
				path.Append("ThemisSettings",false);
				prefsent.SetTo(path.Path(),true);
				if (prefsent.Exists())
				{
					BFile *file=new BFile(&prefsent,B_READ_ONLY);
					ret=file->InitCheck();
					if (ret!=B_OK)
					{
						delete file;
						CheckSettings();
						return ret;
					}
					
					file->Lock();
					ret|=AppSettings->Unflatten(file);
					file->Unlock();
					delete file;
					printf("  Settings loaded.\n");
					
					/* check for missing entries, or add defaults if needed */
					CheckSettings();
					
					return ret;
					
				}
				else	// prefsfile does not exist
				{
					path.GetParent(&path);
					
					/* check for missing entries, or add defaults if needed */
					CheckSettings( ( char* )path.Path() );

					SaveSettings();
					return B_OK;
				}
				
			}
			else	// prefsdir does not exist
			{
				create_directory(path.Path(),0777);
				CheckSettings( ( char* )path.Path() );
				SaveSettings();
				
				return B_OK;
			}
			
		}
		else
			return B_ERROR;
	}
	return B_ERROR;
}

status_t App::SaveSettings() {
	if (AppSettings!=NULL) {
		
		status_t ret=B_OK;
		
		BString fname,dname;
		AppSettings->FindString(kPrefsSettingsDirectory,&dname);
		AppSettings->FindString(kPrefsSettingsFilePath,&fname);
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
		
		// temp
		AppSettings->PrintToStream();
		
		return ret;
	}
	return B_ERROR;
}

void
App::SetFirstWindow( Win* newfirst )
{
	fFirstWindow = newfirst;
}
