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
#include "http_proto.h"
http_protocol *HTTP_proto;
#include "commondefs.h"
#include "http_defines.h"
#include <Menu.h>
#include "plugman.h"
tcplayer *TCP;
BMessage *AppSettings;
BMessage **AppSettings_p;
//plugman *PlugMan;
BMessage *HTTPSettings;

status_t Initialize(void *info)
 {
  TCP=NULL;
  AppSettings=NULL;
	HTTPSettings=NULL;
//	 PlugMan=NULL;
  if (info!=NULL) {
	  
	BMessage *imsg=(BMessage *)info;
//	if (imsg!=NULL) {
		imsg->PrintToStream();
		
	 if (imsg->HasPointer("tcp_layer_ptr"))
	 	imsg->FindPointer("tcp_layer_ptr",(void**)&TCP);
//	}
//	if (imsg->HasPointer("plug_manager"))
//		imsg->FindPointer("plug_manager",(void**)&PlugMan);
	printf("(http) TCP layer: %p\n",TCP);
  HTTP_proto=new http_protocol(imsg);
	 

  }else 
  	HTTP_proto=new http_protocol(NULL);
  
  return B_OK;
 }
status_t Shutdown(bool now)
 {
	printf("1234 HTTP Being Shutdown\n");
	if (AppSettings_p!=NULL) {
		if (!AppSettings->HasMessage("http_settings_message")) {
			AppSettings->AddMessage("http_settings_message",HTTPSettings);
		} else
			AppSettings->ReplaceMessage("http_settings_message",HTTPSettings);
	}
	 
//  if (now)
	  if (HTTP_proto!=NULL)
	   {
	    HTTP_proto->Stop();
  		delete HTTP_proto;
  	   }
	TCP=NULL;
//	PlugMan=NULL;
  return B_OK;
 }
protocol_plugin* GetObject(void)
 {
  if (HTTP_proto==NULL)
   HTTP_proto=new http_protocol();
  return HTTP_proto;
 }
int32 http_protocol::Type() 
{
	return HTTP_PROTOCOL;
}
status_t http_protocol::BroadcastReply(BMessage *msg){
	printf("http proto: BroadcastReply\n");
	
	int32 cmd=0;
	msg->FindInt32("command",&cmd);
	switch(cmd){
		case COMMAND_INFO: {
			switch(msg->what) {
				case SupportedMIMEType: {
//					printf("Add types in following message to MIME Type list.\n");
//					msg->PrintToStream();
					BString str;
					int32 count=0;
					type_code type;
					msg->GetInfo("mimetype",&type,&count);
					smt_st *cur,*cur2,*cur3;
					for (int32 i=0;i<count;i++) {
						str="nothing";
						msg->FindString("mimetype",i,&str);
						cur3=NULL;
						cur3=smthead;
						while (cur3!=NULL) {
							if (strcasecmp(cur3->type,str.String())==0)
								break;
							cur3=cur3->next;
						}
						if (cur3!=NULL)
							continue;
						/*
							The above code should prevent a duplicate type from being added.
						*/
						cur3=NULL;
						cur=new smt_st;
						cur->type=new char[str.Length()+1];
						memset(cur->type,0,str.Length()+1);
						strcpy(cur->type,str.String());
						if (smthead==NULL) {
							smthead=cur;
						} else {
							cur2=smthead;
							while (cur2->next!=NULL)
								cur2=cur2->next;
							cur2->next=cur;
						}
						
					}
					
				}break;
				case CachedObject: 
				case CacheObjectNotFound: {
					printf("http: reply from cache received.\n");
					cache_reply=new BMessage(*msg);
					release_sem(HTTP->cache_sem);
				}break;
				
				default: {
					msg->PrintToStream();
					//display the message info, but otherwise ignore it
				}
			}
		}break;
		default:
			return PLUG_DOESNT_HANDLE;
}
	return PLUG_REPLY_RECEIVED;
}

status_t http_protocol::ReceiveBroadcast(BMessage *msg)
{
	status_t stat=B_ERROR;
	int32 command=0;
	if (msg->HasInt32("command"))
		msg->FindInt32("command",&command);
 	BView *vw;
	if (msg->HasPointer("window"))
		msg->FindPointer("window",(void**)&win);
		if (Window==NULL)
			Window=win;
	
	if (msg->HasPointer("top_view"))
		msg->FindPointer("top_view",(void**)&vw);
 	if (view==NULL) {
		view=vw;
			BMenu *v;
			if (msg->HasPointer("options_menu"))
				msg->FindPointer("options_menu",(void**)&v);
//			HOH->AddMenu(v);
//			win->AddHandler((BHandler*)HOH);
		
	} else {
		if (vw!=view) {
			BMenu *v;
			if (msg->HasPointer("options_menu"))
				msg->FindPointer("options_menu",(void**)&v);
//			HOH->AddMenu(v);
			
			view=vw;
		}
		
	}
	if (command==COMMAND_RETRIEVE)
		printf("http: command is retrieve\n");
	if (command==COMMAND_INFO)
		printf("http: command is info\n");
	switch (command) {
		case COMMAND_RETRIEVE: {
			int32 action=0;
			BMessage *rmsg=new BMessage(*msg);
//			printf("[http proto retrieve 1] message:\n");
//			rmsg->PrintToStream();
			
//			if (msg->HasInt32("action"))
//				msg->FindInt32("action",&action);
//			printf("http proto: action is %c%c%c%c\n",action>>24,action>>16,action>>8,action);
			BString targ;
			rmsg->FindString("target_url",&targ);
			printf("http proto: target url: %s\n",targ.String());
			
			HTTP->Lock();
			printf("http proto: http locked\n");
			HTTP->AddRequest(rmsg);
			printf("http proto: add request done\n");
			HTTP->Unlock();
			//we're already deleting rmsg (as info) int he HTTP->AddRequest() call.
//			delete rmsg;
			rmsg=NULL;
		}break;
		case COMMAND_INFO: {
			switch(msg->what) {
				case B_QUIT_REQUESTED: {
					HTTP->Lock();
					 if (HTTP->CacheSys!=NULL)
					 	HTTP->CacheSys->Unregister(HTTP->CacheToken);
					HTTP->CacheSys=NULL;
					HTTP->Unlock();
				}break;
				case LoadingNewPage: {
					printf("Do something useful when winview says we're loading a new page.\n");
				}break;
				case PlugInUnLoaded: {
					uint32 type=0;
					type=msg->FindInt32("type");
					switch(type) {
						case (TARGET_CACHE|TYPE_RAM|TYPE_DISK): {
							HTTP->Lock();
							HTTP->CacheToken=0;
							HTTP->CacheSys=NULL;
							HTTP->Unlock();
							printf("Cache system unloaded.\n");
						}break;
						default: {
							
							//do nothing
						}
						
					}
					
				}break;
				case PlugInLoaded: {
					int32 pid=0;
					msg->FindInt32("plugid",&pid);
					if (pid!=PlugID()) {
						PlugClass *pobj=NULL;
						msg->FindPointer("plugin",(void**)&pobj);
						if (pobj!=NULL) {
							if ((pobj->Type()&TARGET_CACHE)!=0) {
								HTTP->Lock();
										HTTP->CacheSys=(CachePlug*)pobj;
										HTTP->CacheToken=HTTP->CacheSys->Register(Type(),"HTTP Protocol Add-on");
										printf("*** Loaded CacheToken & object: %lu %p\n",HTTP->CacheToken,HTTP->CacheSys);
								HTTP->Unlock();
							}
							
							if (((pobj->Type()&TARGET_HANDLER)!=0) || ((pobj->Type()&TARGET_PARSER)!=0)) {
								if (PlugMan!=NULL) {
									BMessage *amsg=new BMessage(GetSupportedMIMEType);
									amsg->AddInt32("ReplyTo",Type());
									amsg->AddPointer("ReplyToPointer",this);
									amsg->AddInt32("command",COMMAND_INFO_REQUEST);
									amsg->AddBool("supportedmimetypes",true);
									pobj->ReceiveBroadcast(amsg);
									delete amsg;
									amsg=NULL;
								}
							}
							pobj=NULL;
						}
						
					}
					
				}break;
				default: {
					msg->PrintToStream();
					//display the message info, but otherwise ignore it
				}
			}
		}break;
		default:
			return PLUG_DOESNT_HANDLE;
	}
	printf("http_protocol::ReceiveBroadcast() exiting\n");
}
bool http_protocol::IsPersistent(){
	return true;
}
char* http_protocol::PlugName(void)
 {
  return PlugNamedef;
 }
uint32 http_protocol::PlugID(void)
 {
  return PlugIDdef;
 }
float http_protocol::PlugVersion(void)
 {
  return PlugVersdef;
 }
status_t http_protocol::Go(void)
 {
  return B_OK;
 }
http_protocol::http_protocol(BMessage *info)
              :protocol_plugin(info)
 {
 	Window=NULL;
	 smthead=NULL;
	 /*
	 	All web browsers should at least be able to support
	 	text/plain and text/html, so include those as the
	 	default mime types that are supported.
	 */
	 smthead=new smt_st;
	 smthead->type=new char[11];
	 memset(smthead->type,0,11);
	 strcpy(smthead->type,"text/plain");
	 smthead->next=new smt_st;
	 smthead->next->type=new char[10];
	 memset(smthead->next->type,0,10);
	 strcpy(smthead->next->type,"text/html");
	 
	Go();
	if (TCP==NULL) {
	 if (info->HasPointer("tcp_layer_ptr"))
	 	info->FindPointer("tcp_layer_ptr",(void**)&TCP);
	}
	
		HTTP=new httplayer(TCP,this);
		HTTP->Proto=this;
	HTTP->Start();
	HOH=new http_opt_handler;
	
 }
http_protocol::~http_protocol() {
	printf("http_protocol destructor\n");
//	Stop();
	printf("Locking HTTP Layer.\n");
	HTTP->Lock();
	printf("Stopping HTTP Layer.\n");
 	HTTP->Quit();
	delete HTTP;
	printf("http: Window: %p\n",Window);
	if (smthead!=NULL) {
		smt_st *cur=smthead;
		while (smthead!=NULL) {
			cur=smthead->next;
			delete smthead;
			smthead=cur;
		}
	}
	printf("~http_protocol end\n");
 }
int32 http_protocol::SpawnThread(BMessage *info)
 {
  return thread;
 }
void http_protocol::Stop()
 {
 	HTTP->Lock();
	 if (HTTP->CacheSys!=NULL) {
	 	printf("Unregistering from cache system.\n");
		 
	 	HTTP->CacheSys->Unregister(HTTP->CacheToken);
	 }
	 
	printf("done unregistering.\n");
 }
void http_protocol::AddMenuItems(BMenu *menu) {
	printf("http_protocol AddMenuItems\n");
	if (Window!=NULL) {
		Window->Lock();
		Window->AddHandler(HOH);
		HOH->browserident->SetTargetForItems(HOH);
		
		printf("handler added\n");
		
		Window->Unlock();
	}
	
//	BMenuItem *options=menubar->FindItem("Options");
//	printf("http_protocol: options %p\n",options);
	
		HOH->AddMenu(menu);
	
	
}
void http_protocol::RemoveMenuItems() {
	HOH->RemoveMenu();
	
}

int32 http_protocol::GetURL(BMessage *info)
 {
 	BView *vw;
	info->FindPointer("window",(void**)&win);
	 if (Window==NULL)
	 	Window=win;
	 
	info->FindPointer("top_view",(void**)&vw);
 	if (view==NULL) {
		view=vw;
			BMenu *v;
			info->FindPointer("options_menu",(void**)&v);
//			HOH->AddMenu(v);
//			win->AddHandler((BHandler*)HOH);
		
	} else {
		if (vw!=view) {
			BMenu *v;
			info->FindPointer("options_menu",(void**)&v);
//			HOH->AddMenu(v);
			
			view=vw;
		}
		
	}
	
	
/*
 	if (TCP==NULL)
	 if (info->HasPointer("tcp_layer_ptr"))
	 	info->FindPointer("tcp_layer_ptr",(void**)&TCP);
*/
		printf("trying to lock HTTP\n");
	 
		HTTP->Lock();
		printf("lock successful\n");
	 
	 printf("TCP %p\n HTTP::TCP %p\n",TCP,HTTP->TCP);
/*	
	if (HTTP->TCP==NULL)
		HTTP->SetTCP(TCP);
	 else
	  {
	  	if (TCP!=HTTP->TCP)
			HTTP->SetTCP(TCP);//TCP layer pointer changed... Why? Update HTTP layer
	  }
*/
	 
	  
	printf("(http) TCP layer: %p\n",HTTP->TCP);
	HTTP->AddRequest(info);
	 HTTP->Unlock();
	 
//	BString url,host,uri;
//	 uint16 port;
//	 info->FindString("target_url",&url);

//	 bool secure=false;
//	 FindURI(url.String(),host,&port,uri,&secure);
//	 printf("host: %s\nport: %d\nuri: %s\nsecure: %s\n",host.String(),port,uri.String(),secure? "yes":"no");
//	 connection *conn=TCP->ConnectTo('http',(char*)host.String(),port,secure);
//	 BString out="GET ";
//	 out << uri<<" HTTP/1.1\r\nUser-Agent: Themis (http://themis.sourceforge.net\r\nHost: ";
//	 out << host;
	 
//	 if ((port!=80) && (port!=443))
//	 	{
//			out += ":";
//	 out+=port;
//		}
		
//	 out << "\r\nconnection: keep-alive\r\n\r\n";
//	 TCP->Send(&conn,(unsigned char*)out.String(),out.Length());
/*	 if (TCP->DataWaiting(conn))
	  {
		unsigned char *buff=new unsigned char[10000];
		while (TCP->DataWaiting(conn)) {
		 memset(buff,0,10000);
			if(TCP->Receive(&conn,buff,9999)>0)
			printf("%s\n",buff);
		}
		delete buff;
		  
	  }
*/	  
  return 0;
 }
int32 http_protocol::ThreadFunc(void *info)
 {
  return 0;
 }
void http_protocol::FindURI(const char *url,BString &host,uint16 *port,BString &uri,bool *secure)
 {
printf("finding uri...\n");
	 
  BString master(url),servant;
  int32 href,urltype,slash,colon,quote;
  href=master.IFindFirst("href=\"");
  if (href!=B_ERROR)
   {
    master.CopyInto(servant,href+6,master.Length()-6);
    master=servant;
    quote=master.IFindFirst("\"");
    master.CopyInto(servant,0,quote);
    master=servant;
   }
  urltype=master.IFindFirst("://");
  if (urltype!=B_ERROR)
   {
    master.CopyInto(servant,urltype+3,master.Length()-urltype+3);
    BString urlkind;
    master.CopyInto(urlkind,0,urltype);
	printf("urlkind: %s\n",urlkind.String());
	if (urlkind.ICompare("https")==0)
		*secure=true;
    master=servant;
   }
  slash=master.IFindFirst("/");
  if (slash!=B_ERROR)
   {
    master.CopyInto(servant,slash,master.Length()-slash);
    uri=servant.String();
    master.CopyInto(servant,0,slash);
    master=servant;
   }
  else
   uri="/";
  colon=master.IFindFirst(":");
  if (colon!=B_ERROR)
   {
    master.CopyInto(servant,colon+1,master.Length()-(colon+1));
    *port=atoi(servant.String());
    master.CopyInto(servant,0,colon);
    master=servant;
   }
  else
   {
	if (*secure)
	 *port=443;
	else   
    *port=80;
   }
   
  host=master.String();
  printf("Done.\n");
	 
  }
