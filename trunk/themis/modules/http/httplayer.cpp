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

#include "httplayer.h"
#include <string.h>
#include <stdio.h>
#include <Alert.h>
//#include <cmath.h>
#include "stripwhite.h"
#include "authvw.h"

httplayer *meHTTP;
tcplayer *__TCP;

int32 L;
int32 U;

void DataReceived(connection *conn) 
{
	printf("http: callback function\n");
	meHTTP->DReceived(conn);
	printf("http: callback done.\n");
}
int32 LockHTTP(int32 timeout)
{
	printf("LockHTTP %ld\n",L);
	L++;
	return meHTTP->Lock();
}
void UnlockHTTP(void) 
{
	printf("UnlockHTTP %ld\n",U);
	U++;
	meHTTP->Unlock();
}

/*
httplayer::httplayer(tcplayer *_TCP) {
	Basic=NULL;
	meHTTP=this;
	SetTCP(_TCP);
//	file=new BFile("/var/tmp/themis_output",B_ERASE_FILE|B_CREATE_FILE|B_WRITE_ONLY);
	UnprocessedReqs=new BList(1);
	ResubmitReqs=new BList(1);
	
	requests_head=NULL;
	quit=0;
	http_mgr_sem=create_sem(1,"http_mgr_sem");
	use_useragent=0;
	PluginManager=NULL;
	CachePlug=NULL;
}
*/
httplayer::httplayer(tcplayer *_TCP) 
{
	lock=new BLocker("http lock",false);
	lock->Lock();
	printf("HTTP object at %p\nhttp tcp: %p\n",this,_TCP);
	
	L=U=0;
	TCP=NULL;
	if (_TCP!=NULL)
		SetTCP(_TCP);
	__TCP==_TCP;
	meHTTP=this;
	Basic=NULL;
	requests_head=NULL;
	quit=0;
	http_mgr_sem=create_sem(1,"http_mgr_sem");
	connhandle_sem=create_sem(1,"connection_handler_sem");
	reqhandle_sem=create_sem(1,"request_handler_sem");
	httplayer_sem=create_sem(1,"httplayer_sem");
	use_useragent=0;
	PluginManager=NULL;
	CachePlug=NULL;
	lock->Unlock();
}
/*
httplayer::~httplayer() {
	printf("HTTP layer destructor\n");
	ResubmitReqs->MakeEmpty();
	delete ResubmitReqs;
//	delete file;
	http_request *req;
	printf("removing queued request items\n");
	while(UnprocessedReqs->CountItems()>0) {
		req=(http_request*)UnprocessedReqs->RemoveItem((int32)0);
		if (req!=NULL)
			delete req;
	}
	delete UnprocessedReqs;
	printf("removing other request items\n");
	while(requests_head!=NULL) {
//		printf("%s:%u\ndata:\n%s\n=========\n",requests_head->host,requests_head->port,(char*)requests_head->data->Buffer());
		req=requests_head->next;
		delete requests_head;
		requests_head=req;
	}
	delete_sem(http_mgr_sem);
	if (Basic!=NULL) {
		auth_realm *realms=Basic->realms_head,*tr;
		while (realms!=NULL) {
			tr=realms->next;
			delete realms;
			realms=tr;
		}
		delete Basic;
	}
}
*/
httplayer::~httplayer() 
{
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
//	release_sem(http_mgr_sem);
	delete_sem(http_mgr_sem);
	http_mgr_sem=0;
//	release_sem(connhandle_sem);
	delete_sem(connhandle_sem);
	connhandle_sem=0;
//	release_sem(reqhandle_sem);
	delete_sem(reqhandle_sem);
	delete_sem(httplayer_sem);
	delete lock;
}


void httplayer::SetTCP(tcplayer *_TCP) {
//	printf("Entered SetTCP\n");
	
	if (_TCP!=NULL) {
		
	TCP=_TCP;
//	printf("_TCP: 0x%lx\n", (uint32)_TCP);
//	printf("TCP: 0x%lx\n", (uint32)TCP);
	fflush(stdout);
	TCP->Lock();
	TCP->SetDRCallback((int32)'http',DataReceived,LockHTTP,UnlockHTTP);
	TCP->Unlock();
	}
//	printf("Leaving SetTCP\n");
}
/*
void httplayer::DReceived(connection *conn) {
	printf("Data received on connection %p\n",conn);
	http_request *req=requests_head;
	while(req!=NULL) {
		if ((req->conn==conn) && (req->done==0)) {
			printf("Connection %p belongs to request %p\n",conn,req);
			break;
		}
		req=req->next;
		if (quit)
			break;
	}
	
	if (req!=NULL) {
		printf("Data is for URI %s on %s\n",req->uri, req->host);
		
		if (req->datawaiting==0)
			atomic_add(&req->datawaiting,1);	
	}
}
*/
void httplayer::DReceived(connection *conn) 
{
//	acquire_sem(connhandle_sem);
	printf("http: DReceived start.\n");
	
	http_request *req=requests_head;
	while (req!=NULL) {
		if ((req->conn==conn) && (req->done==0)) 
			break;
		req=req->next;
		if (quit)
			break;
	}
	if (req!=NULL) {
		printf("Data is for URL %s\n",req->url);
		atomic_add(&req->datawaiting,1);
	}
//	release_sem(connhandle_sem);
	printf("http: DReceived is done.\n");
}


//the following three functions probably can be rewritten to be much more
//efficient and faster. my priority, however, was getting something to work.
int httplayer::find_lcd(int fact1,int fact2,int start) {
	int i=start;
	bool found=false;
	while (!found) {
		if ((i%fact1)==0) {
			if ((i%fact2)==0) {
				found=true;
				break;
			}	
		}
		i++;	
		if (quit)
			break;	
	}
	return i;
}
long httplayer::b64encode(void *unencoded,long in_size,char *encoded,long maxsize) {
	char BASE[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	unsigned char *data=(unsigned char*)unencoded;
	int base2[8]={1,2,4,8,16,32,64,128};
	long realsize=find_lcd(6,8,in_size);
	if (realsize>maxsize)
		return -1;
	uint8 *buff=new uint8[(realsize*8)];
	memset(buff,0,realsize);
	long pos=0;
	for (long i=0;i<in_size;i++) {
		for (int j=7;j>-1;j--) {
			buff[pos]= ((data[i] & base2[j])>0) ? 1:0;
			fflush(stdout);
			pos++;
			if (quit)
			break;
		}
		if (quit)
			break;	
	}
	long i=0;
	int chr=0;
	int final_offset=0;
	memset(encoded,'=',realsize);
	do
	{
		chr=0;
		for (int j=5; j>-1;j--){
			chr|=(buff[i]==1)?base2[j]:0;				
			i++;
			if (quit)
			break;
		}
		*(encoded+final_offset)=BASE[chr];
		final_offset++;
		//		if (final_offset>realsize)
		//			break;
		if (quit)
		break;
	} while(i<(in_size*8));
	printf("encoded auth: %s\n",(char*)encoded);
	delete []buff;
	return realsize;	
}
long httplayer::b64decode(char *encoded,unsigned char **decoded,long *size) {
	char BASE[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	long in_size=strlen(encoded);
	char *buff=new char[find_lcd(6,8,in_size)*8];
	*size=find_lcd(6,8,in_size);//by finding the lowest common denominator above the size of the data being decoded
				//we can get a good idea about the largest amount of data that will be generated. Likely,
				//the actual amount will be smaller.
	*decoded=new unsigned char[*size];
	memset((*decoded),0,*size);
	int pos=0; //position of character [in question] in the BASE64 string.
	long buff_pos=0;
	int base2[8]={1,2,4,8,16,32,64,128};
	memset(buff,0,in_size*8);
	for (int i=0;i<in_size;i++) {
		if (encoded[i]=='=')//ignore equal signs; they're just filler
			continue;		
		for (int j=0;j<64;j++) {
			if (encoded[i]==BASE[j]) {
				pos=j;
				break;
			}
			if (quit)
				break;
		}
		for (int j=5;j>-1;j--) {
			*(buff+buff_pos)=((pos & base2[j]) > 0) ? 1:0;
			buff_pos++;
			if (quit)
				break;
		}
		if (quit)
			break;
	}
	int final_offset=0;//off set from beginning of the final reconstituted data
	int bit_counter=7; //which bit
	int chr=0; //character being reconstituted
	for (long i=0;i<buff_pos;i++) {
	if (bit_counter==-1) {
	*(*decoded+final_offset)=(unsigned char)chr;
	chr=0;
	bit_counter=7;
	final_offset++;
	}
	chr|=(*(buff+i)==1)?base2[bit_counter]:0;
	bit_counter--;
	if (quit)
		break;
		
	}
	*size=final_offset;
	delete buff;
	return *size;
}
auth_realm *httplayer::FindAuthRealm(char *realm,char *host) {
	if (Basic!=NULL) {
		auth_realm *cur=Basic->realms_head;
		auth_realm *target=NULL;
		while (cur!=NULL) {
			if ((strcasecmp(cur->realm,realm)==0) && (strcasecmp(host,cur->host)==0)){
				target=cur;
				break;
			}
			cur=cur->next;
			if (quit)
				break;
		}
		return target;
	}
	return NULL;
}
auth_realm *httplayer::FindAuthRealm(http_request *request) {
	auth_realm *realm=NULL;
	if (request->a_realm!=NULL)
		return request->a_realm;
	if (Basic!=NULL) {
		auth_realm *cur=Basic->realms_head;
		while (cur!=NULL) {
			if (strcasecmp(request->host,cur->host)==0) {
				char *uriloc=NULL;
				uriloc=strstr(request->uri,cur->baseuri);
				if (uriloc!=NULL) {
					realm=cur;
					break;
				}
			}
			if (quit)
				break;
		}
		
	}
	return realm;
}

auth_realm *httplayer::AddAuthRealm(http_request *request,char *realm, char *user, char *password) {
	BString authinfo;
	authinfo<<user<<":"<<password;
	int size=(strlen(user)+strlen(password)+1)*2;//make sure that we have plenty of room.
	auth_realm *rlm=new auth_realm;
	rlm->auth=new char[size+1];
	size=b64encode((char*)authinfo.String(),authinfo.Length(),rlm->auth,size);
	authinfo="nothing";
	memset(rlm->auth+size,0,strlen(rlm->auth)-size);
	
	rlm->host=new char[strlen(request->host)+1];
	memset(rlm->host,0,strlen(request->host)+1);
	strcpy(rlm->host,request->host);
	char *bu=strrchr(request->uri,'/');
	if (bu!=NULL) {
		size=bu-request->uri;
		rlm->baseuri=new char[size+1];
		memset(rlm->baseuri,0,size+1);
		strncpy(rlm->baseuri,request->uri,size);
	}
	size=strlen(realm);
	rlm->realm=new char[size+1];
	memset(rlm->realm,0,size+1);
	strcpy(rlm->realm,realm);
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
//	CloseRequest(request);
	
	ResubmitRequest(request);
	return rlm;
}
void httplayer::UpdateAuthRealm(auth_realm *realm,char *user,char *pass) {
	delete realm->auth;
	BString authinfo;
	authinfo<<user<<":"<<pass;
	int size=(strlen(user)+strlen(pass)+1)*2;//make sure that we have plenty of room.
	realm->auth=new char[size+1];
	b64encode((char*)authinfo.String(),authinfo.Length(),realm->auth,size);
	authinfo="nothing";
}
void httplayer::ClearRequests() {
	acquire_sem(reqhandle_sem);
	http_request *curreq=requests_head,*next=NULL;
	requests_head=NULL;
	release_sem(reqhandle_sem);
	//The sem is released so that other processes may continue quicker.
	while(curreq!=NULL) {
		next=curreq->next;
		delete curreq;
		curreq=next;
	}
	
}
void httplayer::KillRequest(http_request *request) {
	acquire_sem(reqhandle_sem);
	http_request *curreq=requests_head;
	if (request==curreq) {
		requests_head=request->next;
		if (!request->done) {
/*
			TCP->Lock();
			TCP->RequestDone(request->conn);
			TCP->Unlock();
*/
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
					/*
					TCP->Lock();
				TCP->RequestDone(request->conn);
					TCP->Unlock();
					*/
					CloseRequest(request);
			}
			http_request *next=request->next;
			delete request;
			curreq->next=next;
		} else {
			if (!request->done) {
			/*
					TCP->Lock();
				TCP->RequestDone(request->conn);
					TCP->Unlock();
			*/
				CloseRequest(request);
			}
			
			delete request;
		}
	}
	release_sem(reqhandle_sem);
}

http_request *httplayer::AddRequest(BMessage *info) {
	int32 what=0;
	if (info!=NULL) {
		if (info->HasInt32("action")) {
			info->FindInt32("action",&what);
			switch(what) {
				case LoadingNewPage: {
					if (requests_head!=NULL) {
						printf("Cleaning up older requests...\n");
						ClearRequests();
						/*
						http_request *req=requests_head;
						while (requests_head!=NULL) {
							req=requests_head->next;
							CloseRequest(requests_head);
							delete requests_head;
							requests_head=req;
						}
						requests_head=NULL;
						*/
					}
				}break;
				case ReloadData: {
					if (CachePlug==NULL) {
						CachePlug=(PlugClass*)PluginManager->FindPlugin(CachePlugin);
						if (CachePlug==NULL) {
							//hmm... we must not have the cache plug-in... no biggie... act normal
						} else {
							//clear the cache of this URL, and any related.
						}
					}
				}break;
				default: {
					printf("\t\t*** Unknown protocol action requested! %ld (%c%c%c%c)\n",what,what>>24,what>>16,what>>8,what);
				}
			}
		}
		if (PluginManager==NULL) {
			info->FindPointer("plug_manager",(void**)&PluginManager);
		}
		if (CachePlug==NULL) {
			CachePlug=(PlugClass*)PluginManager->FindPlugin(CachePlugin);
		}
		http_request *request=new http_request;
		if (info->HasInt32("browser_string"))
			info->FindInt32("browser_string",&use_useragent);
		if (info->HasString("referrer")) {
			BString refr;
			info->FindString("referrer",&refr);
			request->referrer=new char[refr.Length()+1];
			memset(request->referrer,0,refr.Length()+1);
			strcpy(request->referrer,refr.String());
		}
		
		BString url;
		info->FindString("target_url",&url);
		request->url=new char[url.Length()+1];
		strcpy(request->url,url.String());
		FindURI(&request->url,&request->host,&request->port,&request->uri,&request->secure);
		if (what==LoadingNewPage) {
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
		printf("OpenSSL not supported in this build.");
		(new BAlert("SSL Not Supported","I'm afraid that this version of Themis\nwas built without OpenSSL support.\nUnfortunately, it means that you can't\nvisit HTTPS sites.","Damn."))->Go();
		delete info;
		delete request;
		request=NULL;
		return request;
#else
// We have and want to be able to use OpenSSL in Themis.
		printf("Request is to: %s:%u\nURI: %s\n",request->host,request->port,request->uri);
		delete info;
		printf("End AddRequest\n");
#endif
		} else {
			printf("Request is to: %s:%u\nURI: %s\n",request->host,request->port,request->uri);
			delete info;
			printf("End AddRequest\n");
		}
		char *requeststr=BuildRequest(request);
		SendRequest(request,requeststr);
		delete requeststr;
		return request;
	}else
	return NULL;
}
char *httplayer::FindEndOfHeader(char *buffer, char **eohc) {
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
	printf("attribute: %s\t%d bytes\nvalue: %s\t%d bytes\n",attribute,strlen(attribute),value,strlen(value));
	
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
		cur->value=new char[len+1];
		memset(cur->value,0,len+1);
		strcpy(cur->name,attribute);
		strcpy(cur->value,value);
		request->headers=cur;
	}
	return cur;
}

char *httplayer::FindHeader(http_request *request,char *attribute) {
	header_st *cur=request->headers;
	if (cur!=NULL) {
		while (cur!=NULL) {
			if (strcasecmp(attribute,cur->name)==0) 
				break;
			cur=cur->next;
		}
		if (cur!=NULL)
			return cur->value;
		return NULL;
	}
	return NULL;
}
void httplayer::ClearHeaders(http_request *request) {
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
				int len=eoh-buf;
				header=new char[len+1];
				memset(header,0,len+1);
				strncpy(header,buf,len);
			}
	}
	
//	printf("ProcessHeaders getting started:\n%s\n",buf);
	
	char *tmp=new char[5];
	memset(tmp,0,5);
	strncpy(tmp,buf,4);
	for (int i=0;i<4;i++)
		tmp[i]=toupper(tmp[i]);
	if ((strcmp(tmp,"HTTP")==0) || (request->storage!=NULL)) {
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
						printf("Being redirected to %s\n",request->url);
						ResubmitRequest(request);
						delete loc;
						loc=NULL;
						delete eohc;
						return;
					} else {
						delete loc;
						loc=NULL;
						if (destination[0]=='/') {
							//relative location
							BString url("http");
							if (request->secure)
								url << "s";
							url << "://"<<request->host<<":"<<request->port<<destination;
							memset(request->url,0,strlen(request->url)+1);
							delete request->url;
							request->url=new char[url.Length()+1];
							memset(request->url,0,url.Length()+1);
							strcpy(request->url,url.String());
							FindURI(&request->url,&request->host,&request->port,&request->uri,&request->secure);
							ClearHeaders(request);
							printf("Being redirected to %s\n",request->url);
							if (request->cacheinfo!=NULL) {
								delete request->cacheinfo;
								request->cacheinfo=NULL;
							}
							if (request->cache!=UsesCache)
								request->cache=UsesCache;
							if (request->data!=NULL) {
								delete request->data;
								request->data=NULL;
							}
							ResubmitRequest(request);
							delete eohc;
							return;
						}
					}
				}
			}break;
			case 4: {
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
		printf("Diner says hi!\n");
		exit(0);
		
		delete tmp;
	}
//	printf("ProcessHeaders done.\n");
}

/*
void httplayer::ProcessHeaders(http_request *request,void *buffer,int size) {
	printf("%s:%u\n",request->host,request->port);
	bool conthead=false;
	bool triggerauth=false;
	bool http_continue=false;
	unsigned char *eoh=NULL;
	unsigned char *eorl=NULL;//end of response line
	int eoh_len=4;
	header_st *tmp=new header_st;
	if (request->headers==NULL) {
		
	request->headers=tmp;
	
	unsigned char *buf=(unsigned char*)buffer;
	ProcessHeadersTop:
	if (http_continue)
		buf=(eoh+eoh_len);
	eoh=(unsigned char*)strstr((char*)buf,"\r\n\r\n");
	if (eoh==NULL) {
		eoh=(unsigned char*)strstr((char*)buf,"\n\n");
		if (eoh==NULL) {
			eoh=(unsigned char*)strstr((char*)buf,"\r\r");
			if (eoh==NULL) {
				for (int16 i=0; i<size; i++) {
		//         fprintf(stdout,"buf[%d]: 0x%x - %c\n",i,*(buf+i),*(buf+i));
					if ((*(buf+i))=='\0') {
						printf("NULL found... assuming that's the end of the header!\n");
						eoh=(unsigned char*)buf+(i-2);
						eoh_len=eoh-buf;
						break;
					}
				}//for loop
			}//eoh is still NULL
			else
				eoh_len=2;
		}
		else
			eoh_len=2;
	}
	if (eoh_len<=0) {
		delete request->headers;
		request->headers=NULL;
		return;
	}
	
	eorl=(unsigned char*)strstr((char*)buf,"\r\n");
	{
		char *data=new char[eorl-buf+1];
		memset(data,0,(eorl-buf+1));
		strncpy(data,(char*)buf,(eorl-buf));
		printf("response line: %s\n",data);
		int oldstatus=request->status;
		//we're looking for a HTTP reponse of 100...
		sscanf(data,"HTTP/%d.%d %d",&request->http_v_major,&request->http_v_minor,&request->status);
		printf("http/%ld.%ld, status %d\n",request->http_v_major,request->http_v_minor,request->status);
		if ((request->status==401) && (oldstatus==401)) {
			if (request->a_realm!=NULL) {
				(new authwin("Password required!",request,request->a_realm->realm));
				ResubmitRequest(request);
				
			}
			
		}
		
		if (strlen(data)==(unsigned)size) {
			printf("data received is http response only\n");
			delete request->headers;
			request->headers=NULL;
			delete data;
			return;
		} else {
			if (request->status==100) {//Response 100 is "Continue". It is usually immediately followed by another response line, headers, and data.
				memset(buf,32,eorl-buf);
				
				http_continue=true;
				delete data;
				goto ProcessHeadersTop;
			}
		}
		delete data;
	}
//	printf("buf: %p\neoh: %p\ndiff: %ld\neoh len: %d\n",buf,eoh,eoh-buf,eoh_len);
//	printf("=========\nbuf:\n%s\n=========\neoh:\n%s\n=========\n",buf,(eoh+eoh_len));
	char *eol=NULL;
	char *colon=NULL;
	char *temp=NULL;
	
	int pos=eorl-buf+2;
	header_st *curhead=request->headers;
				memset(buf,32,pos);//eorl-buf);//wipe out the response line and it's \r\n
	while (eol<(char*)eoh) {
		if (quit)
			break;
		if (curhead->name!=NULL) {
			curhead->next=new header_st;
			curhead=curhead->next;
		}
		
		colon=strchr((char*)(buf+pos),':');
		if (colon!=NULL) {
			curhead->name=new char[colon-((char*)(buf+pos))+1];
			memset(curhead->name,0,(colon-((char*)(buf+pos))+1));			
			strncpy(curhead->name,((char*)(buf+pos)),(colon-((char*)(buf+pos))));
			printf("header name [%ld]: %s\t",strlen(curhead->name),curhead->name);
			fflush(stdout);
			eol=strstr(colon,"\r\n");
			curhead->value=new char[eol-colon+1];
			temp=new char[eol-colon+1];
			memset(temp,0,(eol-colon)+1);
			strncpy(temp,(colon+1),(eol-colon)-1);
			memset(curhead->value,0,(eol-colon)+1);
			stripfrontwhite((const char*)temp,curhead->value);
			printf("value: %s\n",curhead->value);

		}
		pos=(eol+2)-(char*)buf;
		
	}
	printf("bytes remaining after header: %ld\n",size-((eoh-buf)+eoh_len));
	if ((size-((eoh-buf)+eoh_len))==0) {
//		conthead=true;
		return;
	}
	if ((size-((eoh-buf)+eoh_len))<0) {
		conthead=true;
		return;
	}
	if (conthead!=true)
		DoneWithHeaders(request);//request->headersdone=true;
	printf("eoh: %p\n",eoh);
	
	if (request->chunked) {
		sscanf((char*)eoh,"%x\r\n",&request->chunkbytesremaining);
		printf("%x (%d)\n",request->chunkbytesremaining,request->chunkbytesremaining);
		
		char num[10];
		sscanf((char*)eoh,"%s\r\n",num);
		printf("Chunk size: %ld\n",request->chunkbytesremaining);
		int32 bufdata=0;
		int32 bytes=((eoh+eoh_len)+strlen(num)+1)-buf;
		printf("header bytes: %ld\nbytes left: %ld\n",bytes,size-bytes);
		//added at 7:25 pm on 5-Jan-02
		unsigned char *start=NULL;
			start=eoh+(eoh_len+strlen(num)+2);
		
//			DoneWithHeaders(request);
		
		ProcessChunkedData(request,start,(size-(eoh_len+strlen(num)+2)));
	} else {	
		request->bytesremaining-=(size-((eoh+eoh_len)-buf));
		
		if (request->bytesremaining<=0)
			CloseRequest(request);
		
		request->data->WriteAt(0,(eoh+eoh_len),(size-((eoh+eoh_len)-buf)));
	}
		printf("[PH] Bytes remaining: %ld\n",request->chunked?request->chunkbytesremaining:request->bytesremaining);
	}//headers were null... we're not continuing them.
	else {
		sscanf((char*)buffer,"%x\r\n",&request->chunkbytesremaining);
		printf("%x (%d)\n",request->chunkbytesremaining,request->chunkbytesremaining);
		char num[10];
		sscanf((char*)buffer,"%s\r\n",num);
		printf("Chunk size: %ld\n",request->chunkbytesremaining);
		ProcessChunkedData(request,((unsigned char*)buffer)+(strlen(num)+2),size-(strlen(num)+2));
	}
	
	
}
*/
void httplayer::DoneWithHeaders(http_request *request) {
	printf("DoneWithHeaders\n");
	atomic_add(&request->headersdone,1);
	char *result;
	result=FindHeader(request,"transfer-encoding");
	if (result!=NULL) {//transfer encoding
		if (strcasecmp(result,"chunked")==0)
			request->chunked=true;
		printf("chunked: %d\n",request->chunked);
	}//transfer encoding
	result=FindHeader(request,"content-length");
	if ((!request->chunked) && (result!=NULL)) {//content length
		//according to specs, a content-length and chunked transfer encoding can't be
		//specified together.
		request->bytesremaining=atol(result);
	}//content length
	result=FindHeader(request,"cache-control");
	if (result!=NULL) {//Cache control header
				if (strcasecmp(result,"no-cache")==0) {
					request->cache=DoesNotUseCache;
				}
	}//Cache control header
	result=FindHeader(request,"Pragma");
	if (result!=NULL) {//Pragma Header
				if  (strcasecmp(result,"no-cache")==0) {
					request->cache=DoesNotUseCache;
				}
	}//Pragma Header
	result=FindHeader(request,"www-authenticate");
	if (result!=NULL) {//Authentication Header
				if (strncasecmp(result,"basic",5)==0) {
					char *q1=NULL,*q2=NULL;
					q1=strchr(result,'"');
					q2=strchr(q1+1,'"');
					if ((q1!=NULL) && (q2!=NULL)) {
						int size=q2-(q1+1);
							char *rlmc=new char[size+1];
							memset(rlmc,0,size+1);
							strncpy(rlmc,q1+1,size);
							printf("auth realm: %s\n",rlmc);
						if (Basic!=NULL) {
							auth_realm *realm=FindAuthRealm(rlmc,request->host);
							if (realm!=NULL) {
								printf("auth realm found: %p\n",realm);
								
							} else {
								printf("auth realm must be added\n");
								(new authwin("Password required!",request,rlmc));
							}
						} else {
							printf("auth realm must be added\n");
							(new authwin("Password required!",request,rlmc));
						}
						delete rlmc;
					}
					
				}
	}//Authentication header
	result=NULL;
	
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
			request->cacheinfo=new BMessage;
			BMessenger *msgr=new BMessenger(CachePlug->Handler(),NULL,NULL);
			msgr->SendMessage(ci,request->cacheinfo);
			delete ci;
			request->cacheinfo->PrintToStream();
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
			while (curhead!=NULL) {
				ui->AddString(curhead->name,curhead->value);
				curhead=curhead->next;
			}
			BMessenger *msgr=new BMessenger(CachePlug->Handler(),NULL,NULL);
			msgr->SendMessage(ui,reply);
			reply->PrintToStream();
			delete request->cacheinfo;
			request->cacheinfo=reply;
			delete ui;
			if (request->data==NULL) {
				printf("Creating cache file.\n");
				request->data=new BFile(&ref,B_READ_WRITE);
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
	if ((request->bytesremaining==-1) && (request->http_v_major==1) && (request->http_v_minor==0)) {
		request->receivetilclosed=true;
	}
}

/*
void httplayer::DoneWithHeaders(http_request *request) {
	request->headersdone=true;
	header_st *head=request->headers,*curhead=request->headers;
	while(curhead!=NULL) {
			if ((strcasecmp(curhead->name,"transfer-encoding")==0) && (strcasecmp(curhead->value,"chunked")==0)) {
				request->chunked=true;
			
			}
			if (strcasecmp(curhead->name,"Content-length")==0) {
				printf("[DWH]content length: %ld\n",atol(curhead->value));
				request->bytesremaining=atol(curhead->value);
			}
			if (strcasecmp(curhead->name,"Cache-Control")==0) {
				if (strcasecmp(curhead->value,"no-cache")==0) {
					request->cache=DoesNotUseCache;
				}
			}
			if (strcasecmp(curhead->name,"Pragma")==0) {//Pragma
				if  (strcasecmp(curhead->value,"no-cache")==0) {
					request->cache=DoesNotUseCache;
				}
			}//Pragma
			if (strcasecmp(curhead->name,"WWW-Authenticate")==0) {
				if (strncasecmp(curhead->value,"basic",5)==0) {
					char *q1=NULL,*q2=NULL;
					q1=strchr(curhead->value,'"');
					q2=strchr(q1+1,'"');
					if ((q1!=NULL) && (q2!=NULL)) {
						int size=q2-(q1+1);
							char *rlmc=new char[size+1];
							memset(rlmc,0,size+1);
							strncpy(rlmc,q1+1,size);
							printf("auth realm: %s\n",rlmc);
						if (Basic!=NULL) {
							auth_realm *realm=FindAuthRealm(rlmc,request->host);
							if (realm!=NULL) {
								printf("auth realm found: %p\n",realm);
								
							} else {
								printf("auth realm must be added\n");
								(new authwin("Password required!",request,rlmc));
							}
						} else {
							printf("auth realm must be added\n");
							(new authwin("Password required!",request,rlmc));
						}
						delete rlmc;
					}
					
				}
			}
		curhead=curhead->next;
	}
	if (request->cache!=DoesNotUseCache) {
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
			request->cacheinfo=new BMessage;
			BMessenger *msgr=new BMessenger(CachePlug->Handler(),NULL,NULL);
			msgr->SendMessage(ci,request->cacheinfo);
			delete ci;
			request->cacheinfo->PrintToStream();
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
			while (curhead!=NULL) {
				ui->AddString(curhead->name,curhead->value);
				curhead=curhead->next;
			}
			BMessenger *msgr=new BMessenger(CachePlug->Handler(),NULL,NULL);
			msgr->SendMessage(ui,reply);
			reply->PrintToStream();
			delete request->cacheinfo;
			request->cacheinfo=reply;
			delete ui;
			if (request->data==NULL) {
				printf("Creating cache file.\n");
				request->data=new BFile(&ref,B_READ_WRITE);
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
			
}
*/
/*
void httplayer::ProcessChunkedData(http_request *request,void *buffer, int size) {
	//call with last known chunk size already set in request->chunkbytesremaining
	//and having just grabbed the chunk size.
	printf("Process ChunkedData\n");
	
	if (quit)
		return;
	if (size<0)
		return;
		int pos=0;
		char *eol=NULL;
		char *eoh_c=NULL;
		int eoh_len=0;
	
	if (request->chunkbytesremaining>0) {
		unsigned char *ubuf=(unsigned char*)buffer;
		char *cbuf=(char*)buffer;
		if (request->headersdone) {
			
			if (request->chunkbytesremaining<size) {
				printf("1Bytes being written:\n");
				for (int i=0;i<request->chunkbytesremaining; i++)
					printf("%c",cbuf[i]);
				printf("\n\t\t*** done\n");
				
				request->data->Write(ubuf,request->chunkbytesremaining);
				pos=request->chunkbytesremaining+1;
				request->chunkbytesremaining=0;
			} else {
				printf("2Bytes being written:\n");
				for (int i=0;i<size;i++)
					printf("%c",cbuf[i]);
				printf("\n\t\t*** done\n");
				
				request->data->Write(ubuf,size);
				request->chunkbytesremaining-=size;
				return;
			}
		} else {//not done with headers yet
			printf("==========\nMore header processing to be done\n===========\n");
			header_st *lasthead=request->headers;
			if (lasthead!=NULL) {
				while (lasthead->next!=NULL)
					lasthead=lasthead->next;
				sscanf(cbuf,"%x\r\n",&request->chunkbytesremaining);
				printf("%x (%d)\n",request->chunkbytesremaining,request->chunkbytesremaining);
				char num[10];
				sscanf(cbuf,"%s\r\n",num);
				printf("Chunk size: %ld\n",request->chunkbytesremaining);
				if (request->chunkbytesremaining==0) {
					CloseRequest(request);
					return;
					
				}
				
				pos=strlen(num)+2;
				eoh_c=strstr(cbuf,"\r\n\r\n");
				if (eoh_c==NULL) {
					eoh_c=strstr(cbuf,"\n\n");
					if (eoh_c==NULL) {
						eoh_c=strstr(cbuf,"\r\r");
						if (eoh_c==NULL) {
							//then what the fuck is the end of header marker??
						} else {
							eoh_len=2;
						}
					} else {
						eoh_len=2;
					}
				} else {
					eoh_len=4;
				}
				if (eoh_c!=NULL) {
					char *colon=strchr(cbuf+pos,':');
					if (colon>eoh_c) {
						//no more headers after all... grrr...
						DoneWithHeaders(request);
						ProcessChunkedData(request,ubuf+(cbuf-eoh_c),(cbuf-eoh_c));
						return;
					}
					eol=strstr(cbuf+pos,"\r\n");
					int namelen=0;
					int vallen=0;
					char *temp;
					while(eol<eoh_c) {
						if (lasthead==NULL) {
							request->headers=new header_st;
							lasthead=request->headers;
						} else {
							lasthead->next=new header_st;
							lasthead=lasthead->next;
						}
						colon=strchr(cbuf+pos,':');
						if (colon!=NULL) {
							namelen=colon-(cbuf+pos);
							lasthead->name=new char[namelen+1];
							memset(lasthead->name,0,namelen+1);
							strncpy(lasthead->name,cbuf+pos,namelen);
							printf("header name [%ld]: %s\t",namelen,lasthead->name);
							fflush(stdout);
							eol=strstr(colon,"\r\n");
							vallen=eol-colon;
							lasthead->value=new char[vallen+1];
							temp=new char[vallen+1];
							memset(temp,0,vallen+1);
							strncpy(temp,colon+1,vallen-1);
							memset(lasthead->value,0,vallen+1);
							stripfrontwhite((const char*)temp,lasthead->value);
							printf("value: %s\n",lasthead->value);
							//process the header info... don't process chunked, we know that already...duh
							
						}
						pos=(eol+2)-cbuf;	
					}
					
					int headersize=(eoh_c+eoh_len)-cbuf;
					
					request->chunkbytesremaining-=headersize;
					pos=headersize;
					if (headersize<size) {
						DoneWithHeaders(request);
						if (request->chunkbytesremaining<(size-headersize)) {//we need to look for additional chunks
							if (request->chunkbytesremaining<=0) {
								sscanf(cbuf+pos,"%x\r\n",&request->chunkbytesremaining);
								printf("[Post Header Chunk] %x (%d)\n",request->chunkbytesremaining,request->chunkbytesremaining);
								sscanf(cbuf+pos,"%s\r\n",num);
								printf("[Post Header Chunk] Chunk size: %ld\n",request->chunkbytesremaining);
								if (request->chunkbytesremaining==0) {
									CloseRequest(request);
									return;
									
								}
								
							}
							ProcessChunkedData(request,ubuf+headersize,size-headersize);
							return;
						} else {//write all remaining data in buffer to the collection depot.
							request->data->Write(ubuf+headersize,size-headersize);
							return;
						}
					}
					
				}//end of header block found
				
			} else {
				//this should be an impossible situation, because we should've already received some headers to inform
				//us that the data being processed is chunked.
			}
			
		}//not done with headers yet
		
			sscanf(cbuf+pos,"%x\r\n",&request->chunkbytesremaining);
			printf("%x (%d)\n",request->chunkbytesremaining,request->chunkbytesremaining);
			
			char num[10];
			sscanf(cbuf+pos,"%s\r\n",num);
	//		if (atol(num)!=request->chunkbytesremaining)
	//			request->chunkbytesremaining=atol(num);
			
			printf("Chunk size: %ld\n",request->chunkbytesremaining);
			if (request->chunkbytesremaining==0){
				
				CloseRequest(request);
				return;
				
			}
			
			ProcessChunkedData(request,ubuf+pos,size-pos);
		
	} else {
		//if the newest chunk size is zero, there shouldn't be any more data to be received.
		//in short, this request should be done.
//		CloseRequest(request);
	}
	
	printf("\t********ProcessChunkedData done.********\n");
	
}
*/
void httplayer::ProcessChunkedData(http_request *request,void *buffer, int size) {
	printf("\tProcessChunkedData()\n");
	if (size==0)
		return;
			int32 oldchunk=0;
		printf("buffer size: %ld chunk size: %ld chunk: %ld\n",size,request->chunkbytesremaining,request->chunk);
	
	if (size>request->chunkbytesremaining) {
		//size is greater than chunkbytesremaining
		printf("chr: %ld\tchunk: %ld\n",request->chunkbytesremaining,request->chunk);
		
		if ((request->chunkbytesremaining==0) && (request->chunk==0)){
			char *eol=(char*)memchr(((char*)buffer),'\r',20);
			char temp[20];
			memset(temp,0,20);
			int len=eol-(char*)buffer;
			
			strncpy(temp,(char*)buffer,len);
			request->chunkbytesremaining=strtol(temp,&eol,16);
			request->chunk++;
			printf("First chunk size is %ld (0x%x) bytes\n",request->chunkbytesremaining,request->chunkbytesremaining);
//			request->data->Write((unsigned char*)(buffer+len+2),request->chunkbytesremaining);
			
			ProcessChunkedData(request,(char*)buffer+len+2,size-(len+2));
			return;
			
		} 
			
		oldchunk=request->chunkbytesremaining;
		request->data->Write((unsigned char*)buffer,request->chunkbytesremaining);
		char *kk=new char[request->chunkbytesremaining+1];
		memset(kk,0,request->chunkbytesremaining+1);
		strncpy(kk,(char*)buffer,request->chunkbytesremaining);
//		printf("======Chunk======\n%s\n++++++++++\n",kk);
		delete kk;
		request->chunkbytesremaining=0;
		
		
		if ((size-oldchunk)>=7) {
			char *eol=NULL;
			
			char *start=NULL;
//			if (request->chunk>0) {
				
			char *rem=(char*)buffer+oldchunk;
			int32 br=size-oldchunk;
				char *rr=new char[br+1];
				memset(rr,0,br+1);
				strncpy(rr,rem,br);
				
//			printf("========Remaining========\n%s\n++++++++++++++\n",rr);
			delete rr;
			
			eol=(char*)memchr(rem,'\r',10);//strstr(rem,"\r\n");
				
			start=eol+2;
			eol=(char*)memchr(start,'\r',10);
//			} else {
//				start=(char*)buffer;
//			}
			
			
			bool good=false;
			//eol=strstr(start,"\r\n");
			if ((eol!=NULL) && (*(eol+1)=='\n'))
				good=true;
			if ((!good) || ((eol-start)>99)) {
				eol=start+4;
				//guess...
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
			printf("New Chunk size: %ld\t%s\n",request->chunkbytesremaining,bs);
			if (request->chunkbytesremaining==0) {
				printf("\tRequest is done!\n");
				CloseRequest(request);
				return;
			}
			
			printf("Call ProcessChunkedData here.\n");
			request->chunk++;
			ProcessChunkedData(request,eol+2,size-((eol-start)+4+oldchunk));
		} else {
//			char *eol=strstr((char*)buffer+request->chunkbytesremaining),"\r\n");
//			char *start=NULL;
//			if (eol!=NULL) {
//				start=eol+2;
//				eol=strstr(start,"\r\n");
//				if (eol==NULL) {
//					if (size
//				}
//				
//			}
//			
		}
		
	} else {
		if (size<request->chunkbytesremaining) {
			//size is less than chunkbytesremaining
			request->data->Write((unsigned char*)buffer,size);
			request->chunkbytesremaining-=size;
		} else {
			//size == chunkbytesremaining
			printf("size==chunkbytesremaining\n");
			
			request->data->Write((unsigned char*)buffer,size);
			request->chunkbytesremaining=0;
			request->chunk++;
		}
		
	}
	printf("ProcessChunkedData done.\n");
}

void httplayer::ProcessData(http_request *request, void *buffer, int size) {
	printf("\tProcessData()\n");

	if (request->chunked) {
		/*
		if (request->chunkbytesremaining==0) {
			char *eol=strstr((char*)buffer,"\r\n");
			char *start=(char*)buffer;
			if (request->chunk>0) {
				start=eol+2;
				eol=strstr(start,"\r\n");
			}
			char *semi=strchr(start,';');
			char *bs=NULL;
			if (semi!=NULL) {
				int len=semi-start;
				bs=new char[len+1];
				memset(bs,0,len+1);
				strncpy(bs,start,len);
				len=0;
			} else {
				int len=eol-start;
				bs=new char[len+1];
				memset(bs,0,len+1);
				strncpy(bs,start,len);
				len=0;
			}
			char *stop=NULL;
			int32 bsize=strtol(bs,&stop,16);
			printf("block/chunk size should be: %ld bytes\n",bsize);
			request->chunkbytesremaining=bsize;
			delete bs;
			ProcessChunkedData(request,eol+2,size-((eol+2)-(char*)buffer));
			bsize=0;
		} else {
			ProcessChunkedData(request,buffer,size);
		}
		*/
		ProcessChunkedData(request,buffer,size);
	} else {
		request->data->Write((unsigned char*)buffer,size);
		request->bytesremaining-=size;
		if (request->bytesremaining<=0) {
			if (request->receivetilclosed) {
				if (TCP->Connected(request->conn,true)) {
					request->bytesremaining=0;
					return;
				}
				
				CloseRequest(request);
			}
			
			CloseRequest(request);
		}
		
	}
	printf("ProcessData done.\n");
	
}

/*
void httplayer::ProcessData(http_request *request,void *buffer,int size) {
	printf("ProcessData:\n%s\n",(char*)buffer);
	
	if (request->chunked) {
		ProcessChunkedData(request,buffer,size);
	} else {
		request->data->Write((unsigned char*)buffer,size);
		request->bytesremaining-=size;
		if (request->bytesremaining<=0)
			CloseRequest(request);
		
	}
//	printf("+++++++++\nBuffer so far:===========\n%s\n----------\n",(char*)request->data->Buffer());
	printf("Bytes remaining in request/chunk: %ld\n",request->chunked?request->chunkbytesremaining:request->bytesremaining);
	
	
}
*/
void httplayer::CloseRequest(http_request *request) {
//	acquire_sem(connhandle_sem);
	//atomic_add(&request->done,1);
	Done(request);
	if (request->conn!=NULL) {
		
									TCP->Lock();
	TCP->RequestDone(request->conn);
					TCP->Unlock();
	atomic_add(&request->conn_released,1);
	request->conn=NULL;
	}
	if (request->conn_released!=0)
		request->conn_released=0;
//	release_sem(connhandle_sem);

}
void httplayer::SendRequest(http_request *request, char *requeststr){
					TCP->Lock();
			request->conn=TCP->ConnectTo('http',request->host,request->port,request->secure);
					TCP->Unlock();
			if (request->conn!=NULL)
				if (request->conn->result>=0) {
					int32 bytes=0;
					TCP->Lock();
					bytes=TCP->Send(&request->conn,(unsigned char *)requeststr,strlen(requeststr));
					TCP->Unlock();
					printf("%ld bytes sent on connection: %p\n",bytes,request->conn);
				} else
					printf("The connection failed.\n");
	
}
		
char *httplayer::BuildRequest(http_request *request){
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
			char *temp=NULL;
			reqstr << "GET " << request->uri <<" HTTP/1.1\r\nHost: " << request->host;
			if (request->port!=80)
				reqstr<<":"<<(int)request->port;
			
			reqstr << "\r\nUser-agent: "<<UserAgent()<<"\r\n";
			temp=GetSupportedTypes();
			reqstr << temp;
			delete temp;
			temp=NULL;
			//check authentication zones vs domain being connected to.
			reqstr << "Connection: keep-alive\r\n";
			auth_realm *realm=FindAuthRealm(request);
			if (realm!=NULL) {
				reqstr<<"Authorization: Basic "<<realm->auth<<"\r\n";
			}
			BMessage *cachestatus=CheckCacheStatus(request);
			if (cachestatus!=NULL) {
				request->cacheinfo=cachestatus;
				if (request->cacheinfo!=NULL) {
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
				}
			}
			reqstr << "\r\n";
			printf("Request string:\n---------\n%s\n---------\n",reqstr.String());
	requeststr=new char[reqstr.Length()+1];
	memset(requeststr,0,reqstr.Length()+1);
	strcpy(requeststr,reqstr.String());
	return requeststr;
}
		

bool httplayer::ResubmitRequest(http_request *request) {
	if (request->done==0)
		CloseRequest(request);
/*
	header_st *heads=request->headers,*tmp;
	while (heads!=NULL) {
		tmp=heads->next;
		delete heads;
		heads=tmp;
		if (quit)
			break;
		
	}

	delete request->host;
	delete request->uri;
	FindURI(&request->url,&request->host,&request->port,&request->uri,&request->secure);
	if (request->data!=NULL) {
		request->data->SetSize(0);
		delete request->data;
		request->data=NULL;
	}
	request->done=0;
	ResubmitReqs->AddItem(request);
*/
	BMessage *info=new BMessage;
	info->AddString("target_url",request->url);
	if (request->referrer!=NULL) {
		info->AddString("referrer",request->referrer);
	}
	KillRequest(request);
	AddRequest(info);
}

int32 httplayer::Lock(int32 timeout) 
{
	thread_id callingthread=find_thread(NULL),lockingthread=lock->LockingThread();
//	printf("calling thread: %ld\tlocking thread: %ld\tlocks: %ld\n",callingthread,lockingthread,lock->CountLocks());
	
	if (lockingthread!=B_ERROR) {
		if (callingthread==lockingthread)
			return B_OK;
	}
	if (timeout==-1) {
		if (lock->Lock()) {
//			printf("calling thread %ld has gotten the lock\n",callingthread);
			
			return B_OK;
		}
//		printf("calling thread failed to get the lock.\n");
		
		return B_ERROR;
		
	} else {
		status_t stat=lock->LockWithTimeout(timeout);
//		printf("status %ld while calling thread %ld attempted to get the lock\n",stat,callingthread);
		return stat;
	}
	
/*
	sem_id targetsem=0;
	sem_info info;
	int32 cookie=0;
	while (get_next_sem_info(0,&cookie,&info)==B_OK) {
		if (strcasecmp(info.name,"httplayer_sem")==0) {
			targetsem=info.sem;
		}
		
	}
	if (targetsem==0)
		return B_ERROR;
	int32 result=B_ERROR;
	if (timeout==-1) {
		result=acquire_sem(targetsem);
	} else {
		result=acquire_sem_etc(targetsem,1,B_ABSOLUTE_TIMEOUT,timeout);
	}
	return result;
*/
}
void httplayer::Unlock() 
{
	thread_id callingthread=find_thread(NULL),lockingthread=lock->LockingThread();
	
	lock->Unlock();
/*
	sem_id targetsem=0;
	sem_info info;
	int32 cookie=0;
	while (get_next_sem_info(0,&cookie,&info)==B_OK) {
		if (strcasecmp(info.name,"httplayer_sem")==0) {
			targetsem=info.sem;
		}
		
	}
	if (targetsem==0)
		return;
	release_sem(targetsem);
*/
}
void httplayer::FindURI(char **url,char **host,uint16 *port,char **uri,bool *secure) {
	printf("finding uri...\n");
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
		printf("urlkind: %s\n",urlkind.String());
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
			printf("username: %s\npassword: %s\n",user.String(),pass.String());
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
	printf("%d is %ld characters wide.\n",*port,width);
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
	printf("new composite url: %s\n",nuuri);
	if (strcasecmp(*uri,nuuri)!=0) {
		delete (*url);
		*url=nuuri;
	} else
		delete nuuri;
}
void httplayer::Done(http_request *request) {
	printf("Marking request as done.8&8\n\tchunked:%s\n\tbytes remaining: %ld\n",request->chunked?"yes":"no",request->chunked?request->chunkbytesremaining:request->bytesremaining);
	
	atomic_add(&request->done,1);
}

int32 httplayer::LayerManager() {
	http_request *request=NULL,*current=NULL, *current2=NULL;
	status_t stat;
	unsigned char *buffer=new unsigned char[10240];
	int32 bytes=0;
	while (!quit) {
		stat=Lock(20000);
		if (stat!=B_OK) {
			snooze(10000);
			continue;
		}
		if (current==NULL) {
			current=requests_head;
		} else
			current=current->next;
		if (current==NULL) {
			Unlock();
			snooze(40000);
			continue;
		}
		if (current->done==0) {
			if (current->datawaiting>=1) {
				/*
					If we timeout while waiting for TCP->Lock() then it's possible
					that TCP is waiting for a lock on this protocol. A deadlock
					would occur if we didn't timeout. So, we can simply pick up
					the data on the next pass.
				*/
				if (TCP->Lock(100000)==B_OK) {
					memset(buffer,0,10240);
					bytes=0;
					bytes=TCP->Receive(&current->conn,buffer,10240);
//					printf("http: data received: %ld bytes\n",bytes);
					TCP->Unlock();
					if (bytes>0) {
						if (current->headersdone!=1) {
							ProcessHeaders(current,buffer,bytes);
						} else {
							ProcessData(current,buffer,bytes);
						}
					}
				}
			}
		}
		Unlock();
		snooze(20000);
		
	}
	memset(buffer,0,10240);
	delete buffer;
	buffer=NULL;
	return 0;
}

/*
int32 httplayer::LayerManager() {
	http_request *request=NULL,*current=NULL, *current2=NULL;
	unsigned char *altbuffer=new unsigned char[100000];
	uint32 altbytes=0;
	memset(altbuffer,0,100000);
	while (!quit) {
		if (quit) {
			
			printf("LayerManager quit raised\n");
			break;
			
		}
		
		if (current==NULL) {
			current=requests_head;
		}
//		acquire_sem(connhandle_sem);
		if (ResubmitReqs->CountItems()>0) {
			request=(http_request *)ResubmitReqs->RemoveItem((int32)0);
			//build request string
			BString reqstr;
			char *temp=NULL;
			reqstr << "GET " << request->uri <<" HTTP/1.1\r\nHost: " << request->host;
			if (request->port!=80)
				reqstr<<":"<<(int)request->port;
			
			reqstr << "\r\nUser-agent: "<<UserAgent()<<"\r\n";
			temp=GetSupportedTypes();
			reqstr << temp;
			delete temp;
			temp=NULL;
			//check authentication zones vs domain being connected to.
			reqstr << "Connection: keep-alive\r\n";
			auth_realm *realm=FindAuthRealm(request);
			if (realm!=NULL) {
				reqstr<<"Authorization: Basic "<<realm->auth<<"\r\n";
			}
			BMessage *cachestatus=CheckCacheStatus(request);
			if (cachestatus!=NULL) {
				request->cacheinfo=cachestatus;
				if (request->cacheinfo!=NULL) {
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
				}
			}
			reqstr << "\r\n";
			printf("Request string:\n---------\n%s\n---------\n",reqstr.String());
			TCP->Lock();
			request->conn=TCP->ConnectTo('http',request->host,request->port,request->secure);
			TCP->Unlock();
			if (request->conn!=NULL)
				if (request->conn->result>=0) {
					int32 bytes=0;
					TCP->Lock();
					
					bytes=TCP->Send(&request->conn,(unsigned char *)reqstr.String(),reqstr.Length());
					TCP->Unlock();
					printf("%ld bytes sent on connection: %p\n",bytes,request->conn);
				}
		}
		
		if (UnprocessedReqs->CountItems()>0) {
			request=(http_request *)UnprocessedReqs->RemoveItem((int32)0);
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
			//build request string
			BString reqstr;
			char *temp=NULL;
			reqstr << "GET " << request->uri <<" HTTP/1.1\r\nHost: " << request->host;
			if (request->port!=80)
				reqstr<<":"<<(int)request->port;
			
			reqstr << "\r\nUser-agent: "<<UserAgent()<<"\r\n";
			temp=GetSupportedTypes();
			reqstr << temp;
			delete temp;
			temp=NULL;
			//check authentication zones vs domain being connected to.
			reqstr << "Connection: keep-alive\r\n";
			auth_realm *realm=FindAuthRealm(request);
			if (realm!=NULL) {
				reqstr<<"Authorization: Basic "<<realm->auth<<"\r\n";
			}
			BMessage *cachestatus=CheckCacheStatus(request);
			if (cachestatus!=NULL) {
				request->cacheinfo=cachestatus;
				if (request->cacheinfo!=NULL) {
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
				}
			}
			reqstr << "\r\n";
			printf("Request string:\n---------\n%s\n---------\n",reqstr.String());
					TCP->Lock();
			request->conn=TCP->ConnectTo('http',request->host,request->port,request->secure);
					TCP->Unlock();
			if (request->conn!=NULL)
				if (request->conn->result>=0) {
					int32 bytes=0;
					TCP->Lock();
					bytes=TCP->Send(&request->conn,(unsigned char *)reqstr.String(),reqstr.Length());
					TCP->Unlock();
					printf("%ld bytes sent on connection: %p\n",bytes,request->conn);
				}
		}
//		release_sem(connhandle_sem);
		
		if (current!=NULL) {
//			printf("current %p datawaiting: %ld\n",current,current->datawaiting);
			if (!current->done) {
					
			if (current->datawaiting>=1) {
				printf("processing request for %s\n",current->url);
				unsigned char *buff=new unsigned char[10001];
				memset(buff,0,10001);
				int32 bytes=0;
				altbytes=0;
				memset(altbuffer,0,100000);
				do {
					Lock();
					
//					if (TCP->Lock()!=true) {
//						printf("TCP Locked by: %ld\nTCP Locks in place: %ld\nRequests: %ld\n",TCP->lock->LockingThread(),TCP->lock->CountLocks(),TCP->lock->CountLockRequests());
//						continue;
//					}
					TCP->Lock();
					bytes=TCP->Receive(&current->conn,buff,10000);
				atomic_add(&current->datawaiting,-1);
					TCP->Unlock();
					if (current->headers==NULL) {
						char *dat=(char*)buff;
						char *eoh=NULL;
						eoh=strstr(dat,"\r\n\r\n");

						if (eoh==NULL) {
							memmove(altbuffer+altbytes,buff,bytes);
							altbytes+=bytes;
							memset(buff,0,10001);
							if (quit) {
								Unlock();
								
								break;//just to make sure...
							}
							Unlock();
							
							continue;
						}

					}
				printf("%ld bytes received\n",bytes);
				if (altbytes==0) {
					if (bytes>0) {
						if ((current->headers==NULL) || (!current->headersdone)) {
							printf("About to process headers\n");
							ProcessHeaders(current,buff,bytes);
						} else {
							printf("Adding data to buffer\n");
							ProcessData(current,buff,bytes);
							
							//current->data->WriteAt(current->data->BufferLength(),buff,bytes);
						}
						
					}
				} else {
					printf("About to process headers [alt]\n");
					ProcessHeaders(current,altbuffer,bytes);
					memset(altbuffer,0,100000);
					altbytes=0;
				}
				memset(buff,0,10001);
				Unlock();
					
				if (request->done>0)
					break;
				if (quit)
					break;
					
				}while (bytes>0);
				
							
			delete buff;
			}
//			acquire_sem(connhandle_sem);
			if (current->conn!=NULL)
				if ((current->conn->open==false) && (current->done==false)) {
					CloseRequest(current);//atomic_add(&current->done,1);
				}
//			release_sem(connhandle_sem);
				
		if ((current->done) && (!current->conn_released)){
			
			//TCP->RequestDone(current->conn);
			//atomic_add(&current->conn_released,1);
//			printf("Request:\n%s\n==========\n",(char*)current->data->Buffer());
			
			continue;
			
		}
		}
		
		}
//		if (current!=NULL)
//		printf("current: %p\n",current);
		
		if (current!=NULL)
			current=current->next;
		else
			continue;
		snooze(50000);//give other processes some time
	}
	delete altbuffer;
	printf("LayerManager Exiting: %s\n",quit==1?"quit issued":"something else");
	exit_thread(0);
	return B_OK;
}
*/

BMessage *httplayer::CheckCacheStatus(http_request *request) {
	if (request!=NULL) {
		if (CachePlug==NULL)
			PluginManager->FindPlugin(CachePlugin);
		if (CachePlug==NULL)
			return NULL;//if it's still null, it means the plug-in isn't loaded.
		BMessenger *msgr=new BMessenger(CachePlug->Handler(),NULL,NULL);
		BMessage *msg=new BMessage(FindCachedObject);
		msg->AddString("url",request->url);
		BMessage *reply=new BMessage(B_ERROR);
		msgr->SendMessage(msg,reply);
		delete msg;
		delete msgr;
		if (reply->what==CacheObjectNotFound){
			delete reply;
			return NULL;
		}
		reply->PrintToStream();
		return reply;
	} else
		return NULL;
}
void httplayer::Start() {
	if (acquire_sem_etc(http_mgr_sem,1,B_ABSOLUTE_TIMEOUT,100000)==B_OK) {
		thread=spawn_thread(StartLayer,"http_layer_manager",B_LOW_PRIORITY,this);
		resume_thread(thread);
		printf("http layer manager thread: %ld\n",thread);
		
	}
	else
	{
		fprintf(stderr,"http_layer_manager already running.\n");
		
	}
	
}

int32 httplayer::StartLayer(void *arg) {
	printf("http layer starting\n");
	httplayer *obj=(httplayer*)arg;
	return obj->LayerManager();
}

status_t httplayer::Quit() {
	Lock();
	printf("http layer stopping\n");
	
	atomic_add(&quit,1);
	printf("\"Quit\" is set\n");
	status_t stat=B_OK;
	printf("Waiting for thread death\n");
	wait_for_thread(thread,&stat);
	printf("releasing semaphore\n");
	release_sem(http_mgr_sem);
	printf("quitting\n");
	return stat;
}
//*
char *httplayer::GetSupportedTypes() {
	BString output("Content-Type: text/plain; text/html");
	//insert code here to get content types from other plug-ins via the plug-in manager
	output << "\r\n";
	char *out=new char[output.Length()+1];
	strcpy(out,output.String());
	return out;
}
char *httplayer::UserAgent() {
	
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
