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
#include "app.h"
#include "tcplayer.h"
#include <Directory.h>
#include <storage/FindDirectory.h>
plugman *PluginManager;
tcplayer *TCP;
BMessage *AppSettings;

App::App(const char *appsig)
	:BApplication(appsig) {
	AppSettings=new BMessage;
	LoadSettings();
	app_info ai;
	qr_called=0;
	AWin=NULL;
	GetAppInfo(&ai);
	entry_ref appdirref;
	BEntry *ent=new BEntry(&ai.ref);
	ent->GetParent(ent);
	ent->GetRef(&appdirref);
	delete ent;
	PluginManager=new plugman(appdirref);
	TCP=new tcplayer;
	TCP->Start();
	BRect r(100,100,650,450);
	if (AppSettings->HasRect("main_window_rect"))
		AppSettings->FindRect("main_window_rect",&r);
	
	win=new Win(r,"Themis",B_DOCUMENT_WINDOW,B_QUIT_ON_WINDOW_CLOSE|B_ASYNCHRONOUS_CONTROLS,B_CURRENT_WORKSPACE);
	PluginManager->Window=win;
	BMessenger *msgr=new BMessenger(PluginManager,NULL,NULL);
		BMessage *msg=new BMessage(AddInitInfo);
		msg->AddPointer("tcp_layer_ptr",TCP);
		msg->AddPointer("settings_message_ptr",&AppSettings);
		{
		BMessage reply;	
		msgr->SendMessage(msg,&reply);
			if (reply.what==B_ERROR)
				printf("Problem setting plug-in initialization info.\n");
		}
	delete msgr;
		delete msg;
	PluginManager->BuildRoster(true);
}
App::~App(){
	printf("app destructor\n");
	if (!qr_called)
		QuitRequested();
	SaveSettings();
	if (AppSettings!=NULL) {
		
		delete AppSettings;
		AppSettings=NULL;
	}
	printf("~app end\n");
}
void App::AboutRequested() {
	if (AWin==NULL) {
		AWin=new aboutwin(BRect(100,100,600,300),"About Themis",B_TITLED_WINDOW,B_ASYNCHRONOUS_CONTROLS|B_NOT_MINIMIZABLE);
	} else {
		AWin->Activate(true);
	}
	
}

bool App::QuitRequested(){
	atomic_add(&qr_called,1);
	status_t stat;
	BWindow *w=NULL;
	BMessenger *msgr=NULL;
	thread_id th;
	while (CountWindows()>0) {
		w=WindowAt(0);
		if (w!=NULL) {
//			printf("app: quit message target is Window \"%s\".\n",w->Title());
			msgr=new BMessenger(NULL,w,NULL);
			th=w->Thread();
			msgr->SendMessage(B_QUIT_REQUESTED);
//			printf("app: quit requested message sent, waiting for the window to die.\n");
			wait_for_thread(th,&stat);
			delete msgr;
		}
	}
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
	
	if (TCP!=NULL) {
		TCP->lock->Lock();
		stat=TCP->Quit();
		TCP->lock->Unlock();
		delete TCP;
		TCP=NULL;
	}
	
	//  PluginManager->Lock();
	//  PluginManager->Quit();
	return true;
}
void App::MessageReceived(BMessage *msg){
	switch(msg->what){
		default:{
			BApplication::MessageReceived(msg);
		}
	}
}
void App::RefsReceived(BMessage *refs){
}
void App::ReadyToRun(){
	win->Show();
}
void App::ArgvReceived(int32 argc, char **argv){
}
void App::InitSettings(char *settings_path) {
	if (settings_path!=NULL) {
		AppSettings->AddString("settings_directory",settings_path);
		BString name(settings_path);
		name+="/Themis Settings";
		AppSettings->AddString("settings_file",name.String());
	} else {
		AppSettings->AddString("settings_directory","/boot/home/config/settings/Themis/");
		AppSettings->AddString("settings_file","/boot/home/config/settings/Themis/Themis Settings");
	}
	AppSettings->PrintToStream();
	
}

status_t App::LoadSettings() {
	if (AppSettings!=NULL) {
			
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
