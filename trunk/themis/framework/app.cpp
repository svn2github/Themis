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
#include "app.h"
#include "tcplayer.h"
plugman *PluginManager;
tcplayer *TCP;

App::App(const char *appsig)
	:BApplication(appsig) {
	app_info ai;
	GetAppInfo(&ai);
	entry_ref appdirref;
	BEntry *ent=new BEntry(&ai.ref);
	ent->GetParent(ent);
	ent->GetRef(&appdirref);
	delete ent;
	TCP=new tcplayer;
	TCP->Start();
	BRect r(100,100,650,450);
	PluginManager=new plugman(appdirref);
	win=new Win(r,"Themis",B_DOCUMENT_WINDOW,B_QUIT_ON_WINDOW_CLOSE,B_CURRENT_WORKSPACE);
	PluginManager->BuildRoster();
	PluginManager->Window=win;
}
App::~App(){
}
bool App::QuitRequested(){
	printf("app destructor\n");
	status_t stat;
	stat=TCP->Quit();
	delete TCP;
	BWindow *w=NULL;
	BMessenger *msgr=NULL;
	thread_id th;
	while (CountWindows()>0) {
		w=WindowAt(0);
		if (w!=NULL) {
			msgr=new BMessenger(NULL,w,NULL);
			th=w->Thread();
			msgr->SendMessage(B_QUIT_REQUESTED);
			wait_for_thread(th,&stat);
			delete msgr;
		}
	}
	msgr=new BMessenger(NULL,PluginManager,NULL);
	th=PluginManager->Thread();
	msgr->SendMessage(B_QUIT_REQUESTED);
	delete msgr;
	wait_for_thread(th,&stat);
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
