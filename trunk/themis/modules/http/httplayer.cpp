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

void DataReceived(connection *conn) {
	meHTTP->DReceived(conn);
}

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
	
}
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

void httplayer::SetTCP(tcplayer *_TCP) {
	TCP=_TCP;
	if (TCP!=NULL)
		TCP->SetDRCallback((int32)'http',DataReceived);
}

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
/*
	unsigned char *buff=new unsigned char[1001];
	memset((char*)buff,0,1001);
	int32 bytes=TCP->Receive(&conn,buff,1000);
	if (bytes>0)
		;
	if ((req!=NULL) && (bytes>0)) {
		if (req->headers==NULL)
			ProcessHeaders(req,buff,bytes);
		else
			req->data->WriteAt(req->data->BufferLength(),buff,bytes);
	}
	delete buff;
*/	
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
for (long i=0;i<in_size;i++)
{
for (int j=7;j>-1;j--)
{
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
for (int j=5; j>-1;j--)
{
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
} while(i<(in_size*8));//((pos/8)*6));
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

http_request *httplayer::AddRequest(BMessage *info) {
	http_request *request=new http_request;
		if (info->HasInt32("browser_string"))
			info->FindInt32("browser_string",&use_useragent);
		BString url;
		info->FindString("target_url",&url);
		request->url=new char[url.Length()+1];
	strcpy(request->url,url.String());
	 FindURI(url.String(),&request->host,&request->port,&request->uri,&request->secure);
#ifndef USENETSERVER
	printf("Request is to: %s:%u\nURI: %s\n",request->host,request->port,request->uri);
	UnprocessedReqs->AddItem(request);
	delete info;
	printf("End AddRequest\n");
	return request;
#else
	if (request->secure) {
		printf("OpenSSL not supported in this build.");
		(new BAlert("SSL Not Supported","I'm afraid that this version of Themis\nwas built without OpenSSL support.\nUnfortunately, it means that you can't\nvisit HTTPS sites.","Damn."))->Go();
		delete info;
		delete request;
		return NULL;
	} else {
		printf("Request is to: %s:%u\nURI: %s\n",request->host,request->port,request->uri);
		UnprocessedReqs->AddItem(request);
		delete info;
		printf("End AddRequest\n");
		return request;
	}
#endif
}

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
	printf("buf: %p\neoh: %p\ndiff: %ld\neoh len: %d\n",buf,eoh,eoh-buf,eoh_len);
	printf("=========\nbuf:\n%s\n=========\neoh:\n%s\n=========\n",buf,(eoh+eoh_len));
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
			if ((strcasecmp(curhead->name,"transfer-encoding")==0) && (strcasecmp(curhead->value,"chunked")==0)) {
				request->chunked=true;
			
			}
			if (strcasecmp(curhead->name,"Content-length")==0) {
				printf("[PH]content length: %ld\n",atol(curhead->value));
				request->bytesremaining=atol(curhead->value);
			}
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
		
			request->headersdone=true;
		
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
	
	if (conthead!=true)
		request->headersdone=true;
	
}
void httplayer::ProcessChunkedData(http_request *request,void *buffer, int size) {
	//call with last known chunk size already set in request->chunkbytesremaining
	//and having just grabbed the chunk size.
	if (quit)
		return;
	if (size<0)
		return;
	
//	printf("ProcessChunkedData:\trequest: %p\tbuffer: %p\tsize: %d\n",request,buffer,size);
//	printf("Data in Chunk:\n%s\n\t\t****done\n",(char*)buffer);
//	printf("request->chunkbytesremaining: %ld\n",request->chunkbytesremaining);
	
		int pos=0;
		char *eol=NULL;
		char *eoh_c=NULL;
		int eoh_len=0;
	
	if (request->chunkbytesremaining>0) {
		unsigned char *ubuf=(unsigned char*)buffer;
		char *cbuf=(char*)buffer;
		if (request->headersdone) {
			
			if (request->chunkbytesremaining<size) {
				printf("Bytes being written:\n");
				for (int i=0;i<request->chunkbytesremaining; i++)
					printf("%c",cbuf[i]);
				printf("\n\t\t*** done\n");
				
				request->data->Write(ubuf,request->chunkbytesremaining);
				pos=request->chunkbytesremaining+1;
				request->chunkbytesremaining=0;
			} else {
				printf("Bytes being written:\n");
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
						request->headersdone=true;
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
					
/*
//lines below placed here for reference purposes
	while (eol<(char*)eoh) {
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
			if ((strcasecmp(curhead->name,"transfer-encoding")==0) && (strcasecmp(curhead->value,"chunked")==0)) {
				request->chunked=true;
			
			}
			if (strcasecmp(curhead->name,"Content-length")==0) {
				printf("[PH]content length: %ld\n",atol(curhead->value));
				request->bytesremaining=atol(curhead->value);
			}
			
		}
		pos=(eol+2)-(char*)buf;
		
	}
//lines above placed here for reference purposes
*/					
					int headersize=(eoh_c+eoh_len)-cbuf;
					
					request->chunkbytesremaining-=headersize;
					pos=headersize;
					if (headersize<size) {
						request->headersdone=true;
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

void httplayer::ProcessData(http_request *request,void *buffer,int size) {
	if (request->chunked) {
		ProcessChunkedData(request,buffer,size);
	} else {
		request->data->WriteAt(request->data->BufferLength(),(unsigned char*)buffer,size);
		request->bytesremaining-=size;
		if (request->bytesremaining<=0)
			CloseRequest(request);
		
	}
//	printf("+++++++++\nBuffer so far:===========\n%s\n----------\n",(char*)request->data->Buffer());
	printf("Bytes remaining in request/chunk: %ld\n",request->chunked?request->chunkbytesremaining:request->bytesremaining);
	
	
}
void httplayer::CloseRequest(http_request *request) {
	atomic_add(&request->done,1);
	TCP->RequestDone(request->conn);
	atomic_add(&request->conn_released,1);

}
bool httplayer::ResubmitRequest(http_request *request) {
	CloseRequest(request);
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
	FindURI(request->url,&request->host,&request->port,&request->uri,&request->secure);
	
	request->data->SetSize(0);
	request->done=0;
	ResubmitReqs->AddItem(request);
}

void httplayer::FindURI(const char *url,char **host,uint16 *port,char **uri,bool *secure) {
	printf("finding uri...\n");
	 
  BString master(url),servant;
  int32 href=0,urltype=0,slash=0,colon=0,quote=0,at=0;
  bool auth=false;
	BString user,pass;
	
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
	
  if (slash!=B_ERROR)
   {
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
  *host=new char[master.Length()+1];
  strcpy(*host,master.String());
  }
int32 httplayer::LayerManager() {
	http_request *request=NULL,*current=NULL, *current2=NULL;
	while (!quit) {
		if (quit) {
			
			printf("LayerManager quit raised\n");
			break;
			
		}
		
		if (current==NULL) {
			current=requests_head;
		}
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
			
			reqstr << "\r\n";
			printf("Request string:\n---------\n%s\n---------\n",reqstr.String());
			request->conn=TCP->ConnectTo('http',request->host,request->port,request->secure);
			if (request->conn->result>=0) {
			int32 bytes=0;
			bytes=TCP->Send(&request->conn,(unsigned char *)reqstr.String(),reqstr.Length());
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
			reqstr << "\r\n";
			printf("Request string:\n---------\n%s\n---------\n",reqstr.String());
			request->conn=TCP->ConnectTo('http',request->host,request->port,request->secure);
			if (request->conn->result>=0) {
			int32 bytes=0;
			bytes=TCP->Send(&request->conn,(unsigned char *)reqstr.String(),reqstr.Length());
			printf("%ld bytes sent on connection: %p\n",bytes,request->conn);
			}
		}
		if (current!=NULL) {
//			printf("current %p datawaiting: %ld\n",current,current->datawaiting);
			if (!current->done) {
					
			if (current->datawaiting==1) {
				printf("processing request for %s\n",current->url);
				unsigned char *buff=new unsigned char[10001];
				memset(buff,0,10001);
				int32 bytes=0;
				do {
					bytes=TCP->Receive(&current->conn,buff,10000);
					
				printf("%ld bytes received\n",bytes);
				
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
				memset(buff,0,10001);
				if (request->done>0)
					break;
				if (quit)
					break;
					
				}while (bytes>0);
				
							
				atomic_add(&current->datawaiting,-1);
			delete buff;
			}
		if ((current->conn->open==false) && (current->done==false)) {
				CloseRequest(current);//atomic_add(&current->done,1);
			}
			
		if ((current->done) && (!current->conn_released)){
			
			//TCP->RequestDone(current->conn);
			//atomic_add(&current->conn_released,1);
			printf("Request:\n%s\n==========\n",(char*)current->data->Buffer());
			
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
	printf("LayerManager Exiting: %s\n",quit==1?"quit issued":"something else");
	exit_thread(0);
	return B_OK;
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
char *httplayer::GetSupportedTypes() {
	BString output("Content-Type: text/plain; text/html");
	//insert code here to get content types from other plug-ins via the plug-in manager
	output << "\r\n";
	char *out=new char[output.Length()+1];
	strcpy(out,output.String());
	return out;
}
char *httplayer::UserAgent() {
	/*
		Return the identity that the user selects. This should allow Themis access to pages
		that have been written specifically for one or a few browsers; I hate those pages.
		Bastards.
	*/
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

