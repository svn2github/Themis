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

#ifndef _http_layer
#define _http_layer
#include "tcplayer.h"
#include <File.h>
#include <stdlib.h>
#include <List.h>
#include <DataIO.h>
#include <Message.h>
#include <be/kernel/OS.h>
#include <stdio.h>
#include <String.h>
struct auth_realm {
	char *auth;
	char *host;
	char *baseuri;
	char *realm;
	auth_realm *next;
	auth_realm() {
		host=NULL;
		realm=NULL;
		next=NULL;
		auth=NULL;
	}
	~auth_realm() {
		if (auth)
			delete auth;
		if (host)
			delete host;
		host=NULL;
		if (realm)
			delete realm;
		auth=realm=host=NULL;
		next=NULL;
	}
};

struct auth_req {
	uint16 port;
	auth_realm *realms_head;
	auth_req() {
		port=80;
		realms_head=NULL;
	}
	~auth_req() {

		if (realms_head) {
			auth_realm *cur;
			while (realms_head!=NULL) {
				cur=realms_head->next;
				delete realms_head;
				realms_head=cur;
			}
		}
	}
};
struct cacheinfo_st {
	
};


struct header_st {
	char *name;
	char *value;
	header_st *next;
	header_st() {
		name=NULL;
		value=NULL;
		next=NULL;
	}
	~header_st() {
		if (name)
			delete name;
		if (value)
			delete value;
		name=value=NULL;
		next=NULL;
	}
};


struct http_request {
	auth_realm *a_realm;//authentication realm
	char *url;
	char *uri;
	char *host;
	uint16 status;
	uint16 port;
	bool secure;
	int16 http_v_major,http_v_minor;
	BMallocIO *data;
	connection *conn;
	header_st *headers;
	bool headersdone;
	volatile int32 datawaiting;
	volatile int32 done;
	volatile int32 conn_released;
	http_request *next;
	bool chunked;
	int32 bytesremaining;
	int32 chunkbytesremaining;
	http_request() {
		a_realm=NULL;
		url=uri=host=NULL;
		port=80;
		status=0;
		bytesremaining=0;
		conn=NULL;
		headers=NULL;
		next=NULL;
		data=new BMallocIO;
		data->SetBlockSize(1);
		data->SetSize(1);//take minimum space
		secure=false;
		datawaiting=0;
		done=0;
		http_v_major=http_v_minor=0;
		chunked=false;
		conn_released=0;
		chunkbytesremaining=0;
		headersdone=false;
	}
	~http_request() {
		printf("So many hopes and dreams, gone... lost forever... %s:%u%s\n",host,port,uri);
		a_realm=NULL;//do not delete this; it's done else where. so just null it out
		if (url)
			delete url;
		if (uri)
			delete uri;
		if (host)
			delete host;
		url=uri=host=NULL;
		/*
		Important safety note:
			Even though we're deleting most objects that are assigned to this structure
		we should not delete the connection object (conn). Doing so, will cause the TCP
		layer to crash when it is trying to do it's clean up, such as when the connection
		hasn't been used in a long period of time, or when the app is shutting down. Just
		set the conn value here to NULL and be satisfied.
		*/
		conn=NULL;
		if (headers) {
			header_st *cur=headers;
			while (headers!=NULL) {
//				printf("header: %s\tvalue: %s\n",cur->name,cur->value);
				cur=headers->next;
				delete headers;
				headers=cur;
			}
		}
//		printf("This request held the following data:\n");
//		for (int i=0; i<data->BufferLength(); i++)
//			fprintf(stdout,"%c",((unsigned char*)data->Buffer())+i);
		
		if (data!=NULL)
			delete data;
		data=NULL;
		next=NULL;
	}
};


class httplayer {
	private:
		auth_req *Basic;
		http_request *requests_head;
		BFile *file;
		BList *UnprocessedReqs;//unprocessed requests
		BList *ResubmitReqs;//requests to be reprocessed
		volatile int32 quit;
		auth_realm *FindAuthRealm(char *realm, char *host);
		auth_realm *FindAuthRealm(http_request *request);
		void UpdateAuthRealm(auth_realm *realm,char *user,char *pass);
	public:
		auth_realm *AddAuthRealm(http_request *request,char *realm, char *user, char *password);
		int32 use_useragent;
		char * UserAgent();
		thread_id thread;
		tcplayer *TCP;
		void ClearRequests();
		void GetURL(BMessage *info);
		void SetTCP(tcplayer *_TCP);
		void CloseRequest(http_request *request);
		bool ResubmitRequest(http_request *request);
		void DReceived(connection *conn);
		httplayer(tcplayer *_TCP);
		~httplayer();
		void Start();
		sem_id http_mgr_sem;
		static int32 StartLayer(void *arg);
		int32 LayerManager();
		status_t Quit();
		char * GetSupportedTypes();
		http_request *AddRequest(BMessage *info);
		void ProcessHeaders(http_request *request,void *buffer,int size);
		void ProcessData(http_request *request,void *buffer, int size);
		void ProcessChunkedData(http_request *request, void *buffer, int size);
	
		int find_lcd(int fact1,int fact2,int start);
		int32 b64encode(void *unencoded,int32 in_size,char *encoded,int32 maxsize);
		int32 b64decode(char *encoded,unsigned char **decoded,int32 *size);
		void FindURI(const char *url,char **host,uint16 *port,char **uri,bool *secure);
		
};

#endif
