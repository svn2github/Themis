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
iview::iview(BRect frame,char *name,uint32 resize,uint32 flags)
	:BView(frame,name,resize,flags) 
{
	image=NULL;
	SetViewColor(0,0,0);
	
}
void iview::Draw(BRect updr) 
{
	if (image!=NULL) {
		DrawBitmapAsync(image,updr,updr);
	}
	
}

iview::~iview() 
{
	delete image;
	image=NULL;
}
imagewin::imagewin(char *title)
	:BWindow(BRect(100,100,200,200),title,B_TITLED_WINDOW,B_NOT_ZOOMABLE,B_CURRENT_WORKSPACE)
{
	view=new iview(Bounds(),title,B_FOLLOW_ALL,B_WILL_DRAW);
	AddChild(view);
	Show();
}
imagewin::~imagewin()
{
}

testplug::testplug(BMessage *info):PlugClass(info)
{
	whead=NULL;
	CacheSys=NULL;
	CacheSys=(CachePlug*)PlugMan->FindPlugin(CachePlugin);
	if (CacheSys!=NULL)
		cache_user_token=CacheSys->Register(Type(),"Test Plug-in");
}

testplug::~testplug(){
	iwind *cur=whead;
	while (cur!=NULL) {
		whead=cur->next;
		delete cur;
		cur=whead;
	}
	
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
				}break;
				case ReturnedData: {
					BString mime;
					BString url;
					uint64 br=0,cl=0;
					BMallocIO *data=NULL;
					msg->FindString("mimetype",&mime);
					printf("testplug: checking mime type - ");
					fflush(stdout);
						int32 objtoken=0;
					
					if ((mime.ICompare("image/png")==0) || (mime.ICompare("image/jpeg")==0)) {
						printf("supported\n");
						msg->FindInt64("content-length",(int64*)&cl);
						msg->FindInt64("bytes-received",(int64*)&br);
						msg->FindString("url",&url);
						printf("content-length: %lu\n",cl);
						printf("bytes received: %lu\n",br);
//						printf("data pointer: %p\n",data);
							if (CacheSys!=NULL) {
								if (msg->HasInt32("cache_object_token"))
									objtoken=msg->FindInt32("cache_object_token");
								else
									objtoken=CacheSys->FindObject(cache_user_token,url.String());
								if (objtoken<0)
									return PLUG_HANDLE_GOOD;
								printf("creating imagedata\n");
								data=new BMallocIO();
								ssize_t size=CacheSys->GetObjectSize(cache_user_token,objtoken);
								printf("cache object size is: %ld\n",size);
								unsigned char *dta=new unsigned char[size];
								memset(dta,0,size);
								size=CacheSys->Read(cache_user_token,objtoken,dta,size);
								printf("read %ld bytes from cache.\n",size);
								data->Write(dta,size);
								printf("wrote data to imagedata\n");
								memset(dta,0,size);
								delete dta;
								dta=NULL;
							}
						
						iwind *cur=NULL;
						if ((cl==0) && (br>0)) {
							//unknown size...
							cur=whead;
							if (cur==NULL) {
								whead=new iwind;
								whead->objecttoken=objtoken;
								cur=whead;
								cur->url=url;
									BString title("Themis - TestPlug Image: ");
									title <<url;
								cur->win=new imagewin((char*)title.String());
							} else {
								cur=whead;
								iwind *last=NULL;
								while (cur!=NULL) {
									if (cur->url==url)
										break;
									if (cur->next==NULL)
										last=cur;
									cur=cur->next;
								}
								if (cur==NULL) {
									cur=new iwind;
									cur->objecttoken=objtoken;
									last->next=cur;
									cur->url=url;
									BString title("Themis - TestPlug Image: ");
									title <<url;
									cur->win=new imagewin((char*)title.String());
									
								}
								
							}
							if (cur->win->view->image!=NULL) {
								delete cur->win->view->image;
								cur->win->view->image=NULL;
							} 
							cur->win->Lock();
							
							cur->win->view->image=BTranslationUtils::GetBitmap(data);
							cur->win->view->DrawBitmapAsync(cur->win->view->image);
							cur->win->Unlock();
							
							
						} else {
							if (br==cl) {
								cur=whead;
								if (cur==NULL) {
									cur=new iwind;
									cur->objecttoken=objtoken;
									whead=cur;
								} else {
									while (cur->next!=NULL)
										cur=cur->next;
									cur->next=new iwind;
									cur=cur->next;
									cur->objecttoken=objtoken;
								}
								cur->url=url;
								printf("creating an image window by the name of %s\n",url.String());
									BString title("Themis - TestPlug Image: ");
									title <<url;
								
								cur->win=new imagewin((char*)title.String());
								cur->win->Lock();
								
								cur->win->view->image=BTranslationUtils::GetBitmap(data);
								cur->win->view->DrawBitmapAsync(cur->win->view->image);
								cur->win->Unlock();
								
							}
							
						}
						
						
					} else {
						printf(" unsupported: %s\n",mime.String());
					}
				}break;
			}
			
		}break;
		case COMMAND_INFO_REQUEST: {
			int32 replyto=0;
			msg->FindInt32("ReplyTo",&replyto);
			switch(msg->what) {
				case GetSupportedMIMEType: {
					
			if (msg->HasBool("supportedmimetypes")) {
				BMessage types(SupportedMIMEType);
				types.AddString("mimetype","image/jpeg");
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
					PlugMan->Broadcast(PlugID(),replyto,&container);
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

