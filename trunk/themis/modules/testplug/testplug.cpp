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

#include "testplug.h"
#include <Message.h>
#include "commondefs.h"
#include <stdio.h>
#include "plugman.h"
#define PlugIDdef 'test'
#define PlugNamedef "Test Plug-in"
#define PlugVersdef 1.0
BMessage **AppSettings_p;
BMessage *AppSettings;

testplug *TP;
status_t Initialize(void *info) {
  AppSettings=NULL;
	TP=NULL;
  if (info!=NULL) {
	  
	BMessage *imsg=(BMessage *)info;
	if (imsg->HasPointer("settings_message_ptr")) {
		
		imsg->FindPointer("settings_message_ptr",(void**)&AppSettings_p);
		AppSettings=*AppSettings_p;
		
	}
	TP=new testplug(imsg);
  } else {
  	TP=new testplug;
  }
  
	return B_OK;
	
}
status_t Shutdown(bool now) {
//	if (now)
	delete TP;
	return B_OK;
	
}
PlugClass *GetObject(void) {
	return TP;
	
}

testplug::testplug(BMessage *info):PlugClass(info)
{
}

testplug::~testplug(){
}

void testplug::MessageReceived(BMessage *msg){
	switch(msg->what) {
		default:
			;
//			BHandler::MessageReceived(msg);
	}
}
bool testplug::IsHandler(){
	return false;
}
BHandler *testplug::Handler(){
	return NULL;
}
bool testplug::IsPersistent(){
	return true;
}
uint32 testplug::PlugID(){
	return PlugIDdef;
}

char *testplug::PlugName(){
	return PlugNamedef;
}
float testplug::PlugVersion(){
	return PlugVersdef;
}
void testplug::Heartbeat(){
}
status_t testplug::ReceiveBroadcast(BMessage *msg){
	int32 command=0;
	msg->FindInt32("command",&command);
	printf("TestPlug ReceiveBroadcast: \n");
	msg->PrintToStream();
	
	switch(command) {
		case COMMAND_INFO_REQUEST: {
			int32 replyto=0;
			msg->FindInt32("ReplyTo",&replyto);
			switch(msg->what) {
				case GetSupportedMIMEType: {
					
			if (msg->HasBool("supportedmimetypes")) {
				BMessage types(SupportedMIMEType);
				types.AddString("mimetype","image/jpeg");
				types.AddString("mimetype","image/gif");
				types.AddString("mimetype","image/png");
				types.AddInt32("command",COMMAND_INFO);
				PlugClass *plug=NULL;
				if (msg->HasPointer("ReplyToPointer")) {
					msg->FindPointer("ReplyToPointer",(void**)&plug);
					if (plug!=NULL)
						plug->BroadcastReply(&types);
				} else {	
					BMessage container;
					container.AddMessage("message",&types);
					PlugMan->Broadcast(replyto,&container);
				}
				
			}
				}break;
			}
			
		}break;
		default: {
			printf("TestPlug addon doesn't handle this broadcast.\n");
			msg->PrintToStream();
			return PLUG_DOESNT_HANDLE;
		}
	
	}
	return PLUG_HANDLE_GOOD;
}

int32 testplug::Type(){
	return IMAGE_HANDLER;
}

