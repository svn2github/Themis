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

/*
Include *both* plugclass.h *and* plugclass.cpp in your plugin!
*/

//Themis network module round 3
#ifndef _http_main
#define _http_main
#include <sys/socket.h>
#include <NetAddress.h>
#ifdef USENETSERVER
#include <NetKit.h>
#endif
#include <stdio.h>
#include "commondefs.h"
//#include "plugman.h"
#include <String.h>
#include <errno.h>
#include <File.h>
#include <Path.h>
#include <SupportKit.h>
#include <AppKit.h>
#include <KernelKit.h>
#include <Locker.h>
#include "protocol_plugin.h"
#ifdef USEOPENSSL
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/ssl23.h>
#endif
#define TCP_DEFAULT_TIME_TO_LIVE 60
#define BUFFER_SIZE 16384
#ifdef USEBONE
#include <netdb.h>
#define closesocket close
#endif
//#define CHK_NULL(x) if ((x)==NULL) {ERR_print_errors_fp(stdout); }

//#define CHK_ERR(err,s) if ((err)==-1) { ERR_print_errors_fp(stdout);  }
//#define CHK_SSL(err) if ((err)==-1) { ERR_print_errors_fp(stdout);  }

class mutex {
	public:
		mutex(){
		}
		~mutex(){
		}
		status_t lock() const {
			return _locker.Lock() ? B_NO_ERROR : B_ERROR;
		}
		status_t unlock() const {
			_locker.Unlock();
			return B_NO_ERROR;
		}
		mutable BLocker _locker;
};

struct connection {
	int32 proto_id;
	uint16 port;
	volatile int32 closedcbdone;
	volatile int32 callbackdone;
	int32 socket;
	uint32 last_trans_time;//last transaction time
	uint32 time_to_live;
	volatile uint8 open;
#ifdef USEOPENSSL
	bool usessl;
//	#ifndef BONE_VERSION
	BIO *sslbio;
//	#endif
	SSL* ssl;
	X509* server_cert;
#endif
	int result;
	volatile int32 requests;
//	BNetAddress *address;
	struct hostent *hptr;
	in_addr **pptr;
	BString addrstr;
	connection() {
#ifdef USEOPENSSL
		sslbio=NULL;
		usessl=false;
		server_cert=NULL;
		ssl=NULL;
#endif
		next=NULL;
		closedcbdone=0;
		callbackdone=0;
		proto_id=0;
		port=80;
//		address=new BNetAddress;
		time_to_live=TCP_DEFAULT_TIME_TO_LIVE;
		result=0;
		requests=0;
		addrstr="";
		open=false;
		hptr=NULL;
		pptr=NULL;
	}
	~connection() {
		printf("Connection going away, socket %ld\n",socket);
		fflush(stdout);
		char addr[200];
		uint16 port;
		memset(addr,0,200);
		port=0;
		hptr=NULL;
		pptr=NULL;
/*		
		if (address!=NULL) {
			
		address->GetAddr(addr,&port);
		printf(", %s:%d",addr,port);
		fflush(stdout);
		}
		printf(" (%s)\n",open ? "open":"closed");
		fflush(stdout);
*/		
//		if (address!=NULL)
//			delete address;
		if (open)
			closesocket(socket);
#ifdef USEOPENSSL
		if (usessl){
			if (server_cert!=NULL)
				X509_free (server_cert);
			if (ssl!=NULL)
				SSL_free (ssl);
		}
#endif
		next=NULL;
	}
	connection *next;
};

struct DRCallback_st 
{
	int32 protocol;
	void (*callback)(connection *conn);
	void (*connclosedcb)(connection *conn);
	int32 (*Lock)(int32 timeout=-1);
	void (*Unlock)(void);
	DRCallback_st *next;
	DRCallback_st() {
		protocol=0;
		next=NULL;
		callback=NULL;
		Lock=NULL;
		Unlock=NULL;
		connclosedcb=NULL;
	}
	
};


class tcplayer {
	public:
		mutex *mtx;
#ifdef USEOPENSSL
		SSL_METHOD *sslmeth;
	SSL_CTX* sslctx;
#endif
		sem_id tcp_mgr_sem;
		sem_id conn_sem;
		sem_id cb_sem;
		sem_id tcplayer_sem;
		int32 Lock(int32 timeout=-1);
		BLocker *lock;
		void Unlock();
		volatile int32 firstcb;
		thread_id thread;
		volatile int32 quit;
		tcplayer();
		~tcplayer();
		void Start();
		static int32 StartManager(void *arg);
		int32 Manager();
		status_t Quit();
		void SetDRCallback(int32 proto,void (*DataReceived)(connection* conn),int32 (*Lock)(int32 timeout=-1),void(*Unlock)(void));
		void SetConnectionClosedCB(int32 proto, void (*connclosedcb)(connection *conn));
	
		bool IsValid(connection *conn);
		connection *conn_head;//first connection
		connection *prev_conn;
		DRCallback_st *callback_head; //first callback pointer
		connection* ConnectTo(int32 protoid, char *host, int16 port=80, bool ssl=false, bool forcenew=false);
		uint32 Connections();
		bool Connected(connection *conn,bool skipvaild=false);
		connection *NextConnection();
		void KillConnection(connection *target);
		void CloseConnection(connection *target);
		int32 Send(connection **conn,unsigned char *data,int32 size);
		int32 Receive(connection **conn,unsigned char *data,int32 size);
		void RequestDone(connection *target,bool close=false);//this clears the request bit.
	
		bool DataWaiting(connection *conn);
/*
		static void sig_io(int action);
*/	
};

#endif
