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
#include <Autolock.h>
#include "plugman.h"
#include <Message.h>
#include <Locker.h>
#include <Autolock.h>
#include <signal.h>
#ifndef NEWNET
tcplayer *TCP;
#endif
//BMessage *AppSettings=NULL;
//BMessage **AppSettings_p=NULL;
//plugman *PlugMan;
BMessage *HTTPSettings;
//BLocker *lock=NULL;
status_t Initialize(void *info)
 {
// 	lock=new BLocker(true);
// 	BAutolock alock(lock);
// 	if (alock.IsLocked()) {
#ifndef NEWNET
  TCP=NULL;
#endif	 
	HTTPSettings=new BMessage();
//	 PlugMan=NULL;
  if (info!=NULL) {
	  
	BMessage *imsg=(BMessage *)info;
//	if (imsg!=NULL) {
		imsg->PrintToStream();
	  
#ifndef NEWNET
	  
	 if (imsg->HasPointer("tcp_layer_ptr"))
	 	imsg->FindPointer("tcp_layer_ptr",(void**)&TCP);
//	}
//	if (imsg->HasPointer("plug_manager"))
//		imsg->FindPointer("plug_manager",(void**)&PlugMan);
	printf("HTTP_PROTO: TCP layer: %p\n",TCP);
#endif
	http_protocol *proto=NULL;
  proto=new http_protocol(imsg,"HTTP Protocol");
	 if (proto!=HTTP_proto)
		 proto=HTTP_proto;

  }else 
  	HTTP_proto=new http_protocol(NULL,"HTTP Protocol");
//  }
  return B_OK;
 }
status_t Shutdown(bool now)
 {
	printf("HTTP_PROTO: 1234 HTTP Being Shutdown\n");
{
//			BAutolock alock(lock);
//	if (alock.IsLocked()) {
	 
//  if (now)
	  if (HTTP_proto!=NULL)
	   {
	    HTTP_proto->Stop();
  		delete HTTP_proto;
  	   }
//	if (AppSettings_p!=NULL) {
//
//		if (!AppSettings->HasMessage("http_settings_message")) {
//			AppSettings->AddMessage("http_settings_message",HTTPSettings);
//		} else
//			AppSettings->ReplaceMessage("http_settings_message",HTTPSettings);
//	}
	delete HTTPSettings;
#ifndef NEWNET
	 
	TCP=NULL;
#endif
//	PlugMan=NULL;
//}
 }
 
//  delete lock;
  return B_OK;
 }
protocol_plugin* GetObject(void)
 {
  if (HTTP_proto==NULL)
   HTTP_proto=new http_protocol();
  return HTTP_proto;
 }
void quitalarm(int signum)
{
	printf("HTTP_PROTO: it went and hung on me, damn it! It's so dead now...\n");
	kill_thread(HTTP_proto->HTTP->thread);
	
}

int32 http_protocol::Type() 
{
	return HTTP_PROTOCOL;
}
status_t http_protocol::BroadcastReply(BMessage *msg){
	printf("HTTP_PROTO: BroadcastReply\n");
	
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
					printf("HTTP_PROTO: CacheObjectNotFound\n");
					printf("HTTP_PROTO: reply from cache received.\n");
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
	
//	printf("http_protocol::ReceiveBroadcast()\n");
//	msg->PrintToStream();
	
	status_t stat=B_ERROR;
	BAutolock alock(lock);
	if (alock.IsLocked()) {
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
//		printf("HTTP_PROTO: command is retrieve\n");
	if (command==COMMAND_INFO)
//		printf("HTTP_PROTO: command is info\n");
	switch (command) {
		case COMMAND_RETRIEVE: {
			int32 action=0;
			BMessage *rmsg=new BMessage(*msg);
//			printf("[http proto retrieve 1] message:\n");
//			rmsg->PrintToStream();
			
//			if (msg->HasInt32("action"))
//				msg->FindInt32("action",&action);
//			printf("http proto: action is %c%c%c%c\n",action>>24,action>>16,action>>8,action);
//			BString targ;
//			rmsg->FindString("target_url",&targ);
//			printf("http proto: target url: %s\n",targ.String());
			
//			HTTP->Lock();
//			printf("http proto: http locked\n");
//			http_request *request=NULL;
//			while (request==NULL) {
				BString url;
				rmsg->FindString("target_url",&url);
				if (url.ICompare("http",4)==0)
					HTTP->AddRequest(rmsg);
//				if (request==NULL)
//					snooze(25000);
//			}
//			printf("http proto: add request done\n");
//			HTTP->Unlock();
			//we're already deleting rmsg (as info) int he HTTP->AddRequest() call.
//			delete rmsg;
			//rmsg=NULL;
		}break;
		case COMMAND_INFO: {
//			printf("HTTP_PROTO: Info what: %ld\n",msg->what);
			switch(msg->what) {
				case B_QUIT_REQUESTED: {
					printf("HTTP_PROTO: has received B_QUIT_REQUESTED\n");
//					HTTP->Lock();
					 if (HTTP->CacheSys!=NULL)
					 	HTTP->CacheSys->Unregister(HTTP->CacheToken);
					HTTP->CacheSys=NULL;
//					HTTP->Unlock();
				}break;
				case LoadingNewPage: {
//					printf("HTTP_PROTO: Do something useful when winview says we're loading a new page.\n");
				}break;
				case PlugInUnLoaded: {
					uint32 type=0;
					type=msg->FindInt32("type");
					switch(type) {
						case (TARGET_CACHE|TYPE_RAM|TYPE_DISK): {
//							HTTP->Lock();
							HTTP->CacheToken=0;
							HTTP->CacheSys=NULL;
//							HTTP->Unlock();
							printf("HTTP_PROTO: Cache system unloaded.\n");
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
							if (pobj->PlugID()=='cash') {
//								HTTP->Lock();
										HTTP->CacheSys=(CachePlug*)pobj;
										HTTP->CacheToken=HTTP->CacheSys->Register(Type(),"HTTP Protocol Add-on");
										printf("HTTP_PROTO: *** Loaded CacheToken & object: %lu %p\n",HTTP->CacheToken,HTTP->CacheSys);
//								HTTP->Unlock();
							}
							
							if (((pobj->BroadcastTarget()&TARGET_HANDLER)==pobj->BroadcastTarget()) || ((pobj->BroadcastTarget()&TARGET_PARSER)==pobj->BroadcastTarget())) {
								if (PlugMan!=NULL) {
									BMessage *amsg=new BMessage(GetSupportedMIMEType);
									amsg->AddInt32("ReplyTo",pobj->BroadcastTarget());
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
					;
//					msg->PrintToStream();
					//display the message info, but otherwise ignore it
				}
			}
		}break;
		default:
			return PLUG_DOESNT_HANDLE;
	}
	}
//	printf("http_protocol::ReceiveBroadcast() exiting\n");
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
void http_protocol::Heartbeat() {
	HTTP->CookieMonster->ClearExpiredCookies();
//	HTTP->CookieMonster->PrintCookies();
}
http_protocol::http_protocol(BMessage *info,const char *msg_sys_name)
              :protocol_plugin(info,msg_sys_name)
 {
 	lock=new BLocker(true);
	 
 	HTTP_proto=this;
 	AppSettings=NULL;
 	AppSettings_p=NULL;
	info->FindPointer("settings_message_ptr",(void**)&AppSettings_p);
	if (AppSettings_p!=NULL)
		AppSettings=*AppSettings_p;
	else {
		AppSettings=NULL;
	}
	 printf("HTTP_PROTO: @@!!\tAppSettings_p %p\tAppSettings %p\n",AppSettings_p,AppSettings);
 	uses_heartbeat=true;
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
#ifndef NEWNET
	if (TCP==NULL) {
	 if (info->HasPointer("tcp_layer_ptr"))
	 	info->FindPointer("tcp_layer_ptr",(void**)&TCP);
	}
		HTTP=new httplayer(TCP,this);
#else
	TCPMan=NULL;
		if (info->HasPointer("tcp_manager"))
			info->FindPointer("tcp_manager",(void**)&TCPMan);
		
	HTTP=new httplayer(TCPMan,this);
	 
#endif

		HTTP->Proto=this;
	HTTP->Start();
	HOH=new http_opt_handler;
	
 }
http_protocol::~http_protocol() {
	lock->Lock();
	
	printf("HTTP_PROTO:  destructor\n");
	 printf("HTTP_PROTO: @@##\tAppSettings_p %p\t%AppSettings %p\n",AppSettings_p,AppSettings);
//	Stop();
	printf("HTTP_PROTO: Locking HTTP Layer.\n");
//	HTTP->Lock();
	printf("HTTP_PROTO: Stopping HTTP Layer.\n");
	signal(SIGALRM,&quitalarm);
	set_alarm(5000000,B_ONE_SHOT_RELATIVE_ALARM);
	
 	HTTP->Quit();
	set_alarm(B_INFINITE_TIMEOUT,B_PERIODIC_ALARM);
	
	delete HTTP;
	printf("HTTP_PROTO: Window: %p\n",Window);
	if (smthead!=NULL) {
		smt_st *cur=smthead;
		while (smthead!=NULL) {
			cur=smthead->next;
			delete smthead;
			smthead=cur;
		}
	}
	printf("HTTP_PROTO: ~http_protocol end\n");
	lock->Unlock();
	delete lock;
	
 }
uint32 http_protocol::BroadcastTarget() {
//	printf("HTTP\n");
	return MS_TARGET_HTTP_PROTOCOL;
}
int32 http_protocol::SpawnThread(BMessage *info)
 {
  return thread;
 }
void http_protocol::Stop()
 {
// 	HTTP->Lock();
	 if (HTTP->CacheSys!=NULL) {
	 	printf("HTTP_PROTO: Unregistering from cache system.\n");
		 
	 	HTTP->CacheSys->Unregister(HTTP->CacheToken);
	 }
	printf("HTTP_PROTO: done unregistering.\n");
	 if (((*AppSettings_p)!=NULL) && (*AppSettings_p==AppSettings)) {
	 	if (AppSettings->HasMessage("cookie_settings"))
	 		AppSettings->ReplaceMessage("cookie_settings",HTTP->CookieMonster->CookieSettings);
	 	else
	 		AppSettings->AddMessage("cookie_settings",HTTP->CookieMonster->CookieSettings);
	 }
 }
void http_protocol::AddMenuItems(BMenu *menu) {
	printf("HTTP_PROTO: Window %p\n",Window);
	BAutolock alock(Lock);
	if (alock.IsLocked()) {
	printf("HTTP_PROTO: AddMenuItems\n");
	if (Window!=NULL) {
//		Window->Lock();
		Window->AddHandler(HOH);
		HOH->browserident->SetTargetForItems(HOH);
		
		printf("HTTP_PROTO: handler added\n");
		
//		Window->Unlock();
	}
	
//	BMenuItem *options=menubar->FindItem("Options");
//	printf("http_protocol: options %p\n",options);
	
		HOH->AddMenu(menu);
	
	}
}
void http_protocol::RemoveMenuItems() {
	BAutolock alock(Lock);
	if (alock.IsLocked()) {
	HOH->RemoveMenu();
	}
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
		printf("HTTP_PROTO: trying to lock HTTP\n");
	 
//		HTTP->Lock();
		printf("HTTP_PROTO: lock successful\n");
	 
//	 printf("TCP %p\n HTTP::TCP %p\n",TCP,HTTP->TCP);
/*	
	if (HTTP->TCP==NULL)
		HTTP->SetTCP(TCP);
	 else
	  {
	  	if (TCP!=HTTP->TCP)
			HTTP->SetTCP(TCP);//TCP layer pointer changed... Why? Update HTTP layer
	  }
*/
	 
	  
//	printf("(http) TCP layer: %p\n",HTTP->TCP);
	HTTP->AddRequest(info);
//	 HTTP->Unlock();
	 
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
printf("HTTP_PROTO: finding uri...\n");
	 
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
	printf("HTTP_PROTO: urlkind: %s\n",urlkind.String());
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
  printf("HTTP_PROTO: Done.\n");
	 
  }
void http_protocol::ConnectionEstablished(connection *conn)
{

	printf("HTTP_PROTO: Connection established!!\n");
	BAutolock alock(lock);
	if (alock.IsLocked()) {
#ifndef NEWNET
		HTTP->ConnectionEstablished(conn);
#endif
	}
	
}

void http_protocol::ConnectionDisconnected(connection *conn,uint32 reason)
{
	BAutolock alock(lock);
	if (alock.IsLocked()) {
#ifndef NEWNET
		
	HTTP->ConnectionClosed(conn);
#endif
	}
	
}

void http_protocol::DataWaiting(connection *conn)
{
	BAutolock alock(lock);
	if (alock.IsLocked()) {
#ifndef NEWNET
		
	HTTP->DReceived(conn);
#endif
	}
	
}
#ifdef NEWNET
#warning New Networking System Enabled
using namespace _Themis_Networking_;

void http_protocol::ConnectionEstablished(Connection *connection)
{
	HTTP->ConnectionEstablished(connection);
	
}
void http_protocol::ConnectionAlreadyExists(Connection *connection)
{
	HTTP->ConnectionAlreadyExists(connection);
}
void http_protocol::ConnectionTerminated(Connection *connection)
{
	HTTP->ConnectionTerminated(connection);
}
void http_protocol::DataIsWaiting(Connection *connection)
{
	HTTP->DataIsWaiting(connection);
}
void http_protocol::ConnectionError(Connection *connection)
{
	HTTP->ConnectionError(connection);
}
void http_protocol::ConnectionFailed(Connection *connection)
{
	HTTP->ConnectionFailed(connection);
}
void http_protocol::DestroyingConnectionObject(Connection *connection)
{
	HTTP->DestroyingConnectionObject(connection);
} 

#endif
