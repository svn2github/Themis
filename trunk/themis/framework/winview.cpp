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
#include "winview.h"
#define OpenLocation 'oloc'
#include <Handler.h>
#include <stdio.h>
#include <Button.h>
#include <ctype.h>
#include "protocol_plugin.h"
#include "tcplayer.h"
extern tcplayer *TCP;

extern plugman *PluginManager;

winview::winview(BRect frame,const char *name,uint32 resizem,uint32 flags)
	:BView(frame,name,resizem,flags) {
	SetViewColor(216,216,216);
	menubar=new BMenuBar(Bounds(),"main_win_menubar");
	AddChild(menubar);
	//set up BMenu Child
	filemenu=new BMenu("File");
	optionsmenu=new BMenu("Options");
	menubar->AddItem(filemenu);
	menubar->AddItem(optionsmenu);
	BMessage *info=new BMessage(AddInitInfo);
				info->AddPointer("main_menu_bar",menubar);
				info->AddPointer("file_menu",filemenu);
				info->AddPointer("options_menu",optionsmenu);
				info->AddPointer("window",Window());
//				info->AddPointer("parser",Parser);
				BMessenger *iimsgr=new BMessenger(PluginManager,NULL,NULL);
		{
		BMessage reply;	
		iimsgr->SendMessage(info,&reply);
		printf("Window has tried to init: %ld\n",reply.what);
			
		reply.PrintToStream();
			
		}
		
		delete iimsgr;
		delete info;
	BRect r(Bounds());
	r.top=20;
	r.bottom=35;
	BMessage *msg=new BMessage(OpenLocation);
	locline=new BTextControl(r,"locationline","Location:",NULL,msg,B_FOLLOW_TOP|B_FOLLOW_LEFT_RIGHT,B_WILL_DRAW|B_NAVIGABLE);
	locline->SetDivider(50);
	r=Bounds();
	r.top=r.bottom-15;
	r.right=r.left+30;
	msg=new BMessage(B_CANCEL);
	AddChild(locline);
	stopbutton=new BButton(r,"cancelbutton","Stop",msg);
	stopbutton->SetEnabled(false);
	AddChild(stopbutton);	
}
winview::~winview() {
}

void winview::AttachedToWindow() {
	stopbutton->SetTarget(this);
	locline->SetTarget(this);
	locline->MakeFocus(true);
	//Yuck, gonna haft to handle this better than this...!
	//basically, we need to set the Window value for all plugins so they can
	//add menu options before they are themselves used... but, that would mean
	//that the plugins would have to be in memory at the time... maybe archive them...
	
}
void winview::MessageReceived(BMessage *msg) {
	switch(msg->what) {
		case UpdateDisplayedURL: {
			BString url;
			msg->FindString("url",&url);
			locline->SetText(url.String());
		}break;
		case B_CANCEL: {
			printf("Ordering protocol to stop\n");
			protocol_plugin *pobj=(protocol_plugin*)PluginManager->FindPlugin(protocol);
			if (pobj!=NULL)
				pobj->Stop();
			status_t stat;
			wait_for_thread(pobj->Thread(),&stat);
			stopbutton->SetEnabled(false);
		}break;
		case OpenLocation: {
			char *usepass=NULL;
			char *workurl=NULL;
			stopbutton->SetEnabled(true);
			msg->PrintToStream();
			BString url=locline->Text();
			int len=strlen(locline->Text());
			char *url_=new char[len+1];
			workurl=new char[len+1];
			memset(url_,0,len+1);
			memset(workurl,0,len+1);
			strcpy(url_,locline->Text());
			strcpy(workurl,url_);
			protocol=HTTPPlugin;
			if (url.Length()>0) {
				int32 pos=url.FindFirst("://");
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
			if (url.Length()==0) {
				return;
			}
//			if (pobj!=NULL) {
				BMessage *info=new BMessage;
//				info->AddPointer("tcp_layer_ptr",TCP);
				info->AddPointer("top_view",this);
				info->AddPointer("window",Window());
//				info->AddPointer("parser",Parser);
				info->AddPointer("plug_manager",PluginManager);
				info->AddPointer("main_menu_bar",menubar);
				info->AddPointer("file_menu",filemenu);
				info->AddPointer("options_menu",optionsmenu);
				info->AddString("target_url",workurl);
				delete workurl;
				info->AddInt32("action",LoadingNewPage);
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
					PluginManager->Broadcast(TARGET_CACHE,bcast);
					delete bcast;
//					BMessenger *msgr=new BMessenger(cache->Handler(),NULL,NULL);
//					msgr->SendMessage(LoadingNewPage);
//					delete msgr;
//				}
				bcast=new BMessage;
				info->AddInt32("command",COMMAND_RETRIEVE);
				bcast->AddMessage("message",info);
				PluginManager->Broadcast(TARGET_PROTOCOL,bcast);
				delete bcast;
				delete info;
			
//				pobj->GetURL(info);
//			}
		}break;
		default:
		BView::MessageReceived(msg);
	}
}
