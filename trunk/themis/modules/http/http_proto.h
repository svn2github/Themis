/*
Copyright (c) 2003 Z3R0 One. All Rights Reserved.

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

Original Author & Project Manager: Z3R0 One (z3r0_one@bbnk.dhs.org)
Project Start Date: October 18, 2000
*/

#ifndef _http_proto
#define _http_proto
//#ifdef _Themis_
#include "httplayer.h"
//#endif
#include "protocol_plugin.h"
#include "optionshandler.h"
#ifdef NEWNET
#include "tcpmanager.h"
#include "connection.h"
#endif
#include "smt.h"
#include <View.h>
#include <Window.h>
#include <Locker.h>
  extern "C" __declspec(dllexport)status_t Initialize(void *info=NULL);
  extern "C" __declspec(dllexport)status_t Shutdown(bool now=false);
  extern "C" __declspec(dllexport)protocol_plugin* GetObject(void);
class CookieManager;
class http_protocol:public ProtocolPlugClass
   {
    private:
	friend void quitalarm(int signum);
	   
     port_id http_proto_port;
     size_t buflen,rawbuflen;
     bool chunked;
     int32 chunksize;
     bool headerreceived;
     BList *Threads;
     BList *queue;
     int32 benval;
     sem_id httpsem;
//#ifdef _Themis_
	 httplayer *HTTP;
//#endif
	 http_opt_handler *HOH;  
	 BView *view;
	 BWindow *win;
	BMessage *AppSettings;
	BMessage **AppSettings_p;
		BLocker *lock;
	   
    public:
		void FindURI(const char *url,BString &host,uint16 *port,BString &uri,bool *secure);
     http_protocol(BMessage *info=NULL);
     ~http_protocol();
     BMessage headers;
     char *PlugName(void);
     uint32 PlugID(void);
     float PlugVersion(void);
     status_t Go(void);
     int32 SpawnThread(BMessage *info);
     bool IsPersistent();
     void Stop();
     int32 GetURL(BMessage *info);
     static int32 ThreadFunc(void *info);
	 void AddMenuItems(BMenu *menu);
	 void RemoveMenuItems();
	 struct smt_st *smthead;
//#ifdef _Themis_
	 friend httplayer;
//#endif
  	 int32 Type();
 	 status_t ReceiveBroadcast(BMessage *msg);
	 status_t BroadcastReply(BMessage *msg);
	 BMessage *cache_reply;
//	 bool RequiresHeartbeat(){return true;}
	 void Heartbeat();
	 friend class CookieManager;
	 uint32 BroadcastTarget();
		void ConnectionEstablished(connection *conn);
		void ConnectionDisconnected(connection *conn,uint32 reason);
		void DataWaiting(connection *conn);
#ifdef NEWNET
	_Themis_Networking_::TCPManager *TCPMan;
	void ConnectionEstablished(_Themis_Networking_::Connection *connection);
	void ConnectionAlreadyExists(_Themis_Networking_::Connection *connection);
	void ConnectionTerminated(_Themis_Networking_::Connection *connection);
	void DataIsWaiting(_Themis_Networking_::Connection *connection);
	void ConnectionError(_Themis_Networking_::Connection *connection);
	void ConnectionFailed(_Themis_Networking_::Connection *connection);
	void DestroyingConnectionObject(_Themis_Networking_::Connection *connection);
#endif	
   };
#endif
