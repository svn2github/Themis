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
tcplayer *TCP;

status_t Initialize(void *info)
 {
  HTTP_proto=new http_protocol;
  TCP=NULL;
  if (info!=NULL) {
	  
	BMessage *imsg=(BMessage *)info;
	 if (imsg->HasPointer("tcp_layer_ptr"))
	 	imsg->FindPointer("tcp_layer_ptr",(void**)&TCP);
	printf("(http) TCP layer: %p\n",TCP);
	 
  }
  
  return B_OK;
 }
status_t Shutdown(bool now)
 {
//  if (now)
	  if (HTTP_proto!=NULL)
	   {
	    HTTP_proto->Stop();
  		delete HTTP_proto;
  	   }
	TCP=NULL;
  return B_OK;
 }
protocol_plugin* GetObject(void)
 {
  if (HTTP_proto==NULL)
   HTTP_proto=new http_protocol();
  return HTTP_proto;
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
http_protocol::http_protocol()
              :protocol_plugin()
 {
 	Window=NULL;
	 
	Go();
		HTTP=new httplayer(TCP);
	if (TCP!=NULL) {
	}
	HTTP->Start();
	HOH=new http_opt_handler;
	
 }
http_protocol::~http_protocol()
 {
  printf("http_protocol destructor\n");
  Stop();
 	HTTP->Quit();
	printf("http: Window: %p\n",Window);
	 
	
  delete HTTP;
 }
int32 http_protocol::SpawnThread(BMessage *info)
 {
  return thread;
 }
void http_protocol::Stop()
 {
	 
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
	
	
 	if (TCP==NULL)
	 if (info->HasPointer("tcp_layer_ptr"))
	 	info->FindPointer("tcp_layer_ptr",(void**)&TCP);
	if (HTTP->TCP==NULL)
		HTTP->SetTCP(TCP);
	 else
	  {
	  	if (TCP!=HTTP->TCP)
			HTTP->SetTCP(TCP);//TCP layer pointer changed... Why? Update HTTP layer
	  }
	  
	printf("(http) TCP layer: %p\n",HTTP->TCP);
	HTTP->AddRequest(info);
	 
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
