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
#ifndef _http_main
#define _http_main
#include <SupportKit.h>
#include <AppKit.h>
#include <KernelKit.h>
#include "protocol_plugin.h"
//namespace Themis_Networking
// {
  extern "C" __declspec(dllexport)status_t Initialize(void *info=NULL);
  extern "C" __declspec(dllexport)status_t Shutdown(bool now=false);
  extern "C" __declspec(dllexport)protocol_plugin* GetObject(void);
class http_worker
 {
  public:
   http_worker(BMessage *info);
   ~http_worker();
   port_id http_proto_port;
   int32 sock;
   BMessage *Info;
   volatile int32 die;
   bool Quit();
   int32 *benval;
   sem_id httpsem;
   thread_id thread;
   static int32 GetIt(void *);
     void FindURI(const char *url,BString &host,uint16 &port,BString &uri);
     BMessage *ProcessRespLine(const char *data);
     BMessage *GetHead(BMessage *info,int32 use_sock=-1);
     BMessage *GetURL(BMessage *info);
     thread_id Thread();
     BList *list;
 };  
class http_protocol:public ProtocolPlugClass
   {
    private:
     port_id http_proto_port;
     size_t buflen,rawbuflen;
     bool chunked;
     int32 chunksize;
     bool headerreceived;
     BList *Threads;
     BList *queue;
     int32 benval;
     sem_id httpsem;
    public:
     http_protocol();
     ~http_protocol();
     BMessage headers;
     BMallocIO *buffer,*RawBuffer; //parsed and unparsed buffers respectively
     char *PlugName(void);
     uint32 PlugID(void);
     float PlugVersion(void);
     status_t Go(void);
     int32 SpawnThread(BMessage *info);
     bool IsPersistant(){return true;}
     void Stop();
   //  void FindURI(const char *url,BString &host,int &port,BString &uri);
 //    void ParseResponse(unsigned char *resp,size_t size);
  //   BMessage *ProcessRespLine(const char *data);
 //    unsigned char *GetDoc(BString &host,int &port,BString &uri);
 //    unsigned char *GetDoc(const char* url);
 //    unsigned char *GetURL(const char* url);
     int32 GetURL(BMessage *info);
     static int32 ThreadFunc(void *info);
   };
// };
#endif
