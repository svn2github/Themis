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
#include <Debug.h>
#include "httplayer.h"
#include <string.h>
#include <stdio.h>
#include <Alert.h>
//#include <cmath.h>
#include "stripwhite.h"
#include "authvw.h"
#include "http_proto.h"
#include <stdexcept>
#include <time.h>
#include "base64.h"
httplayer *meHTTP;
#ifdef NEWNET
using namespace _Themis_Networking_;
#endif
#ifndef NEWNET
tcplayer *__TCP;
void ConnectClosed(connection *conn)
{
#ifdef DEBUG
	printf("ConnectClosed\n");
#endif
	meHTTP->ConnectionClosed(conn);
}

void DataReceived(connection *conn) 
{
//	printf("http: callback function\n");
	meHTTP->DReceived(conn);
//	printf("http: callback done.\n");
}
#endif
/*
int32 LockHTTP(int32 timeout)
{
	return meHTTP->Lock(timeout);
}
void UnlockHTTP(void) 
{
	meHTTP->Unlock();
}
*/
sem_id httplayer::httplayer_sem=0;

#ifndef NEWNET
httplayer::httplayer(tcplayer *_TCP,http_protocol *protoclass) 
#else
httplayer::httplayer(TCPManager *_TCP,http_protocol *protoclass)
#endif
{
	Proto=protoclass;
	CacheToken=0;
	CacheSys=(CachePlug*)(Proto->PlugMan->FindPlugin(CachePlugin));
	if (CacheSys!=NULL) {
		CacheToken=CacheSys->Register(Proto->Type(),"HTTP Protocol Add-on");
	}
	Proto->Debug("HTTP layer starting up");
#ifdef DEBUG
	printf("CacheToken & object: %lu %p\n",CacheToken,CacheSys);
#endif
	lock=new BLocker(false);
	lock->Lock();
#ifdef DEBUG
	printf("HTTP object at %p\nhttp tcp: %p\n",this,_TCP);
#endif
	TRoster=new BTranslatorRoster();
	TRoster->AddTranslators(NULL);

#ifndef NEWNET
	TCP=NULL;
	if (_TCP!=NULL)
		SetTCP(_TCP);
	__TCP==_TCP;
#else
	TCPMan=NULL;
	if (_TCP!=NULL)
		SetTCP(_TCP);
	
#endif
	meHTTP=this;
	Basic=NULL;
	requests_head=NULL;
	quit=0;
	http_mgr_sem=create_sem(1,"http_mgr_sem");
	cache_sem=create_sem(1,"cache_sem");
	connhandle_sem=create_sem(1,"connection_handler_sem");
	reqhandle_sem=create_sem(1,"request_handler_sem");
	httplayer_sem=create_sem(1,"httplayer_sem");
	use_useragent=0;
	PluginManager=NULL;
	CookieMonster=new CookieManager();
	lock->Unlock();
}
httplayer::~httplayer() 
{
	lock->Lock();
	http_request *req;
	
	while(requests_head!=NULL) {
		req=requests_head->next;
		delete requests_head;
		requests_head=req;
	}
	req=NULL;
	requests_head=NULL;

	if (Basic!=NULL) {
		auth_realm *realms=Basic->realms_head,*tr;
		while (realms!=NULL) {
			tr=realms->next;
			delete realms;
			realms=tr;
		}
		delete Basic;
		Basic=NULL;
		realms=NULL;
		tr=NULL;
	}
	delete_sem(http_mgr_sem);
	delete_sem(cache_sem);
	
	http_mgr_sem=0;
	delete_sem(connhandle_sem);
	connhandle_sem=0;
	delete_sem(reqhandle_sem);
	delete_sem(httplayer_sem);
	delete TRoster;
	delete CookieMonster;
	printf("Cookie Monster is dead!\n");
	lock->Unlock();
	delete lock;
	
}
#ifndef NEWNET
void httplayer::ConnectionClosed(connection *conn) 
{
	BAutolock alock(lock);
//	while (!alock.IsLocked())
//		snooze(10000);
	if (alock.IsLocked()) {
		http_request *cur=requests_head;
		while (cur!=NULL) {
			if (cur->conn==conn)
				break;
			cur=cur->next;
		}
		if (cur!=NULL) {
			CloseRequest(cur);
		}
		cur=NULL;
	}
}
#endif
#ifndef NEWNET
void httplayer::SetTCP(tcplayer *_TCP) {
//	BAutolock alock(lock);
//	if (alock.IsLocked()) {
		if (_TCP!=NULL) {
			
		TCP=_TCP;
		fflush(stdout);
		TCP->SetDRCallback((int32)'http',DataReceived,NULL,NULL);
		TCP->SetConnectionClosedCB((int32)'http',ConnectClosed);
		}
//	}
}
void httplayer::DReceived(connection *conn) 
{
	BAutolock alock(lock);
	
#ifdef DEBUG
	printf("http: DReceived start.\n");
#endif
	if (alock.IsLocked()) {
		http_request *req=requests_head;
		while (req!=NULL) {
			if ((req->conn==conn) && (req->done==0)) 
				break;
			req=req->next;
			if (quit)
				break;
		}
		if (req!=NULL) {
#ifdef DEBUG
			printf("Data is for URL %s\n",req->url);
#endif
			atomic_add(&req->datawaiting,1);
		}
	}
}
#else
void httplayer::SetTCP(TCPManager *_TCP) {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		TCPMan=_TCP;
		
	}
	
}

#endif

auth_realm *httplayer::FindAuthRealm(char *realm,char *host) {
//	BAutolock alock(lock);
//	if (alock.IsLocked()) {
		if (Basic!=NULL) {
			auth_realm *cur=Basic->realms_head;
			auth_realm *target=NULL;
			while (cur!=NULL) {
				if (cur->realm!=NULL)
					if ((strcasecmp(cur->realm,realm)==0) && (strcasecmp(host,cur->host)==0)){
						target=cur;
						break;
					}
				cur=cur->next;
				if (quit)
					break;
			}
			if (target==NULL) {
				cur=Basic->realms_head;
				while (cur!=NULL) {
					if (cur->realm==NULL)
						if (strcasecmp(host,cur->host)==0) {
							target=cur;
							break;
						}
						
					cur=cur->next;
					
				}
				
			}
			
			return target;
		}
//	}
	return NULL;
}
auth_realm *httplayer::FindAuthRealm(http_request *request) {
//	BAutolock alock(lock);
	auth_realm *realm=NULL;
//	if (alock.IsLocked()) {
		if (request->a_realm!=NULL)
			return request->a_realm;
		if (Basic!=NULL) {
			auth_realm *cur=Basic->realms_head;
			while (cur!=NULL) {
				if (strcasecmp(request->host,cur->host)==0) {
					char *uriloc=NULL;
					if (strncasecmp(cur->baseuri,request->uri,strlen(cur->baseuri))==0) {
						realm=cur;
						break;
					}
					/*
					uriloc=strstr(request->uri,cur->baseuri);
					if (uriloc!=NULL) {
						realm=cur;
						break;
					}
					*/
				}
				if (quit)
					break;
				cur=cur->next;
			}
			
		}
//	}
	return realm;
}

auth_realm *httplayer::AddAuthRealm(http_request *request,char *realm, char *user, char *password) {
	//BAutolock alock(lock);
	auth_realm *rlm=NULL;
//	if (alock.IsLocked()) {
	BString authinfo;
	authinfo<<user<<":"<<password;
	printf("auth info: %s\n",authinfo.String());
	
	int32 size=base64::expectedencodedsize(authinfo.Length());
	printf("expected base-64 size: %ld\n",size);
	
	rlm=new auth_realm;
	printf("setting up auth memory\n");
	
	rlm->auth=new char[size+1];
	memset(rlm->auth,0,size+1);
	printf("base 64 encoding auth\n");
	
	base64::encode((void*)authinfo.String(),(uint32)authinfo.Length(),(void *)rlm->auth,(uint32*)&size);
	
	authinfo="nothing";
//	memset(rlm->auth+size,0,strlen(rlm->auth)-size);
	printf("copying host info\n");
	
	rlm->host=new char[strlen(request->host)+1];
	memset(rlm->host,0,strlen(request->host)+1);
	strcpy(rlm->host,request->host);
/*
	char *bu=strchr(request->uri,'/');
	if (bu!=NULL) {
		size=bu-request->uri;
		rlm->baseuri=new char[size+1];
		memset(rlm->baseuri,0,size+1);
		strncpy(rlm->baseuri,request->uri,size);
	} else {
*/
		rlm->baseuri=new char[2];
		rlm->baseuri[0]='/';
		rlm->baseuri[1]=0;
//	}
#ifdef DEBUG
	printf("base authentication path: %s\n",rlm->baseuri);
#endif
	if (realm!=NULL) {
		
		size=strlen(realm);
		rlm->realm=new char[size+1];
		memset(rlm->realm,0,size+1);
		strcpy(rlm->realm,realm);
	}
	
	if (Basic==NULL) {
		Basic=new auth_req;
		Basic->realms_head=rlm;
	} else {
		auth_realm *cur=Basic->realms_head;
		while (cur->next!=NULL)
			cur=cur->next;
		cur->next=rlm;
	}
	request->a_realm=rlm;
//	ResubmitRequest(request);
//	}
	return rlm;
}
void httplayer::UpdateAuthRealm(auth_realm *realm,char *user,char *pass) {
	//BAutolock alock(lock);
	delete realm->auth;
	realm->auth=NULL;
	BString authinfo;
	authinfo<<user<<":"<<pass;
	uint32 size=base64::expectedencodedsize(authinfo.Length());
	realm->auth=new char[size+1];
	base64::encode((void*)authinfo.String(),authinfo.Length(),realm->auth,&size);
	authinfo="nothing";
}
void httplayer::ClearRequests() {
	//BAutolock alock(lock);
	acquire_sem(reqhandle_sem);
	http_request *curreq=requests_head,*next=NULL;
	requests_head=NULL;
	release_sem(reqhandle_sem);
	//The sem is released so that other processes may continue quicker.
	while(curreq!=NULL) {
#ifdef DEBUG
		printf("clearing request at %p\n",curreq);
#endif
		next=curreq->next;
		delete curreq;
		curreq=next;
	}
	
}
void httplayer::KillRequest(http_request *request) {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
#ifdef DEBUG
	printf("killing request %p\n",request);
#endif
//	acquire_sem(reqhandle_sem);
	http_request *curreq=requests_head;
	if (request==curreq) {
		requests_head=request->next;
		if (!request->done) {
			CloseRequest(request);
			delete request;
		}
		
	} else {
		while (curreq!=NULL) {
			if (curreq->next==request)
				break;
			curreq=curreq->next;
		}
		if (curreq!=NULL) {
			if (!request->done) {
				CloseRequest(request);
			}
			http_request *next=request->next;
			delete request;
			curreq->next=next;
		} else {
			if (!request->done) {
				CloseRequest(request);
			}
			delete request;
		}
	}
//	release_sem(reqhandle_sem);
	}
}

http_request *httplayer::AddRequest(BMessage *info) {

	printf( "ADDREQUEST!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" );

#ifdef DEBUG
	Proto->Debug("AddRequest");
#endif
	info->PrintToStream();
//	BAutolock alock(lock);
	http_request *request=NULL;
//	if (alock.IsLocked()) {
#ifdef DEBUG
	printf("AddRequest: alock is locked\n");
#endif
	int32 what=0;
	if (info!=NULL) {
		if (info->HasInt32("action")) {
			info->FindInt32("action",&what);
			switch(what) {
				case LoadingNewPage: {
					//this really doesn't matter to http...
					if (requests_head!=NULL) {
					}
				}break;
				case ReloadData: {
					//flush the cache of the specified URL...
					if (CacheSys!=NULL) {
						BString url;
						info->FindString("target_url",&url);
						int32 cached_obj=CacheSys->FindObject(CacheToken,url.String());
						if (cached_obj>=0) {
							if (CacheSys->AcquireWriteLock(CacheToken,cached_obj)) {
								CacheSys->RemoveObject(CacheToken,cached_obj);
							}
							
						}
						
					}
					
				}break;
				default: {
#ifdef DEBUG
					printf("\t\t*** Unknown protocol action requested! %ld (%c%c%c%c)\n",what,what>>24,what>>16,what>>8,what);
#endif
				}
			}
		}
		if (PluginManager==NULL) {
			info->FindPointer("plug_manager",(void**)&PluginManager);
		}
		request=new http_request;
		// added by emwe ( hope its correct here )
		// add the unique IDs
		info->FindInt32( "view_id", &request->view_id );
#ifdef DEBUG
		printf( "UID: %ld\n", request->view_id );
#endif
		//
		if (info->HasInt32("browser_string"))
			info->FindInt32("browser_string",&use_useragent);
		if (info->HasString("referrer")) {
			BString refr;
			info->FindString("referrer",&refr);
			request->referrer=new char[refr.Length()+1];
			memset(request->referrer,0,refr.Length()+1);
			strcpy(request->referrer,refr.String());
		}
		if (info->HasPointer("authorization")) {
			info->FindPointer("authorization",(void **)&request->a_realm);
		}
		
		BString url;
		info->FindString("target_url",&url);
		if (url.Length()==0)
			return NULL;
		request->url=new char[url.Length()+1];
		memset(request->url,0,url.Length()+1);
		strcpy(request->url,url.String());
		FindURI(&request->url,&request->host,&request->port,&request->uri,&request->secure);
		if (info->HasString("username:password")) {
			BString usepass;
			info->FindString("username:password",&usepass);
			if (usepass.Length()>0) {
				char *u=new char[usepass.Length()+1],*p=new char[usepass.Length()+1];
				memset(u,0,usepass.Length()+1);
				memset(p,0,usepass.Length()+1);
				int32 colon=usepass.FindFirst(':');
				
				if (colon!=B_ERROR) {
					usepass.CopyInto(u,0,colon);
					usepass.CopyInto(p,colon+1,usepass.Length()-colon);
					request->a_realm=AddAuthRealm(request,NULL,u,p);
					
				}
				memset(u,0,usepass.Length()+1);
				memset(p,0,usepass.Length()+1);				
				delete u;
				delete p;
				
			}
			
		}
		
#ifdef DEBUG
		printf("[http->addrequest] Host: %s\n",request->host);
#endif
		
		if (what==LoadingNewPage) {
#ifdef DEBUG
			printf( "what==LoadingNewPage\n" );
#endif
			BHandler *target;
			info->FindPointer("top_view",(void**)&target);
			BMessenger *msgr=new BMessenger(target,NULL,NULL);
			BMessage *msg=new BMessage(UpdateDisplayedURL);
			msg->AddString("url",request->url);
			msgr->SendMessage(msg);
			delete msgr;
			delete msg;
		}
		if (request->secure) {
#ifndef USEOPENSSL
// We don't have OpenSSL.
#ifdef DEBUG
		printf("OpenSSL not supported in this build.");
#endif
		(new BAlert("SSL Not Supported","I'm afraid that this version of Themis\nwas built without OpenSSL support.\nUnfortunately, it means that you can't\nvisit HTTPS sites.","Damn."))->Go();
		delete info;
		delete request;
		request=NULL;
		return request;
#else
// We have and want to be able to use OpenSSL in Themis.
#ifdef DEBUG
		printf("Request is to: %s:%u\nURI: %s\n",request->host,request->port,request->uri);
#endif
		delete info;
#ifdef DEBUG
		printf("End AddRequest\n");
#endif
#endif
		} else {
#ifdef DEBUG
			printf("Request is to: %s:%u\nURI: %s\n",request->host,request->port,request->uri);
#endif
			delete info;
#ifdef DEBUG
			printf("End AddRequest\n");
#endif
		}
		request->requeststr=BuildRequest(request);
#ifdef DEBUG
printf("http:SendRequest trying to lock TCP\n");
#endif
	
//					TCP->Lock();
//					printf("tcp is locked\n");
//			TryConnecting:
#ifdef DEBUG
			printf("trying to connect\n");
#endif
#ifndef NEWNET
			request->conn=TCP->ConnectTo(Proto,'http',request->host,request->port,request->secure);
#else
			request->conn=TCPMan->CreateConnection(Proto,request->host,request->port,request->secure);
#endif
#ifdef DEBUG
			printf("[http] Connection struct: %p\n",request->conn);
#endif
//		delete requeststr;
//#ifdef DEBUG
//		printf("requeststr deleted. returning request %p (EOF AddRequest)\n",request);
//#endif
//		requeststr=NULL;
		return request;
	} else
		return NULL;
//	}
	return request;
}
char *httplayer::FindEndOfHeader(char *buffer, char **eohc) {
	//BAutolock alock(lock);
	if (buffer==NULL)
		return NULL;
	char *eoh=NULL;
	char hstr[5];
	strcpy(hstr,"\r\n\r\n");
	int trial=0;
	LookForEndOfHeader:
	eoh=strstr(buffer,hstr);
	if (eoh==NULL) {
				memset(hstr,0,5);
		switch(trial) {
			case 0: {
				strcpy(hstr,"\r\r");
			}break;
			case 1: {
				strcpy(hstr,"\n\n");
			}break;
			case 2: {
				return NULL;
			}break;
		}
		trial++;
		goto LookForEndOfHeader;
	}
	if ((eohc!=NULL) && (eoh!=NULL)) {
		char *es=*eohc;
		memset(es,0,strlen(hstr)+1);
		memcpy(es,hstr,strlen(hstr));
	}
	
	return eoh;
}
header_st *httplayer::AddHeader(http_request *request, char *attribute, char* value) {
	//BAutolock alock(lock);
//	printf("attribute: %s\t%d bytes\nvalue: %s\t%d bytes\n",attribute,strlen(attribute),value,strlen(value));
	
	header_st *cur=request->headers;
	if (cur!=NULL) {
		while (cur->next!=NULL)
			cur=cur->next;
		cur->next=new header_st;
		cur=cur->next;
		int len=strlen(attribute);
		cur->name=new char[len+1];
		memset(cur->name,0,len+1);
		len=strlen(value);
		if (len==0)
			len=1;
		
		cur->value=new char[len+1];
		memset(cur->value,0,len+1);
		strcpy(cur->name,attribute);
		strcpy(cur->value,value);
	} else {
		cur=new header_st;
		int len=strlen(attribute);
		cur->name=new char[len+1];
		memset(cur->name,0,len+1);
		len=strlen(value);
		if (len==0)
			len=1;
		cur->value=new char[len+1];
		memset(cur->value,0,len+1);
		strcpy(cur->name,attribute);
		strcpy(cur->value,value);
		request->headers=cur;
	}
	return cur;
}

char *httplayer::FindHeader(http_request *request,char *attribute, int32 which) {
	//BAutolock alock(lock);
	header_st *cur=request->headers;
	int32 count=0;
	if (cur!=NULL) {
		while (cur!=NULL) {
			if (strcasecmp(attribute,cur->name)==0) {
#ifdef DEBUG
				printf("%ld (%ld) - attribute: %s\n",count,which,attribute);
#endif
				if (count>=which)
					break;
				count++;
			}
			cur=cur->next;
		}
		if (cur!=NULL)
			return cur->value;
		return NULL;
	}
	return NULL;
}
int32 httplayer::CountHeaderOccurances(http_request *request, char *attribute) {
	//BAutolock alock(lock);
	header_st *cur=request->headers;
	int32 count=0;
	if (cur!=NULL) {
		while (cur!=NULL) {
			if (strcasecmp(attribute,cur->name)==0)
				count++;
			cur=cur->next;
		}
	}
	return count;
}
void httplayer::ClearHeaders(http_request *request) {
	//BAutolock alock(lock);
//get rid of the http headers
	if (request!=NULL) {
		header_st *head=request->headers;
		header_st *cur;
		while (head!=NULL) {
			cur=head->next;
			delete head;
			head=cur;
		}
		head=NULL;
		cur=NULL;
		request->headers=NULL;
	}
}

void httplayer::ProcessHeaders(http_request *request,void *buffer,int size) {
	//BAutolock alock(lock);
	bool conthead=false;
	bool triggerauth=false;
		char *header=NULL;
	bool http_continue=false;
	bool altlineending=false;
		char *eoh=NULL;
		char *eohc=new char[5];
		memset(eohc,0,5);
	char lineending[4];
	int32 lesize=0;
	memset(lineending,0,4);
	strcpy(lineending,"\r\n");
	lesize=strlen(lineending);
	char *buf=(char*)buffer;
	if ((request->headers!=NULL) || (request->storage!=NULL)){
			request->storagesize+=size;
			request->storage=(unsigned char*)realloc(request->storage,request->storagesize);
			memcpy(request->storage+(request->storagesize-size),buffer,size);
			buf=(char*)request->storage;
			eoh=FindEndOfHeader(buf,(&eohc));
			if (eoh==NULL) {
				//what??? again????
				delete eohc;
				return;
			} else {
//				int len=eoh-buf;
//				header=new char[len+1];
//				memset(header,0,len+1);
//				strncpy(header,buf,len);
			}
#ifdef DEBUG
		printf("buf==request->storage\n");
#endif
		
	}
	
#ifdef DEBUG
	printf("ProcessHeaders getting started:\n%s\n",buf);
#endif
	
	char *tmp=new char[5];
	memset(tmp,0,5);
	strncpy(tmp,buf,4);
	if ((strcasecmp(tmp,"HTTP")==0) || (request->storage!=NULL)) {
		delete tmp;
//		sscanf(buf,"HTTP/%d.%d %d",&request->http_v_major,&request->http_v_minor,&request->status);
		{
			char *sl=NULL, *sp1=NULL,*eol=NULL;
			sl=strchr(buf,'/');
			sp1=strchr(buf,32);
			eol=strstr(buf,lineending);
			if (eol==NULL) {
				//some stupid web sites don't do the full "\r\n" on the status line... 
				altlineending=true;
				memset(lineending,0,4);
				strcpy(lineending,"\n");
				eol=strstr(buf,lineending);
				lesize=strlen(lineending);
			}
			char temp[10];
			memset(temp,0,10);
			strncpy(temp,sl+1,sp1-(sl+1));
			float vers=atof(temp);
//			printf("version %1.1f\n",vers);
			request->http_v_major=(int)vers/1;
			request->http_v_minor=(int)vers%1;
			memset(temp,0,10);
				strncpy(temp,sp1+1,3);
			request->status=atol(temp);
			memset(temp,0,10);
		}
	
//		printf("[ph1] eohc: %p\n",eohc);
		
		eoh=FindEndOfHeader(buf,(&eohc));
//		printf("[ph1] eohc: %p\n",eohc);
		if (eoh!=NULL) {
			int len=eoh-buf;
			header=new char[len+1];
			memset(header,0,len+1);
			strncpy(header,buf,len);
		} else {
			//place the data in a safe place, and come back to it when called again.
			//BE WARE!! THIS IS ONE OF THE FEW PLACES THAT MEMORY IS BEING MALLOC'ed
			//INSTEAD OF NEW'ed.
			if (request->storage==NULL) {
				request->storage=(unsigned char*)malloc(size);
				memcpy(request->storage,buffer,size);
				request->storagesize=size;
				delete eohc;
				return;
			}
		}
		
//		printf("Status: %d, eoh: %p\n",request->status,eoh);
//		printf("EoH str: ");
		if (eohc==NULL)
//		for (int i=0;i<strlen(eohc);i++)
//			printf("0x%x ",(int)eohc[i]);
//		printf("\n");
//		printf("header:\n%s\n",header);
		switch((request->status/100)) {
			case 1: {
				//100 range responses should trigger another look... if there is no further
				//data in this buffer, however, return and treat as if no data was received
				//from remote server.
				if (header!=NULL)
					delete header;
				if (eoh!=NULL)
					eoh=NULL;
				delete eohc;
				return;
			}break; 
			case 2: {
				//200 range response are normal responses.
			}break;
			case 3: {
				//300 range responses mean that we are looking in the wrong place and/or
				//are being redirected to the new location
			}break;
			case 4: {
				//400 range responses require authorization to access the content
				//or the content isn't available; some client error
			}break;
			case 5: {
				//500 range responses mean that we did something wrong. It's an error condition
				//on the server, typically.
			}break;
			default: {
			}
		}
		char *endoh=header+strlen(header);
		char *eol=strstr(header,lineending);//get to the end of the first line; the status line
		char *curpos=eol+lesize;
		eol=strstr(curpos,lineending);
		
		char *colon;
//		printf("curpos: %p\teol: %p\teoh: %p\n",curpos,eol,eoh);
		while(eol!=NULL) {
			if (curpos==eol)
				break;
			colon=strchr(curpos,':');
			if (colon!=NULL) {
				char *a=NULL,*v=NULL,*v2=NULL;
				int len=colon-curpos;
				a=new char[len+1];
				memset(a,0,len+1);
				strncpy(a,curpos,len);
//				printf("%s (%d)\n",a,len);
				len=eol-(colon+1);
				v=new char[len+1];
				memset(v,0,len+1);
				strncpy(v,colon+1,len);
//				printf("%s (%d)\n",v,len);
				v2=trim(v);
				//printf("%s (%d)\n",v2,strlen(v2));
				delete v;
				v=NULL;
				AddHeader(request,a,v2);
				delete a;
				delete v2;
				a=v2=NULL;
			}
			curpos=eol+lesize;
			eol=strstr(curpos,lineending);//find the next end of line
		}
		//then do  it one more time to pick up the last header...
		{
			char *a=NULL, *v=NULL, *v2=NULL;
			int len=0;
			colon=strchr(curpos,':');
			len=colon-curpos;
			a=new char[len+1];
			memset(a,0,len+1);
			strncpy(a,curpos,len);
			len=strlen(colon+1);
			v=new char[len+1];
			memset(v,0,len+1);
			strncpy(v,colon+1,len);
			v2=trim(v);
			delete v;
			v=NULL;
			AddHeader(request,a,v2);
			delete a;
			delete v2;
			a=v2=NULL;
		}
#ifdef DEBUG
		if (request->headers!=NULL)
			printf("Headers added to request struct\n");
#endif
		
		int status=request->status/100;
		switch(status) {
			case 1: {
			}break;
			case 2: {
				if (request->status==204) {
					BString error;
					error<<"This url \""<<request->url<<"\" returned a status code 204: No content. This is not an error even though it looks like one.";
					
					(new BAlert("Status 204",error.String(),"What the?"));
					CloseRequest(request);
					return;
				}
				
			}break;
			case 3: {
				if (request->status==304) {
					//update the cache, show the current data, and then return.
					DoneWithHeaders(request);
					
					return;
					
				}
				
				if (request->headers!=NULL) {
					char *destination=FindHeader(request,"Location");
					int len=strlen(destination);
					char *loc=new char[len+1];
					memset(loc,0,len+1);
					strcpy(loc,destination);
					for (int i=0;i<len;i++)
						loc[i]=toupper(loc[i]);
					if (strncmp("HTTP",loc,4)==0) {
						//absolute location
						BString oldurl;
						BString oldrefer;
						if (request->referrer!=NULL) {
							oldrefer=request->referrer;
							delete request->referrer;
							
						}
							request->referrer=new char[oldurl.Length()+1];
							memset(request->referrer,0,oldurl.Length()+1);
							strcpy(request->referrer,oldurl.String());
						oldurl=request->url;
						
						memset(request->url,0,strlen(request->url)+1);
						delete request->url;
						request->url=new char[strlen(destination)+1];
						memset(request->url,0,strlen(destination)+1);
						strcpy(request->url,destination);
						FindURI(&request->url,&request->host,&request->port,&request->uri,&request->secure);
//						ClearHeaders(request);
#ifdef DEBUG
						printf("Being redirected to %s\n",request->url);
#endif
//*** added 23-Feb-03
						DoneWithHeaders(request,true);
						ResubmitRequest(request);
						delete loc;
						loc=NULL;
						delete eohc;
						return;
					} else {
						delete loc;
						loc=NULL;
							BString url("http");
							if (request->secure)
								url << "s";
							url << "://"<<request->host<<":"<<request->port;
						if (destination[0]=='/') {
							
							url<<destination;
						} else {
									char *lslash=strrchr(request->uri,'/');
									int32 len=0;
									if (lslash!=NULL){
										
										len=lslash-request->uri;
										char *cp=new char[len+1];
										memset(cp,0,len+1);
										strncpy(cp,request->uri,len);
										url<<cp;
										memset(cp,0,len+1);
										
										delete cp;
										cp=NULL;
										
									}
								if (destination[0]=='.') {
									
									char *slash=strchr(destination,'/');
									if (slash!=NULL) {
										url<<slash;
										
									}
									
									slash=NULL;
									
								} else
									url<<'/'<<destination;
#ifdef DEBUG
							printf("%d redirect to: %s\n",request->status,url.String());
#endif
						}
						
						
//						if (destination[0]=='/') {
							//relative location
							memset(request->url,0,strlen(request->url)+1);
							delete request->url;
							request->url=new char[url.Length()+1];
							memset(request->url,0,url.Length()+1);
							strcpy(request->url,url.String());
							FindURI(&request->url,&request->host,&request->port,&request->uri,&request->secure);
//*** added 23-Feb-03
							DoneWithHeaders(request,true);
							ClearHeaders(request);
#ifdef DEBUG
							printf("Being redirected to %s\n",request->url);
#endif
							if (request->cacheinfo!=NULL) {
								delete request->cacheinfo;
								request->cacheinfo=NULL;
							}
							if (request->cache!=UsesCache)
								request->cache=UsesCache;
//							if (request->data!=NULL) {
//								delete request->data;
//								request->data=NULL;
//							}
							ResubmitRequest(request);
							delete eohc;
							return;
//						}
					}
				}
			}break;
			case 4: {
/*
				if (request->status==401) {
					request->a_realm=FindAuthRealm(request);
					
					if (request->a_realm!=NULL) {
						(new authwin("Password required!",request,request->a_realm->realm));
						ClearHeaders(request);
						ResubmitRequest(request);
						delete eohc;
						return;
					}
				}
*/
			}break;
			default: {
				//do nothing
			}
		}
//		printf("ProcessHeaders is just about done:\n%s\n",eoh+strlen(eohc));
		
		DoneWithHeaders(request);
		ProcessData(request,eoh+strlen(eohc),size-((eoh+strlen(eohc))-buf));
		delete eohc;
	} else {
		//we don't have an HTTP response on our hands...
		delete tmp;
	}
//	printf("ProcessHeaders done.\n");
}

void httplayer::DoneWithHeaders(http_request *request,bool nocaching) {
	//BAutolock alock(lock);
#ifdef DEBUG
	printf("DoneWithHeaders\n");
#endif
	printf("Headers:\n");
	printf("\tHTTP Status:\t%u\n",request->status);
	printf("\tURL:\t%s\n",request->url);
	printf("\t==============\n");
	{
		header_st *curh=request->headers;
		while(curh!=NULL) {
			printf("\t%s:\t%s\n",curh->name,curh->value);
			curh=curh->next;
		}
		
	}
	
	atomic_add(&request->headersdone,1);
	char *result;
	result=FindHeader(request,"transfer-encoding");
	if (result!=NULL) {//transfer encoding
		if (strcasecmp(result,"chunked")==0)
			request->chunked=true;
#ifdef DEBUG
		printf("chunked: %d\n",request->chunked);
#endif
	}//transfer encoding
	result=FindHeader(request,"content-length");
	if ((!request->chunked) && (result!=NULL)) {//content length
		//according to specs, a content-length and chunked transfer encoding can't be
		//specified together.
		request->contentlen=request->bytesremaining=atol(result);
	}//content length
	if (!nocaching) {
		result=FindHeader(request,"cache-control");
		if (result!=NULL) {//Cache control header
					if (strcasecmp(result,"no-cache")==0) {
						request->cache=DoesNotUseCache;
					}
		}//Cache control header
	}
	//COOKIES!!!!
	int32 count=CountHeaderOccurances(request,"set-cookie");
	if (count>0) {
		BString header;
		for (int32 i=0; i<count; i++) {
			result=FindHeader(request,"set-cookie",i);
#ifdef DEBUG
			printf("Result of cookie search: %s\n",result);
#endif
			header="Set-Cookie: ";
			header<<result;
			CookieMonster->SetCookie(header.String(),request->host, request->uri);
		}
	}
	count=CountHeaderOccurances(request,"set-cookie2");
	if (count>0) {
		BString header;
		for (int32 i=0; i<count; i++) {
			result=FindHeader(request,"set-cookie2",i);
			header="Set-Cookie2: ";
			header<<result;
			CookieMonster->SetCookie(header.String(),request->host, request->uri);
		}
	}
	//COOKIES!!
	result=FindHeader(request,"Pragma");
	if (result!=NULL) {//Pragma Header
				if (!nocaching)
					if  (strcasecmp(result,"no-cache")==0) {
						request->cache=DoesNotUseCache;
					}
	}//Pragma Header
	result=FindHeader(request,"www-authenticate");
	if ((result!=NULL) && (request->status==401)) {//Authentication Header
				if (strncasecmp(result,"basic",5)==0) {
					char *q1=NULL,*q2=NULL;
					q1=strchr(result,'"');
					q2=strchr(q1+1,'"');
					if ((q1!=NULL) && (q2!=NULL)) {
						int size=q2-(q1+1);
							char *rlmc=new char[size+1];
							memset(rlmc,0,size+1);
							strncpy(rlmc,q1+1,size);
#ifdef DEBUG
							printf("auth realm: %s\n",rlmc);
#endif
						if (Basic!=NULL) {
							auth_realm *realm=FindAuthRealm(rlmc,request->host);
							if (realm!=NULL) {
#ifdef DEBUG
								printf("auth realm found: %p\n",realm);
#endif
								if (realm==request->a_realm) {
									if (request->awin==NULL)
										request->awin=new authwin("Password required!",request,rlmc,true);
									else
										request->awin->Activate(true);
								} else
									request->awin=new authwin("Password required!",request,rlmc);
#ifdef DEBUG
								printf("done with auth header.\n");
#endif
								
							} else {
#ifdef DEBUG
								printf("auth realm must be added\n");
#endif
								if (request->awin==NULL)
									request->awin=new authwin("Password required!",request,rlmc);
								else
									request->awin->Activate(true);
								
							}
						} else {
#ifdef DEBUG
							printf("auth realm must be added\n");
#endif
								if (request->awin==NULL)
									request->awin=new authwin("Password required!",request,rlmc);
								else
									request->awin->Activate(true);
						}
						delete rlmc;
					}
					
				}
	}//Authentication header
	result=NULL;
	if (request->status==401)
		request->cache=DoesNotUseCache;
#ifdef DEBUG
	printf("cacheinfo: %p\t cache object token: %ld\thttp status: %ld\n",request->cacheinfo,request->cache_object_token,request->status);
#endif
	if (nocaching==false) {
	switch(request->cache) {
		case UsesCache: {
			printf( "HTTP_LAYER: Creating DISK cache file\n" );
			
			if ((request->cacheinfo==NULL) && (request->cache_object_token==B_ERROR)){
				//obviously we didn't find this item in the cache earlier...
				//so create the item...
				if (CacheSys!=NULL) {
					
					BMessage creator(CreateCacheObject);
					creator.AddString("url",request->url);
					creator.AddString("host",request->host);
					creator.AddString("path",request->uri);
					header_st *curhead=request->headers;
					while (curhead!=NULL) {
						creator.AddString(curhead->name,curhead->value);
						curhead=curhead->next;
					}
					creator.AddInt32("ReplyTo",Proto->PlugID());
					creator.AddPointer("ReplyToPointer",Proto);
#ifdef DEBUG
					printf("HTTP_LAYER: about to create cache item\n");
#endif
/*
Even though the CacheObject is created in the next step, the actual disk file isn't created
until/unless data is written to the object; including by writing attributes out.
*/
					request->cache_object_token=CacheSys->CreateObject(CacheToken,request->url);
					
					if( request->cache_object_token )
						printf( "HTTP_LAYER: request->cache_object_token valid!\n" );
					
					CacheSys->SetObjectAttr(CacheToken,request->cache_object_token,&creator);
					request->cacheinfo=CacheSys->GetInfo(CacheToken,request->cache_object_token);
					if (request->cacheinfo!=NULL)
					{
						printf( "HTTP_LAYER: printing request->cacheinfo:\n" );
						request->cacheinfo->PrintToStream();
					}
					else
					{
						printf( "HTTP_LAYER: request->cacheinfo NOT VALID\n" );
					}
				}
				
			} else {
#ifdef DEBUG
				printf("checkpoint!\n");
#endif
				if ((request->cacheinfo!=NULL) && (request->cache_object_token!=B_ERROR) && (request->status==200)) {
					BMessage updater(CreateCacheObject);
					updater.AddString("url",request->url);
					updater.AddString("host",request->host);
					updater.AddString("path",request->uri);
					header_st *curhead=request->headers;
					while (curhead!=NULL) {
						updater.AddString(curhead->name,curhead->value);
						curhead=curhead->next;
					}
					while (!CacheSys->AcquireWriteLock(CacheToken,request->cache_object_token))
						printf("waiting for write lock on cache for %s\n",request->url);
					CacheSys->ClearContent(CacheToken,request->cache_object_token);
					CacheSys->SetObjectAttr(CacheToken,request->cache_object_token,&updater);
				}
				
			}
			
			
		}break;
		case DoesNotUseCache: {
			printf( "HTTP_LAYER: Creating RAM cache file\n" );
			
			//basically, if we're not supposed to create a cache file, create a RAM cache file anyway
			if ((request->cacheinfo==NULL) && (request->cache_object_token==B_ERROR)){
				//obviously we didn't find this item in the cache earlier...
				//so create the item...
				if (CacheSys!=NULL) {
					
					BMessage creator(CreateCacheObject);
					creator.AddString("url",request->url);
					creator.AddString("host",request->host);
					creator.AddString("path",request->uri);
					header_st *curhead=request->headers;
					while (curhead!=NULL) {
						creator.AddString(curhead->name,curhead->value);
						curhead=curhead->next;
					}
					creator.AddInt32("ReplyTo",Proto->PlugID());
					creator.AddPointer("ReplyToPointer",Proto);
#ifdef DEBUG
					printf("12345 about to create cache item 54321\n");
#endif
					request->cache_object_token=CacheSys->CreateObject(CacheToken,request->url,TYPE_RAM);
					CacheSys->SetObjectAttr(CacheToken,request->cache_object_token,&creator);
					request->cacheinfo=CacheSys->GetInfo(CacheToken,request->cache_object_token);
					if (request->cacheinfo!=NULL)
						request->cacheinfo->PrintToStream();
				}
				
			} else {
#ifdef DEBUG
				printf("checkpoint!\n");
#endif
				if ((request->cacheinfo!=NULL) && (request->cache_object_token!=B_ERROR) && (request->status==200)) {
					BMessage updater(CreateCacheObject);
					updater.AddString("url",request->url);
					updater.AddString("host",request->host);
					updater.AddString("path",request->uri);
					header_st *curhead=request->headers;
					while (curhead!=NULL) {
						updater.AddString(curhead->name,curhead->value);
						curhead=curhead->next;
					}
					while (!CacheSys->AcquireWriteLock(CacheToken,request->cache_object_token))
						printf("waiting for write lock on cache for %s\n",request->url);
					CacheSys->ClearContent(CacheToken,request->cache_object_token);
					CacheSys->SetObjectAttr(CacheToken,request->cache_object_token,&updater);
				}
				
			}
		}break;
		default: {
#ifdef DEBUG
			printf("HTTP_LAYER: Unknown cache setting for file.\n");
#endif
		}
		
	}
	}

/*

	if (request->cache!=DoesNotUseCache) {
		header_st *curhead;
		if (request->secure)
			request->cache|=EncryptCache;
		if (request->cacheinfo==NULL) {
			printf("preparing to make new cache\n");
			BMessage *ci=new BMessage(CreateCacheObject);
			curhead=request->headers;
			ci->AddString("url",request->url);
			ci->AddString("host",request->host);
			ci->AddString("path",request->uri);
			while (curhead!=NULL) {
				ci->AddString(curhead->name,curhead->value);
				curhead=curhead->next;
			}
			ci->AddInt32("ReplyTo",Proto->PlugID());
			ci->AddPointer("ReplyToPointer",Proto);
			request->cacheinfo=new BMessage;
			ci->AddInt32("command",COMMAND_STORE);
			BMessage *container=new BMessage();
			container->AddMessage("message",ci);
			bool nocache=false;
			printf("trying to create cache object.\n");
		acquire_sem(cache_sem);//this keeps the http layer from proceeding before a BroadcastReply is called if there is a cache add-on loaded
		printf("sem acquired, sending broadcast\n");
		if (PluginManager->Broadcast( Proto->PlugID(), TARGET_CACHE,container)!=B_OK)
			nocache=true;
		delete container;
		printf("broadcast called, cache received? %s\n",nocache?"no":"yes");

/ * 
	Yes, it looks weird that we're acquiring the cache_sem again, but this makes sure
	that we get a response back from the cache before we continue. We only want to release
	the sem in this function before reacquiring it if there are no cache plug-ins.
* /
		if (nocache)
			release_sem(cache_sem);
		printf("if cache received, I should block here until a response has been received\n");
		acquire_sem(cache_sem);//this keeps the http layer from proceeding before a BroadcastReply is called if there is a cache add-on loaded
			BMessage *reply=NULL;
			if (nocache==false) {
				printf("response received\n");
				reply=Proto->cache_reply;
				reply->what=0;
				Proto->cache_reply=NULL;
			} else
				printf("no cache plug-in responded\n");
			
			release_sem(cache_sem);
			if (reply==NULL) {
				request->data=new BMallocIO;
			} else {
				if (reply->HasString("mime_type")) {
					BString mime;
					reply->FindString("mime_type",&mime);
					request->cacheinfo->AddString("mimetype",mime.String());
				}
				
				if (reply->HasRef("ref")) {
					entry_ref ref;
					reply->FindRef("ref",&ref);
					request->cacheinfo->AddRef("ref",&ref);
				}
				if (reply->HasPointer("data")) {
					void *pointer;
					reply->FindPointer("data",&pointer);
					request->data=(BPositionIO*)pointer;
				}
				
			}
			
//			BMessenger *msgr=new BMessenger(CachePlug->Handler(),NULL,NULL);
//			msgr->SendMessage(ci,request->cacheinfo);
//			delete ci;
//			request->cacheinfo->PrintToStream();
			if (request->data==NULL) {
				entry_ref ref;
				request->cacheinfo->FindRef("ref",&ref);
				request->data=new BFile(&ref,B_READ_WRITE);
			}
		} else {//update the cached object if necessary
			printf("updating cache\n");
			entry_ref ref;
			request->cacheinfo->FindRef("ref",&ref);
			BMessage *ui=new BMessage(UpdateCachedObject);
			BMessage *reply=new BMessage;
			curhead=request->headers;
			ui->AddString("url",request->url);
			ui->AddString("host",request->host);
			ui->AddString("path",request->uri);
			ui->AddRef("ref",&ref);
			BString mime;
			request->cacheinfo->FindString("mime_type",&mime);
			ui->AddString("content-type",mime);
			ui->AddString("mime_type",mime);
			
			while (curhead!=NULL) {
				ui->AddString(curhead->name,curhead->value);
				curhead=curhead->next;
			}
//			BMessenger *msgr=new BMessenger(CachePlug->Handler(),NULL,NULL);
//			msgr->SendMessage(ui/*,reply* /);
//			reply->PrintToStream();
//			delete request->cacheinfo;
//			request->cacheinfo=reply;
			delete ui;
			if (request->data==NULL) {
				printf("Creating/opening cache file.\n");
				request->data=new BFile(&ref,B_READ_WRITE);
				request->data->Seek(0,SEEK_END);
				request->contentlen=request->bytesreceived=0;
				request->contentlen=request->bytesreceived=request->data->Position();
				printf("(http cache) br: %ld\n",request->bytesreceived);
				request->data->Seek(0,SEEK_SET);
			}
			
		}
	} else {
		printf("doesn't use cache\n");
		if (request->data==NULL) {
			request->data=new BMallocIO;
			((BMallocIO*)request->data)->SetBlockSize(1);
			((BMallocIO*)request->data)->SetSize(1);
		}
	}

*/

	if ((request->bytesremaining==-1) && (request->http_v_major==1) && (request->http_v_minor==0)) {
		request->receivetilclosed=true;
	}
	if (request->status==304) {
		BMessage *msg=new BMessage(ReturnedData);
		msg->AddInt32("command",COMMAND_INFO);
		if (!nocaching) {
	if (request->cache_object_token>=0) {
		
			msg->AddInt32("cache_object_token",request->cache_object_token);
			msg->AddInt32("cache_system",request->cache_system_type);
	}
	
		}
		msg->AddString("url",request->url);
		msg->AddBool("request_done",true);
		char *result=NULL;
		BString mime;
		if (!nocaching) {
#ifdef DEBUG
			printf("Need to read file from cache\n");
#endif
			request->cacheinfo->PrintToStream();
			request->cacheinfo->FindString("mime-type",&mime);
		}
		msg->AddString("mimetype",mime);
		
		result=mime.LockBuffer(0);
/*
		if (request->cache!=DoesNotUseCache)
			((BFile*)request->data)->GetSize((off_t *)&request->bytesreceived);
		else
			request->bytesreceived=((BMallocIO*)request->data)->BufferLength();
*/		
		if (!nocaching)
				request->bytesreceived=CacheSys->GetObjectSize(CacheToken,request->cache_object_token);
		
#ifdef DEBUG
		printf("request->bytesreceived: %ld\n",request->bytesreceived);
#endif
		
//		request->contentlen=request->bytesreceived;
		
		int32 target=0;
		if (result!=NULL) {
			msg->AddString("mimetype",result);
			char *slash=strchr(result,'/');
			if (strncasecmp("text",result,slash-result)==0) {
				if (strncasecmp("html",slash+1,strlen(result)-((slash+1)-result))==0) {
					target=MS_TARGET_HTML_PARSER;
				} else
					target=MS_TARGET_HANDLER|MS_TARGET_CONTENT_TEXT|MS_TARGET_PARSER;
			}
			if (strncasecmp("image",result,slash-result)==0) {
				target=MS_TARGET_IMAGE_HANDLER;
			}
			if (strncasecmp("audio",result,slash-result)==0) {
				target=AUDIO_HANDLER;
			}
			if (strncasecmp("video",result,slash-result)==0) {
				target=VIDEO_HANDLER;
			}
			if (strncasecmp("application",result,slash-result)==0) {
				target=TARGET_PARSER|TARGET_HANDLER;
			}
			slash=NULL;
		} else 
			target=MS_TARGET_PARSER|MS_TARGET_HANDLER;
		mime.UnlockBuffer(-1);
		
		
		result=NULL;
		if (request->contentlen==0)
			msg->AddInt64("content-length",request->bytesreceived);
		else
			msg->AddInt64("content-length",request->contentlen);
		msg->AddInt64("bytes-received",request->bytesreceived);
		printf("HTTP Layer - Done With Headers Broadcast:\n");
		msg->PrintToStream();
		
		Proto->Broadcast(target,msg);
		
		delete msg;
#ifdef DEBUG
		printf("Sending broadcast to handlers and parsers.\n");
#endif
		
	}
	
}

void httplayer::ProcessChunkedData(http_request *request,void *buffer, int size) {
	if (size==0)
		return;
	int32 oldchunk=0;
	
	if (size>request->chunkbytesremaining) {
		//size is greater than chunkbytesremaining
//		printf("chr: %ld\tchunk: %ld\n",request->chunkbytesremaining,request->chunk);
		if ((request->chunkbytesremaining==0) && (request->chunk==0)){
			char *eol=(char*)memchr(((char*)buffer),'\r',20);
			if (eol==NULL) {
				eol=(char*)memchr(((char*)buffer),'\n',20);
				if (eol==NULL)
					eol=((char*)buffer)+5;//guess where the end of the line may be...
			}
			char temp[20];
			memset(temp,0,20);
			int len=eol-(char*)buffer;
			
			strncpy(temp,(char*)buffer,len);
			request->chunkbytesremaining=strtol(temp,&eol,16);
			request->chunk++;
//			printf("First chunk size is %ld (0x%x) bytes\n",request->chunkbytesremaining,request->chunkbytesremaining);
//			request->data->Write((unsigned char*)(buffer+len+2),request->chunkbytesremaining);
			ProcessChunkedData(request,(char*)buffer+len+2,size-(len+2));
			return;
		} 
			
		oldchunk=request->chunkbytesremaining;
		CacheSys->Write(CacheToken,request->cache_object_token,(unsigned char*)buffer,request->chunkbytesremaining);
		
//		request->data->Write((unsigned char*)buffer,request->chunkbytesremaining);
		request->bytesreceived+=request->chunkbytesremaining;
		
		char *kk=new char[request->chunkbytesremaining+1];
		memset(kk,0,request->chunkbytesremaining+1);
		strncpy(kk,(char*)buffer,request->chunkbytesremaining);
//		printf("======Chunk======\n%s\n++++++++++\n",kk);
		delete kk;
		request->chunkbytesremaining=0;
		if ((size-oldchunk)>=7) {
			char *eol=NULL;
			char *start=NULL;
			char *rem=(char*)buffer+oldchunk;
			int32 br=size-oldchunk;
				char *rr=new char[br+1];
				memset(rr,0,br+1);
				strncpy(rr,rem,br);
//			printf("========Remaining========\n%s\n++++++++++++++\n",rr);
			delete rr;
			eol=(char*)memchr(rem,'\r',10);//strstr(rem,"\r\n");
			if (eol==NULL) {
				eol=(char*)memchr(rem,'\n',10);
				if (eol==NULL)
					eol=rem+10;//guessing...
			}
			start=eol+2;
			eol=(char*)memchr(start,'\r',10);
			if (eol==NULL) {
				eol=(char*)memchr(rem,'\n',10);
				if (eol==NULL)
					eol=rem+10;//guessing...
			}
			bool good=false;
			//eol=strstr(start,"\r\n");
			if ((eol!=NULL) && (*(eol+1)=='\n'))
				good=true;
			if ((!good) || ((eol-start)>99)) {
				//guess...
				eol=start+4;
			}
			
			char *semi=strchr(start,';');
			char bs[100];
			memset(bs,0,100);
			int len=0;
			if (semi!=NULL) {
				if (semi<eol) {
					len=semi-start;
				} else {
					len=eol-start;
				}
				
			} else {
				len=eol-start;
			}
			strncpy(bs,start,len);
			request->chunkbytesremaining=strtol(bs,&semi,16);
//			printf("New Chunk size: %ld\t%s\n",request->chunkbytesremaining,bs);
			if (request->chunkbytesremaining==0) {
//				printf("\tRequest is done!\n");
				CloseRequest(request);
				return;
			}
			
//			printf("Call ProcessChunkedData here.\n");
			request->chunk++;
			ProcessChunkedData(request,eol+2,size-((eol-start)+4+oldchunk));
		}
	} else {
		if (size<request->chunkbytesremaining) {
			//size is less than chunkbytesremaining
//			request->data->Write((unsigned char*)buffer,size);
				CacheSys->Write(CacheToken,request->cache_object_token,(unsigned char*)buffer,size);
			request->bytesreceived+=size;
			
			request->chunkbytesremaining-=size;
		} else {
			//size == chunkbytesremaining
//			printf("size==chunkbytesremaining\n");
			
//			request->data->Write((unsigned char*)buffer,size);
				CacheSys->Write(CacheToken,request->cache_object_token,(unsigned char*)buffer,size);
			request->bytesreceived+=size;
			request->chunkbytesremaining=0;
			request->chunk++;
		}
		
	}
//	printf("ProcessChunkedData done.\n");
}

void httplayer::ProcessData(http_request *request, void *buffer, int size) {
//	BAutolock alock(lock);
//	if (alock.IsLocked()) {
	int64 current_size=request->bytesreceived;
		
	if (request->chunked) {
		ProcessChunkedData(request,buffer,size);
	} else {
				CacheSys->Write(CacheToken,request->cache_object_token,(unsigned char*)buffer,size);
		request->bytesremaining-=size;
		request->bytesreceived+=size;
		if (request->bytesremaining<=0) {
			if (request->receivetilclosed) {
				printf("*** Receive Til Closed ***\n");
				
#ifndef NEWNET
				if (TCP->Connected(request->conn,true)) {
					request->bytesremaining=0;
					return;
				}
#else
				if (request->conn->IsConnected()) {
					request->bytesremaining=0;
					return;
				}
				
#endif	
//				CloseRequest(request);
			}
			if (request->bytesreceived>request->contentlen)
				CacheSys->SetLength(CacheToken,request->cache_object_token,request->contentlen);
			
			CloseRequest(request);
		}
		
	}
#ifdef DEBUG
	printf("ProcessData done. Bytes Received: %ld\n",request->bytesreceived);
#endif
	BMessage *msg=new BMessage(ReturnedData);
	// added by emwe
#ifdef DEBUG
	printf( "returned UID: %ld\n", request->view_id );
#endif
	msg->AddInt32( "view_id", request->view_id );
	msg->AddBool( "secure", request->secure );
	//
	msg->AddInt32("command",COMMAND_INFO);
	if (request->cache_object_token>=0)
		msg->AddInt32("cache_object_token",request->cache_object_token);
//	msg->AddPointer("data_pointer",request->data);
	if (request->url!=NULL)
		msg->AddString("url",request->url);
	if (request->done)
		msg->AddBool("request_done",true);
	else
		msg->AddBool("request_done",false);
	
	char *result=NULL;
	result=FindHeader(request,"content-type");
	int32 target=0;
	if (result!=NULL) {
		msg->AddString("mimetype",result);
		char *slash=strchr(result,'/');
		if (strncasecmp("text",result,slash-result)==0) {
			if (strncasecmp("html",slash+1,strlen(result)-((slash+1)-result))==0) {
				target=MS_TARGET_HTML_PARSER;
			} else
				target=MS_TARGET_HANDLER|MS_TARGET_CONTENT_TEXT|MS_TARGET_PARSER;
		}
		if (strncasecmp("image",result,slash-result)==0) {
			target=MS_TARGET_IMAGE_HANDLER;
		}
		if (strncasecmp("audio",result,slash-result)==0) {
			target=AUDIO_HANDLER;
		}
		if (strncasecmp("video",result,slash-result)==0) {
			target=VIDEO_HANDLER;
		}
		if (strncasecmp("application",result,slash-result)==0) {
			target=MS_TARGET_PARSER|MS_TARGET_HANDLER;
		}
		slash=NULL;
	} else 
		target=MS_TARGET_PARSER|MS_TARGET_HANDLER;
	
	
	result=NULL;
	if (request->contentlen!=0)
		msg->AddInt64("content-length",request->contentlen);
	msg->AddInt64("bytes-received",request->bytesreceived);
	msg->AddInt64("size-delta",request->bytesreceived-current_size);
		
#ifdef DEBUG
	printf("Sending broadcast to handlers and parsers. target: %ld\n",target);
#endif
	Proto->Broadcast(target,msg);
	delete msg;
	
//	}
	
}
void httplayer::CloseRequest(http_request *request,bool quick) {
//	BAutolock alock(lock);
//	if (alock.IsLocked()) {
#ifdef DEBUG
	printf("CloseRequest\n");
#endif
	if (request==NULL)
		return;
	int result=0;
	if (request->conn!=NULL)
#ifndef NEWNET
		result=request->conn->result;
#else
		result=request->conn->ConnectionResult();
#endif
#ifdef DEBUG
	printf("Calling Done()\n");
#endif
	Done(request);
#ifdef DEBUG
	printf("Back in CloseRequest.\n");
#endif
	if (request->conn!=NULL) {
#ifdef DEBUG
		printf("Locating header...\n");
#endif
		char *result=FindHeader(request,"Connection");
#ifdef DEBUG
		printf("Found header\n");
#endif
		if ((result!=NULL) && (strcasecmp("close",result)==0)) {
#ifdef DEBUG
			printf("Calling RequestDone #1\n");
#endif
#ifndef NEWNET
			TCP->RequestDone(request->conn,true);
#else
			TCPMan->Disconnect(request->conn);
#endif
		}else{
#ifdef DEBUG
			printf("Calling RequestDone #2\n");
#endif
#ifndef NEWNET
			TCP->RequestDone(request->conn);
#else
			TCPMan->DoneWithSession(request->conn);
#endif
		}
#ifdef DEBUG
		printf("Done with RequestDone\n");
#endif
		result=NULL;
		atomic_add(&request->conn_released,1);
		request->conn=NULL;
	}
	if (request->conn_released!=0)
		request->conn_released=0;
	if (quick) {
		return;
	}
	
	BMessage *msg=new BMessage(ProtocolConnectionClosed);
	msg->AddInt32("command",COMMAND_INFO);
	
	if (request->bytesreceived>0){
		
#ifdef DEBUG
		printf("CloseRequest bytes received: %Ld\n",request->bytesreceived);
#endif
	
		msg->AddInt64("bytes-received",request->bytesreceived);
	}
	
	else {
		//debugger("Stop here.");
			off_t csize=0;
			csize=CacheSys->GetObjectSize(CacheToken,request->cache_object_token);
			if (request->bytesreceived!=csize)
				request->bytesreceived=csize;
			msg->AddInt64("bytes-received",request->bytesreceived);
	}
	if (request->url!=NULL)
		msg->AddString("url",request->url);
	msg->AddInt32("From",Proto->PlugID());
	msg->AddPointer("FromPointer",Proto);
	if (request->cache_object_token>=0)
		msg->AddInt32("cache_object_token",request->cache_object_token);
//	msg->AddPointer("data_pointer",request->data);
	char *resultstr=NULL;
	resultstr=FindHeader(request,"content-type");
	int32 target=0;
	if (resultstr!=NULL)
		msg->AddString("mimetype",resultstr);
	
/*	//this section is now commented out because it depended on the retrieved data being
	//readily available in a BPositionIO container. As the newest cache system encapsulates
	//the data and doesn't make it directly accessible like that, this functionality is
	//currently broken... A solution might be to copy the data back into a temporary
	//BPositionIO class like BMallocIO, but I think that's something of a hack. The
	//above two lines should suffice for now, though we're taking the server's word for
	//the content type.
	if (resultstr!=NULL) {
		translator_info *outinfo=NULL;
		BMessage extinfo;
		int32 count=0;
		TRoster->GetTranslators(request->data,&extinfo,&outinfo,&count);
#ifdef DEBUG
		printf("supporting translators: %ld\n",count);
#endif
		if (count>0) {
#ifdef DEBUG
			printf("suspected mimetype: %s\n",outinfo[0].MIME);
#endif
			bool found=false;
			for (int32 i=0; i<count; i++) {
				if (strcasecmp(outinfo[i].MIME,resultstr)==0)
					break;
			}
			if (!found) {
				//take the best guess based on BeOS being right...
				msg->AddString("mimetype",outinfo[0].MIME);
			} else 
				msg->AddString("mimetype",resultstr);
			delete []outinfo;
		} else 
			msg->AddString("mimetype",resultstr);
	} else {
		translator_info *outinfo=NULL;
		BMessage extinfo;
		int32 count=0;
		TRoster->GetTranslators(request->data,&extinfo,&outinfo,&count);
#ifdef DEBUG
		printf("supporting translators: %ld\n",count);
#endif
		if (count>0) {
#ifdef DEBUG
			printf("suspected mimetype: %s\n",outinfo[0].MIME);
#endif
			msg->AddString("mimetype",outinfo[0].MIME);
			//really should send an update to the cache here...
			delete []outinfo;
		} else
			msg->AddString("mimetype","application/octet-stream");
		//if we don't know the file type for some reason, assume it's app data rather than text.
	}
*/	
	resultstr=NULL;
	if (request->chunked)
		msg->AddBool("chunked",true);
	
		msg->AddBool("request_done",true);
			if (request->chunked) {
				if (request->chunkbytesremaining!=0)
					msg->AddInt32("status",UnexpectedDisconnect);
			} else {
				if ((request->contentlen!=0) && (request->contentlen>request->bytesreceived))
					msg->AddInt32("status",UnexpectedDisconnect);
			}
	
	/* added by emwe. I need this damn view id :)) */
	msg->AddInt32( "view_id", request->view_id );
	
	if (request->contentlen>0)
		msg->AddInt64("content-length",request->contentlen);
	BMessage container;
	container.AddMessage("message",msg);
#ifdef DEBUG
	printf("http_layer::CloseRequest()\n");
#endif
	container.PrintToStream();
	msg->PrintToStream();
	Proto->Broadcast(MS_TARGET_ALL,msg);
	delete msg;
//	release_sem(connhandle_sem);
//	Unlock();
//	}
}
void httplayer::SendRequest(http_request *request, char *requeststr){
//	BAutolock alock(lock);
/*
			try {
				time_t start=time(NULL);
				while (request->conn->result==-2) {
					if ((time(NULL)-start)>=60)
						break;
					if (quit)
						break;
					snooze(5000);
				}
			}
			catch(...) {
#ifdef DEBUG
				printf("[Alpha]The connection failed.\n");
#endif
				return;
			}
*/
			if (request->conn!=NULL) {
			//	if (request->conn->result>=0) {
					int32 bytes=0;
//					TCP->Lock();
#ifndef NEWNET
					bytes=TCP->Send(&request->conn,(unsigned char *)requeststr,strlen(requeststr));
#else
					bytes=request->conn->Send(requeststr,strlen(requeststr));
#endif
//					TCP->Unlock();
#ifdef DEBUG
					printf("%ld bytes sent on connection: %p\n",bytes,request->conn);
#endif
				}/* else {
					BString title;
					BString mesg;
					title<<"Connection failed: - "<<request->host;
					mesg<<"Your attempt to connect to the server at "<<request->host<<" has failed.";
					(new BAlert( title.String(),mesg.String(),"D'oh"))->Go();
					
#ifdef DEBUG
					printf("[Bravo]The connection failed.\n");
#endif
				}*/
				
//			TCP->Unlock();
	
}
		
char *httplayer::BuildRequest(http_request *request){
//	BAutolock alock(lock);
	char *requeststr=NULL;
	http_request *current2;
	BString reqstr;
//			request=(http_request *)UnprocessedReqs->RemoveItem((int32)0);
			current2=NULL;
			
			current2=requests_head;
			if (current2==NULL) {
				requests_head=current2=request;
			}
			else {
				while (current2->next!=NULL)
					current2=current2->next;
				current2->next=request;
			}
#ifdef DEBUG
			printf("current2 is %p\n",current2);
#endif
	
			char *temp=NULL;
			reqstr << "GET " << request->uri <<" HTTP/1.1\r\nHost: " << request->host;
			if (request->port!=80)
				reqstr<<":"<<(int)request->port;
			
			reqstr << "\r\nUser-agent: "<<UserAgent()<<"\r\n";
			temp=GetSupportedTypes();
			reqstr << temp;
			delete temp;
			temp=NULL;
			//check for cookies...
			temp=CookieMonster->GetCookie(request->host,request->uri,request->secure);
			if (temp!=NULL) {
				reqstr<<temp;
				delete temp;
				temp=NULL;
			}
			//check authentication zones vs domain being connected to.
			reqstr << "Connection: keep-alive\r\n";
			auth_realm *realm=FindAuthRealm(request);
#ifdef DEBUG
			printf("Authentication realm: %p",realm);
#endif
			fflush(stdout);
			if (realm!=NULL) {
#ifdef DEBUG
				printf("\trealm: \"%s\"\tdata: %s",realm->realm,realm->auth);
#endif
				fflush(stdout);
			}
			printf("\n");
	
			if (request->a_realm!=NULL)
				realm=request->a_realm;
	
			if (realm!=NULL) {
				reqstr<<"Authorization: Basic "<<realm->auth<<"\r\n";
			}
#ifdef DEBUG
			printf("Checking cache status.\n");
#endif
	
			/*BMessage *cachestatus=*/CheckCacheStatus(request);
			if (request->cacheinfo!=NULL) {
			//	request->cacheinfo=cachestatus;
			//	if (request->cacheinfo!=NULL) {
					if (request->cacheinfo->HasString("etag"))  {
						BString str;
						request->cacheinfo->FindString("etag",&str);
						reqstr<<"ETag: "<<str<<"\r\n";
					}
					if (request->cacheinfo->HasString("last-modified")) {
						BString str;
						request->cacheinfo->FindString("last-modified",&str);
						reqstr<<"If-Modified-Since: "<<str<<"\r\n";
					}
			//	}
			}
#ifdef DEBUG
			printf("(cache status) done.\n");
#endif
	
			reqstr << "\r\n";
#ifdef DEBUG
			printf("Request string:\n---------\n%s\n---------\n",reqstr.String());
#endif
	requeststr=new char[reqstr.Length()+1];
	memset(requeststr,0,reqstr.Length()+1);
	strcpy(requeststr,reqstr.String());
	return requeststr;
}
		

bool httplayer::ResubmitRequest(http_request *request) {
//	printf( "httplayer::ResubmitRequest()\n" );
	
	//BAutolock alock(lock);
	if (request->done==0)
		CloseRequest(request,true);
	BMessage *info=new BMessage;
	info->AddInt32("command",COMMAND_RETRIEVE);
	
	/* added by enwe */
	info->AddInt32( "view_id", request->view_id );
	
	info->AddString("target_url",request->url);
	if (request->referrer!=NULL) {
		info->AddString("referrer",request->referrer);
	}
	if (request->a_realm!=NULL) {
		info->AddPointer("authorization",request->a_realm);
	}
	request->awin=NULL;
	KillRequest(request);
	Proto->Broadcast(MS_TARGET_SELF,info);
	delete info;
	
}
void httplayer::FindURI(char **url,char **host,uint16 *port,char **uri,bool *secure) {
//	BAutolock alock(lock);
#ifdef DEBUG
	printf("finding uri...\n");
#endif
	int32 nuuril=7;
	char *nuuri=NULL;
	BString master(*url),servant;
	int32 href=0,urltype=0,slash=0,colon=0,quote=0,at=0;
	bool auth=false;
	BString user,pass;
	
	href=master.IFindFirst("href=\"");
	if (href!=B_ERROR) {
		master.CopyInto(servant,href+6,master.Length()-6);
		master=servant;
		quote=master.IFindFirst("\"");
		master.CopyInto(servant,0,quote);
		master=servant;
	}
	
	urltype=master.IFindFirst("://");
	if (urltype!=B_ERROR) {
		master.CopyInto(servant,urltype+3,master.Length()-urltype+3);
		BString urlkind;
		master.CopyInto(urlkind,0,urltype);
#ifdef DEBUG
		printf("urlkind: %s\n",urlkind.String());
#endif
		if (urlkind.ICompare("https")==0)
			*secure=true;
		master=servant;
	}
	
	slash=master.IFindFirst("/");
	at=master.IFindFirst("@");
	if ((at!=B_ERROR) && (at<slash)) {
		colon=master.IFindFirst(":");
		if (colon<at) {
			auth=true;
			master.CopyInto(user,0,colon);
			master.CopyInto(pass,colon+1,at-colon-1);
#ifdef DEBUG
			printf("username: %s\npassword: %s\n",user.String(),pass.String());
#endif
			master.Remove(0,at+1);
			slash=master.IFindFirst("/");
		}
		colon=0;
	}
	
	if (slash!=B_ERROR) {
		master.CopyInto(servant,slash,master.Length()-slash);
		*uri=new char[servant.Length()+1];
		strcpy(*uri,servant.String());
		master.CopyInto(servant,0,slash);
		master=servant;
	}
	else{
		*uri=new char[2];
		strcpy(*uri,"/");
	}
	
	colon=master.IFindFirst(":");
	if (colon!=B_ERROR)	{
		master.CopyInto(servant,colon+1,master.Length()-(colon+1));
		*port=atoi(servant.String());
		master.CopyInto(servant,0,colon);
		master=servant;
	} else 	{
		if (*secure)
			*port=443;
		else   
			*port=80;
	}
	*host=new char[master.Length()+1];
	strcpy(*host,master.String());
	nuuril+=strlen(*host)+strlen(*uri)+((*secure)?1:0);
	int32 width=0;
	float pcopy=(*port)*1.0;
	while (pcopy>1.0) {
		pcopy/=10.0;
		width++;
	}
#ifdef DEBUG
	printf("%d is %ld characters wide.\n",*port,width);
#endif
	nuuril+=width+1;
	nuuri=new char[nuuril+1];
	memset(nuuri,0,nuuril+1);
	if (*secure) {
		if (*port!=443)
			sprintf(nuuri,"https://%s:%lu%s",*host,*port,*uri);
		else
			sprintf(nuuri,"https://%s%s",*host,*uri);
	} else {
		if (*port!=80)
			sprintf(nuuri,"http://%s:%lu%s",*host,*port,*uri);
		else
			sprintf(nuuri,"http://%s%s",*host,*uri);
	}
#ifdef DEBUG
	printf("new composite url: %s\n",nuuri);
#endif
	if (strcasecmp(*uri,nuuri)!=0) {
		delete (*url);
		*url=nuuri;
	} else
		delete nuuri;
}
void httplayer::Done(http_request *request) {
	//BAutolock alock(lock);
//	if (alock.IsLocked()) {
#ifdef DEBUG
		printf("Marking request as done.\n\tchunked:%s\n\tbytes remaining: %ld\n",request->chunked?"yes":"no",request->chunked?request->chunkbytesremaining:request->bytesremaining);
#endif
		CacheSys->ReleaseWriteLock(CacheToken,request->cache_object_token);	
		atomic_add(&request->done,1);
#ifdef DEBUG
		printf("Done with Done()\n");
#endif
//	}
}
void httplayer::httpalarm(int signum) 
{
	release_sem(httplayer_sem);
}

int32 httplayer::LayerManager() {
	http_request *request=NULL,*current=NULL, *current2=NULL;
	status_t stat;
	int32 size=102400;
	
	unsigned char *buffer=new unsigned char[size];
	int32 bytes=0;
	volatile int32 timeouts=0;
	
	volatile int32 c=0;
	signal(SIGALRM,&httpalarm);
	set_alarm(15000,B_PERIODIC_ALARM);
	while (!quit) {
	//new way
		if (acquire_sem(httplayer_sem)==B_OK) {
			if (lock->LockWithTimeout(7500)==B_OK) {
				timeouts=0;
				current=requests_head;
				while (current!=NULL) {
					if (quit)
						break;
					if (!current->done) {
#ifndef NEWNET
						if (current->datawaiting) {
#else
						if ((current->datawaiting) && (current->conn->DataSize()>0L)) {
#endif
							memset(buffer,0,size);
							bytes=0;
	#ifndef NEWNET
							bytes=TCP->Receive(&current->conn,buffer,10240);
	#else
							bytes=current->conn->Receive(buffer,10240);
							if (current->conn->DataSize()==0L)
								current->datawaiting=0;
	#endif
							if (bytes>0) {
								if (!current->headersdone) {
									ProcessHeaders(current,buffer,bytes);
								} else {
									ProcessData(current,buffer,bytes);
								}
							}
							
						}
						
					}
					
					current=current->next;
				}
				
				lock->Unlock();
			} else {
				atomic_add(&timeouts,1);
				if (timeouts>=1300) {//if we get roughly 10 seconds worth of time outs,
									 //we must have hit a deadlock... so unlock this object.
					printf("Possible deadlock detected in http... correcting.\n");
					
					lock->Unlock(); 
					timeouts=0;
				}
				
			}
			
			
		}
		
/* //old way
		if (current==NULL) {
			snooze(40000);
			current=requests_head;
			continue;
		}
		lock->Lock();
		if (current->done==0) {
				lock->Unlock();
			if (current->datawaiting>=1) {
					memset(buffer,0,10240);
					bytes=0;
#ifndef NEWNET
					bytes=TCP->Receive(&current->conn,buffer,10240);
#else
					lock->Lock();
					bytes=current->conn->Receive(buffer,10240);
					lock->Unlock();
#endif
					if (current->datawaiting)
						current->datawaiting=0;
					if (bytes>0) {
							if (current->headersdone!=1) {
								ProcessHeaders(current,buffer,bytes);
							} else {
#ifdef DEBUG
								printf("http layermanager: about to process data for url %s\n",current->url);
#endif
								ProcessData(current,buffer,bytes);
#ifdef DEBUG
								printf("http layermanager: post processdata, %s\n",current->url);
#endif
								
							}
					}
			}
		}else
			lock->Unlock();
		if (current!=NULL)
			current=current->next;
*/		
	}
	set_alarm(B_INFINITE_TIMEOUT,B_PERIODIC_ALARM);
	
	memset(buffer,0,10240);
	delete buffer;
	buffer=NULL;
	return 0;
}


BMessage *httplayer::CheckCacheStatus(http_request *request) {
//	BAutolock alock(lock);
#ifdef DEBUG
	printf("httplayer::CheckCacheStatus\n");
#endif
	if (request!=NULL) {
		
		BMessage *msg=new BMessage(FindCachedObject);
		msg->AddString("url",request->url);
		BMessage *reply=NULL;
		acquire_sem(cache_sem);
		BMessage *container=new BMessage;
		msg->AddInt32("ReplyTo",Proto->PlugID());
		msg->AddPointer("ReplyToPointer",Proto);
		msg->AddInt32("command",COMMAND_INFO_REQUEST);
		msg->AddBool("cache_item",true);
		if (CacheSys!=NULL)
			msg->AddInt32("cache_user_token",CacheToken);
		container->AddMessage("message",msg);
		
		bool nocache=false;
#ifdef DEBUG
		printf("Trying to broadcast\n");
#endif
		Proto->Broadcast(MS_TARGET_CACHE_SYSTEM,msg);
#ifdef DEBUG
		printf("one\n");
#endif
		if (Proto->broadcast_status_code!=B_OK)
			nocache=true;
		delete msg;
		delete container;
/*
	Yes, it looks weird that we're acquiring the cache_sem again, but this makes sure
	that we get a response back from the cache before we continue. We only want to release
	the sem in this function before reacquiring it if there are no cache plug-ins.
*/
		if (nocache)
			release_sem(cache_sem);
#ifdef DEBUG
		printf("HTTP MIGHT HANG HERE\n");
#endif
		acquire_sem(cache_sem);
#ifdef DEBUG
		printf("DONE AT POSSIBLE CHOKE POINT\n");
#endif
		reply=Proto->cache_reply;
		Proto->cache_reply=NULL;
		if (reply==NULL) {
			reply=new BMessage(CacheObjectNotFound);
		}
		
		release_sem(cache_sem);
		
		if (reply->what==CacheObjectNotFound){
			delete reply;
			return NULL;
		} else {
#ifdef DEBUG
			printf("cacheinfo: %p\n");
#endif
			if (request->cacheinfo!=NULL)
				delete request->cacheinfo;
			request->cache_object_token=reply->FindInt32("cache_object_token");
			if (request->cache_object_token>=0) {
				
				request->cacheinfo=CacheSys->GetInfo(CacheToken,request->cache_object_token);
#ifdef DEBUG
			printf("56789 cache info\n");
#endif
			request->cacheinfo->PrintToStream();
			}
			
		}
		delete reply;
		return request->cacheinfo;
	} else
		return NULL;
}
void httplayer::Start() {
	if (acquire_sem_etc(http_mgr_sem,1,B_ABSOLUTE_TIMEOUT,100000)==B_OK) {
		thread=spawn_thread(StartLayer,"http_layer_manager",B_LOW_PRIORITY,this);
		resume_thread(thread);
#ifdef DEBUG
		printf("http layer manager thread: %ld\n",thread);
#endif
//		helperthread=spawn_thread(StartHelper,"http_helper_thread",B_LOW_PRIORITY,this);
//		resume_thread(helperthread);
	}
	else
	{
#ifdef DEBUG
		fprintf(stderr,"http_layer_manager already running.\n");
#endif
		
	}
	
}

int32 httplayer::StartLayer(void *arg) {
#ifdef DEBUG
	printf("http layer starting\n");
#endif
	httplayer *obj=(httplayer*)arg;
	return obj->LayerManager();
}
/*
int32 httplayer::StartHelper(void *arg) {
	httplayer *obj=(httplayer*)arg;
	return obj->Helper(obj);
}
int32 httplayer::Helper(void *arg) {
	httplayer *http=(httplayer*)arg;
	while(http->quit!=1) {
	}
	return 0;
}
*/
status_t httplayer::Quit() {
#ifdef DEBUG
	printf("http layer stopping\n");
#endif
	
	atomic_add(&quit,1);
#ifdef DEBUG
	printf("\"Quit\" is set\n");
#endif
	lock->Unlock();
	
	status_t stat=B_OK;
#ifdef DEBUG
	printf("releasing semaphore\n");
#endif
	release_sem(http_mgr_sem);
#ifdef DEBUG
	printf("Waiting for thread death\n");
#endif
	wait_for_thread(thread,&stat);
//	wait_for_thread(helperthread,&stat);
#ifdef DEBUG
	printf("quitting\n");
#endif
	return stat;
}
//*
char *httplayer::GetSupportedTypes() {
	//BAutolock alock(lock);
#ifdef DEBUG
	printf("httplayer::GetSupportedTypes()\n");
#endif
	BString output("Accept: ");
	//insert code here to get content types from other plug-ins via the plug-in manager
	smt_st *cur=Proto->smthead;
	while (cur!=NULL) {
		output << cur->type;
		if (cur->next!=NULL)
			output << ", ";
		cur=cur->next;
	}
	cur=NULL;
	output << "\r\n";
//	printf("output: %s\n",output.String());
	
	char *out=new char[output.Length()+1];
	memset(out,0,output.Length()+1);
	
	strcpy(out,output.String());
//	printf("Supported MIME TYPE String: %s\n",out);
	
	return out;
}
char *httplayer::UserAgent() {
//	BAutolock alock(lock);
	
	//	Return the identity that the user selects. This should allow Themis access to pages
	//	that have been written specifically for one or a few browsers; I hate those pages.
	//	Bastards.
	
	char *str=NULL;
	switch(use_useragent) {
		case 0:
			str="Themis (http://themis.sourceforge.net)";//Normal & Default
			break;
		case 1:
			str="Mozilla/4.0 (compatible; MSIE 6.0; BeOS ; Themis http://themis.sourceforge.net)";//Internet Explorer 6.0
			break;
		case 2:
			str="Mozilla/5.0 (BeOS; Themis http://themis.sourceforge.net)"; //Netscape/Mozilla
			break;
		default:
			str="Themis (http://themis.sourceforge.net)";//Normal & Default
	}
	return str;
}
//*/
#ifndef NEWNET
void httplayer::ConnectionEstablished(connection *conn)
{
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		http_request *req=requests_head;
		while (req!=NULL) {
			if ((req->conn==conn) && (req->done==0)) {
				printf("http layer: Connection Established!!!!!!\n");
				atomic_add(&req->connection_established,1);
				SendRequest(req,req->requeststr);
				break;
			}
			req=req->next;
			if (quit)
				break;
		}
				
	}
}
#endif

#ifdef NEWNET
void httplayer::ConnectionEstablished(Connection *connection)
{
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		http_request *req=requests_head;
		while (req!=NULL) {
			if ((req->conn==connection) && (req->done==0)) {
				printf("http layer: Connection Established!!!!!!\n");
				atomic_add(&req->connection_established,1);
				SendRequest(req,req->requeststr);
				break;
			}
			req=req->next;
			if (quit)
				break;
		}
				
	}
}
void httplayer::ConnectionAlreadyExists(Connection *connection)
{
	printf("ConnectionAlreadyExists\n");
}
void httplayer::ConnectionTerminated(Connection *connection)
{
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		http_request *req=requests_head;
		while (req!=NULL) {
			if (req->conn==connection) {
				
				printf("Connection to server has terminated. Cache object: %ld\n",req->cache_object_token);
/*
{
							memset(buffer,0,size);
							bytes=0;
	#ifndef NEWNET
							bytes=TCP->Receive(&current->conn,buffer,10240);
	#else
							bytes=current->conn->Receive(buffer,10240);
	#endif
							if (bytes>0) {
								if (!current->headersdone) {
									ProcessHeaders(current,buffer,bytes);
								} else {
									ProcessData(current,buffer,bytes);
								}
							}
							
						}
*/
				printf("Data Waiting on Terminated connection? %d\n",req->conn->IsDataWaiting());
				
				if (req->conn->IsDataWaiting()) {
					printf("Terminated: Data is waiting\n");
					
					off_t datasize=req->conn->DataSize();
					unsigned char *buffer=new unsigned char[datasize];
					memset(buffer,0,datasize);
					off_t recbytes=req->conn->Receive(buffer,datasize);
					if (recbytes>0) {
						printf("terminated: %ld bytes received\n",recbytes);
						printf("%s\n",(char*)buffer);
						
						if (!req->headersdone) {
							ProcessHeaders(req,buffer,recbytes);
						} else {
							ProcessData(req,buffer,recbytes);
						}
						
					}
					memset(buffer,0,datasize);
					delete buffer;
					buffer=NULL;
					
				}
				
				CloseRequest(req);
				break;
				
			}
			req=req->next;
		}
		
	}
	
}
void httplayer::DataIsWaiting(Connection *connection)
{
//	printf("DataIsWaiting\n");
//	BAutolock alock(lock);
//	if (alock.IsLocked()) {
//		printf("(http)DataIsWaiting about to search\n");
		http_request *req=FindRequest(connection);
		if (req!=NULL) {
			if (req->datawaiting==0)
				atomic_add(&req->datawaiting,1);
		}
//	}
}
http_request *httplayer::FindRequest(Connection *connection) {
	http_request *req=requests_head;
	while (req!=NULL) {
		if (req->conn==connection) {
			break;
		}
		req=req->next;
	}
	return req;
}
void httplayer::ConnectionError(Connection *connection)
{
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		printf("ConnectionError: %s\n",connection->ErrorString(connection->Error()));
		switch(connection->Error()) {
			case Connection::ERROR_CONNECTION_RESET: {//this means we should wrap things up...
				http_request *req=FindRequest(connection);
				if (req!=NULL)
					CloseRequest(req);
			}break;
			default:
				;
		}
		connection->ClearError();
	}
}
void httplayer::ConnectionFailed(Connection *connection)
{
	printf("ConnectionFailed\n");

}
void httplayer::DestroyingConnectionObject(Connection *connection)
{
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		http_request *current=requests_head;
		while (current!=NULL) {
			if (current->conn==connection) {
				printf("DestroyingConnectionObject, so I must clean up the request for it.\n");
//				CloseRequest(current);
				
				
//				delete current;
				break;
			}
			current=current->next;
		}
	}
} 
#endif


