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
int WinH=800;
int WinW=600;

extern plugman *PluginManager;
Win::Win(BRect frame,const char *title,window_type type,uint32 flags,uint32 wspace)
    :BWindow(frame,title,type,flags,wspace) {
  View=new winview(Bounds(),"mainview",B_FOLLOW_ALL,B_WILL_DRAW|B_ASYNCHRONOUS_CONTROLS|B_NAVIGABLE_JUMP);
	startup=true;
//  Parser=new HTMLParser;
//  Parser->View=View;
//  View->Parser=Parser;
	AddChild(View);
}
bool Win::QuitRequested() {
  BMessenger *msgr=new BMessenger(NULL,Parser,NULL);
  msgr->SendMessage(B_QUIT_REQUESTED);
  delete msgr;
	return true;
}
void Win::MessageReceived(BMessage *msg) {
	switch(msg->what) {
		case PlugInLoaded: {
					
			protocol_plugin *pobj=NULL;
			msg->FindPointer("plugin",(void**)&pobj);
					
			if (pobj!=NULL) {
				printf("Window: plugin loaded %c%c%c%c\n",(int)pobj->PlugID()>>24,(int)pobj->PlugID()>>16,(int)pobj->PlugID()>>8,(int)pobj->PlugID());
				pobj->Window=this;
				pobj->AddMenuItems(View->optionsmenu);
			} else {
				printf("Win: pobj was null\n");
				
			}
			
		}break;
		default: {
			BWindow::MessageReceived(msg);
		}
	}
//	Unlock();
}
void Win::WindowActivated(bool active) {
	BWindow::WindowActivated(active);
	if (startup) {
		startup=false;
		printf("WindowActivated\n");
		
	}
}
