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
#include <TranslationUtils.h>
#include <DataIO.h>
#include "cacheplug.h"
#include <File.h>
#include <NodeInfo.h>
#define PlugIDdef 'test'
#define PlugNamedef "Test Plug-in"
#define PlugVersdef 1.0
BMessage **AppSettings_p;
BMessage *AppSettings;

testplug *TP;
status_t Initialize(void *info) {
  AppSettings=NULL;
	TP=NULL;
	CachePlug *cobj;
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
testplug::testplug(BMessage *info):PlugClass(info,"Test Plug-in")//,MessageSystem()
{
//	MsgSysRegister(this);
	CacheSys=NULL;
	CacheSys=(CachePlug*)PlugMan->FindPlugin(CachePlugin);
	if (CacheSys!=NULL)
		cache_user_token=CacheSys->Register(Type(),"Test Plug-in");
}

testplug::~testplug(){
	if (CacheSys!=NULL)
		CacheSys->Unregister(cache_user_token);
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
status_t testplug::BroadcastReply(BMessage *msg)
{
}
uint32 testplug::BroadcastTarget()
{
}

status_t testplug::ReceiveBroadcast(BMessage *msg){
	int32 command=0;
	msg->FindInt32("command",&command);
	printf("TestPlug ReceiveBroadcast: \n");
	msg->PrintToStream();
	
	switch(command) {
		case COMMAND_INFO: {
			switch(msg->what) {
				case PlugInLoaded: {
					PlugClass *pobj=NULL;
					msg->FindPointer("plugin",(void**)&pobj);
					if (pobj!=NULL) {
						if ((pobj->Type()&TARGET_CACHE)!=0) {
							CacheSys=(CachePlug*)pobj;
							cache_user_token=CacheSys->Register(Type(),"Test Plug-in");
						}
					}
				}break;
				case PlugInUnLoaded: {
					uint32 type=0;
					type=msg->FindInt32("type");
					switch(type) {
						case (TARGET_CACHE|TYPE_RAM|TYPE_DISK): {
							cache_user_token=0;
							CacheSys=NULL;
						}break;
						default: {
							
							//do nothing
						}
						
					}
					
				}break;
				case B_QUIT_REQUESTED: {
					if (CacheSys!=NULL)
						CacheSys->Unregister(cache_user_token);
					CacheSys=NULL;
					
				}break;
		//		case ProtocolConnectionClosed:
				case ProtocolConnectionClosed: {
					printf("[TESTPLUG] Returned data\n");
					msg->PrintToStream();
					bool request_done=false;
					msg->FindBool("request_done",&request_done);
					if (request_done) {
						int32 cache_object_token=0;
						msg->FindInt32("cache_object_token",&cache_object_token);
						ssize_t size=CacheSys->GetObjectSize(cache_user_token,cache_object_token);
						char *data=new char[size+1];
						memset(data,0,size+1);
						CacheSys->Read(cache_user_token,cache_object_token,data,size);
						printf("[Test Plug-in] Dumping the data to file \"test.data\".\n");
						BFile *file=new BFile("test.data",B_CREATE_FILE|B_ERASE_FILE|B_READ_WRITE);
						file->Write(data,size);
						file->Sync();
						BNodeInfo *ni=new BNodeInfo(file);
						const char *mime=NULL;
						msg->FindString("mimetype",&mime);
						if (mime==NULL)
							mime="application/octet-stream\0";
						ni->SetType(mime);
						delete ni;
						delete file;
						printf("[Test Plug-in] Data:\n%s\n",data);
						
						memset(data,0,size+1);
						delete data;
						data=NULL;
						size=0;
						
					}
				}break;
			}
			
		}break;
		case COMMAND_INFO_REQUEST: {
			int32 replyto=0;
			msg->FindInt32("broadcaster_target_id",&replyto);
			switch(msg->what) {
				case GetSupportedMIMEType: {
					
			if (msg->HasBool("supportedmimetypes")) {
				BMessage types(SupportedMIMEType);
				types.AddString("mimetype","text/html");
				types.AddInt32("command",COMMAND_INFO);
				PlugClass *plug=NULL;
				if (msg->HasPointer("broadcaster_pointer")) {
					msg->FindPointer("broadcaster_pointer",(void**)&plug);
					if (plug!=NULL)
						plug->BroadcastReply(&types);
				} else {	
					Broadcast(replyto,msg);
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

