#include "http_main.h"
#include "http_defines.h"
#include <stdlib.h>
#include <NetworkKit.h>
#include <malloc.h>
#include <String.h>
#include <File.h>
#include <Path.h>
#include "stripwhite.h"
#include <stdio.h>
#include "commondefs.h"
#include "plugman.h"
//using namespace Themis_Networking;
http_protocol *HTTP;
http_protocol *ME;
status_t Initialize(void *info)
 {
  HTTP=new http_protocol;
  return B_OK;
 }
status_t Shutdown(bool now)
 {
  if (now)
	  if (HTTP!=NULL)
  		delete HTTP;
  return B_OK;
 }
protocol_plugin* GetObject(void)
 {
  if (HTTP==NULL)
   HTTP=new http_protocol();
  return HTTP;
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
  ME=this;
  Cancel=0;
  Window=NULL;
  RawBuffer=new BMallocIO;
  RawBuffer->SetBlockSize(B_PAGE_SIZE);
  buffer=new BMallocIO;
  buffer->SetBlockSize(B_PAGE_SIZE);
 }
http_protocol::~http_protocol()
 {
  delete buffer;
  delete RawBuffer;
  if (InitInfo!=NULL)
    delete InitInfo;
 }
int32 http_protocol::SpawnThread(BMessage *info)
 {
  if (InitInfo!=NULL)
    delete InitInfo;
  InitInfo=new BMessage(*((BMessage *)info));
  thread=spawn_thread(ThreadFunc,"http thread",B_LOW_PRIORITY,InitInfo);
  return thread;
 }
/*
void http_protocol::Config(BMessage *msg)
 {
 }
*/

int32 http_protocol::GetURL(BMessage *info)
 {
  if (info!=NULL)
   {
    printf("http_protocol::GetURL info: %p\n",info);
    info->PrintToStream();
    //BString url;
    //info->FindString("target_url",&url);
    BMessage *data=GetURL_(info);
    printf("data:\n=============================\n");
//    data->PrintToStream();
    plugman *PlugMan;
    info->FindPointer("plug_manager",(void**)&PlugMan);
    PlugClass *cache=(PlugClass*)PlugMan->FindPlugin('cash');
    BMessage reply;
    BMessenger msgr(cache->Handler(),NULL,NULL);
    if (!msgr.IsValid())
     printf("Messenger is bad\n");
    data->what=CreateCacheObject;
    msgr.SendMessage(data,&reply);
//    reply.PrintToStream();
    if (reply.HasRef("ref"))
     {
      entry_ref ref;
      reply.FindRef("ref",&ref);
      uint64 size;
      data->FindInt64("bytes-received",(int64*)&size);
      unsigned char *buf=NULL;//=(unsigned char*)malloc(size);
 //     memset(buf,0,size);
      BFile file(&ref,B_CREATE_FILE|B_ERASE_FILE|B_WRITE_ONLY);
      data->FindData("returneddata",B_RAW_TYPE,(const void **)&buf,(ssize_t*)&size);
      file.Write(buf,size);
      file.Sync();
 //     free(buf);
     }
    delete data;
//    printf("output:\n%s\n**** done***\n",GetURL(url.String()));
   }
  exit_thread(B_OK);
  return 0;
 }
BMessage *http_protocol::GetURL_(BMessage *info)
 {
	uint64 contentlength=0;
	uint64 datareceived=0;
    BString url;
    info->FindString("target_url",&url);
  BMessage *responsemsg=new BMessage;
  BString host,uri;
  int port=0;
  FindURI(url.String(),host,port,uri);
  responsemsg->AddString("url",url);
  responsemsg->AddString("host",host);
  {
   BString nme;
   int32 slash=0;
   if ( (slash=uri.FindLast("/"))>B_ERROR) {
    uri.CopyInto(nme,slash+1,uri.Length()-slash);
    printf("name: %s\n",nme.String());
    responsemsg->AddString("name",nme);
  //  nme="";
 //   uri.CopyInto(nme,0,slash);
   responsemsg->AddString("path",uri);
   }
  }
  headerreceived=false;
  BNetEndpoint *ep=new BNetEndpoint();
  ep->SetNonBlocking(false);
  ep->SetTimeout(30*10*100000);//30 seconds...
  if (ep->InitCheck()!=B_OK)
   {
    BString error="<html><head><title>Internal Error</title></head><body><center><h1>Internal Error</h1></center>An error occurred while trying to initialize the connection:";
    error << ep->Error() << " - "<<ep->ErrorStr() <<"<br></body></html>";
    responsemsg->AddString("returneddata",error.String());
    responsemsg->AddInt64("Content-Length",error.Length());
    return responsemsg;
   }
  BString request;
  request<<"GET "<<uri<< " HTTP/1.1\nUser-Agent: Mozilla/4.0 (compatible; Themis/pre-alpha, BeOS Rules!)\nAccept: text/html";
  {
   int32 count;
   type_code tcode;
   info->GetInfo("supported_types",&tcode,&count);
   BString type;
   for (int32 i=0 ; i<count; i++)
    {
     info->FindString("supported_Types",i,&type);
     request<<" "<<type;
    }
  }
  printf("request string:\n%s\n\n",request.String());
  request<<"\nHost: "<<host.String()<<":"<<port<<"\nConnection: close\n\n";
  ep->Connect(host.String(),port);
  ep->Send(request.String(),request.Length());
  BMallocIO *data=new BMallocIO();
  unsigned char *buf=(unsigned char*) malloc(4096);
  memset(buf,0,4096);
  int32 size;
    char crlf[5];
    crlf[0]=13;
    crlf[1]=10;
    crlf[2]=13;
    crlf[3]=10;
    crlf[4]=0;
   size=0;
  while ((size=ep->Receive(buf,4095))>-1)
   {
//    if (size==0)
//     {
//      continue;
//     }
//   printf("while loop\n");
    if (!headerreceived)
     {
      char *headend=strstr((char*)buf,crlf);//end of header
      if (headend==NULL)
       {
        printf("headend is NULL\n");
        exit(0);
       }
      char *databeg=(headend+4);
     char *curp=NULL;
     int32 endofhead=0;
       for (uint32 i=0;i<size;i++)
        {
         curp=(((char*)buf)+i);
         if (curp==databeg)
          {
           datareceived=size-i;
           break;
          }
         endofhead++;
        }
         headerreceived=true;
         BString headtext;
         headtext.SetTo((char*)buf,endofhead);
         printf("header: %s\n",headtext.String());
         int32 pos=0,last=0;
         BString param,value;
         BString response;
         char lf[3];
         lf[0]=13;
         lf[1]=10;
         lf[2]=0;
         if ((pos=headtext.IFindFirst(lf,0))!=B_ERROR)
          {
           headtext.MoveInto(response,0,pos);
           printf("responseline: %s\n",response.String());
          }
         else
          {
           printf("no response line found...\n");
          }
         headtext.RemoveFirst(lf);
           responsemsg->AddString("responseline",response);
         while((pos=headtext.IFindFirst(":"))!=B_ERROR)
          {
           if (Cancel)
            break;
           headtext.MoveInto(param,0,pos);
           char *t=(char*)malloc(param.Length()+1);
           memset(t,0,param.Length()+1);
           stripfrontwhite(param.String(),t);
           param=t;
           memset(t,0,param.Length()+1);
           stripendwhite(param.String(),t);
           free(t);
           headtext.RemoveFirst(":");
//           printf("%s\n",param.String());
           if ((last=headtext.IFindFirst(lf))!=B_ERROR)
            {
             headtext.RemoveFirst(lf);
             headtext.MoveInto(value,0,last);
             t=(char*)malloc(value.Length()+1);
             memset(t,0,value.Length()+1);
             stripfrontwhite(value.String(),t);
             value=t;
             memset(t,0,value.Length()+1);
             stripendwhite(value.String(),t);
             free(t);
             printf("parameter: %s\tvalue: %s\n",param.String(),value.String());

             if (param.ICompare("content-length")==0)
              {
             	contentlength=atol(value.String());
             	responsemsg->AddInt64(param.String(),contentlength);
              }
            }
           responsemsg->AddString(param.String(),value);
           param="";
           value="";
          }
		printf("header size: %Ld\n",size-datareceived);
		unsigned char *dump=(unsigned char*)malloc(datareceived);
		memset(dump,0,datareceived);
		memmove(dump,databeg,datareceived);
		memset(buf,0,4096);
		memmove(buf,dump,datareceived);
		size-=datareceived;
		free(dump);
      headerreceived=true;
     }
    else
     datareceived+=size;
    data->Write(buf,size);
   
    memset(buf,0,4096);
      printf("data received: %Ld/%Ld\n",datareceived,contentlength);
    if (datareceived>=contentlength)
     {
      break;
     }
   
   }
  free(buf);
  printf("data length: %ld\n",data->BufferLength());
  if (data->BufferLength()>datareceived)
   {
    buf=(unsigned char*)malloc(datareceived);
    memset(buf,0,datareceived);
    data->ReadAt(data->BufferLength()-datareceived,buf,datareceived);
    delete data;
    data=new BMallocIO();
    data->Write(buf,datareceived);
   }
  responsemsg->AddInt64("bytes-received",datareceived);
  responsemsg->AddData("returneddata",B_RAW_TYPE,data->Buffer(),data->BufferLength(),true,1);
  delete data;
  printf("done fetching: %d: %s\n",ep->Error(),ep->ErrorStr());
  ep->Close();
  return responsemsg;
 }
int32 http_protocol::ThreadFunc(void *info)
 {
  return (HTTP->GetURL((BMessage *)info));
 }

unsigned char* http_protocol::GetURL(const char* url)
 {
  if (url==NULL)
   return NULL;
  BString host,uri;
  int port=0;
  FindURI(url,host,port,uri);
  return GetDoc(host,port,uri);
 }

void http_protocol::FindURI(const char *url,BString &host,int &port,BString &uri)
 {
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
    port=atoi(servant.String());
    master.CopyInto(servant,0,colon);
    master=servant;
   }
  else
   port=80;
  host=master.String();
 } 
unsigned char* http_protocol::GetDoc(const char* url)
 {
  BString host,uri;
  int port=0;
  FindURI(url,host,port,uri);
  return (GetDoc(host,port,uri));
 }
unsigned char *http_protocol::GetDoc(BString &host,int &port,BString &uri)
 {
  buffer->Seek(0,SEEK_SET);
  RawBuffer->Seek(0,SEEK_SET);
  RawBuffer->SetSize(0);
  buffer->SetSize(0);
  headerreceived=false;
  buflen=rawbuflen=0;
  BNetAddress addr(host.String(),port);
  BNetEndpoint myep,*rep;
  BString req;
  BString hosts;
  rep=new BNetEndpoint();
  rep->SetNonBlocking(false);
  rep->SetReuseAddr(true);
  if (rep->InitCheck()==B_OK)
   {
    if ((rep->Connect(addr))==B_OK)
     {
    
      BNetAddress myaddr(rep->LocalAddr());
      req<<"GET "<<uri<< " HTTP/1.1\nUser-Agent: Mozilla/4.0 (compatible; Themis/pre-alpha, BeOS Rules!)\nAccept: text/html image/jpeg\n";
      req<<"Host: "<<host.String()<<":"<<port<<"\n\n";
      printf("request string:\n%s\n",req.String());
      BNetBuffer *buf=new BNetBuffer(B_PAGE_SIZE*10);
      rep->Send(req.String(),req.Length(),0);
      size_t size;
      while(rep->Receive(*buf,B_PAGE_SIZE*10)>0)
       {
        size=buf->Size();
//        printf("received %ld bytes\n",size);
        RawBuffer->WriteAt(RawBuffer->Position(),buf->Data(),size);
        ParseResponse(buf->Data(),buf->Size());
        delete buf;
        buf=NULL;
        if (Cancel)
         break;
        buf=new BNetBuffer(B_PAGE_SIZE*10);
       }
      if (buf!=NULL)
       delete buf;
     }
   }
  rep->Close();
  if (rep!=NULL)
   delete rep;
  buflen=buffer->BufferLength();
  rawbuflen=RawBuffer->BufferLength();
  return (unsigned char*)buffer->Buffer();
 }
void http_protocol::ParseResponse(unsigned char *resp,size_t size)
 {
  int32 pos=0;
  BString tmp;
  if (buffer->BufferLength()==0)
   {//gather header info
    char crlf[5];
    crlf[0]=13;
    crlf[1]=10;
    crlf[2]=13;
    crlf[3]=10;
    crlf[4]=0;
     char *headend=strstr((char*)resp,crlf);//end of header
     char *databeg=headend+(4);
     char *curp=NULL;
     int32 endofhead=0;
     if(headend!=NULL)
      {
       printf("headend: %p\ndatabeg: %p\ndata: %s\n",headend,databeg,databeg);
       for (uint32 i=0;i<size;i++)
        {
         curp=(((char*)resp)+i);
         if (curp==databeg)
          {
           break;
          }
         endofhead++;
        }
       buffer->WriteAt(0,databeg,(size-endofhead));
       printf("http: size of buffer: %ld\n",buffer->BufferLength());
       if (endofhead<=size)
        {
         headerreceived=true;
         BString headtext;
         headtext.SetTo((char*)resp,endofhead);
         printf("header: %s\n",headtext.String());
         int32 pos=0,last=0;
         BString param,value;
         BString response;
         char lf[3];
         lf[0]=13;
         lf[1]=10;
         lf[2]=0;
         if ((pos=headtext.IFindFirst(lf,0))!=B_ERROR)
          {
           headtext.MoveInto(response,0,pos);
           printf("responseline: %s\n",response.String());
          }
         else
          {
           printf("no response line found...\n");
          }
         headtext.RemoveFirst(lf);
         while((pos=headtext.IFindFirst(":"))!=B_ERROR)
          {
           if (Cancel)
            break;
           headtext.MoveInto(param,0,pos);
           char *t=(char*)malloc(param.Length()+1);
           memset(t,0,param.Length()+1);
           stripfrontwhite(param.String(),t);
           param=t;
           memset(t,0,param.Length()+1);
           stripendwhite(param.String(),t);
           free(t);
           headtext.RemoveFirst(":");
//           printf("%s\n",param.String());
           if ((last=headtext.IFindFirst(lf))!=B_ERROR)
            {
             headtext.RemoveFirst(lf);
             headtext.MoveInto(value,0,last);
             t=(char*)malloc(value.Length()+1);
             memset(t,0,value.Length()+1);
             stripfrontwhite(value.String(),t);
             value=t;
             memset(t,0,value.Length()+1);
             stripendwhite(value.String(),t);
             free(t);
             printf("parameter: %s\tvalue: %s\n",param.String(),value.String());
            }
           param="";
           value="";
          }
        }
      }
     else
      {
       if (!headerreceived)
        {
         printf("Warning!!! Couldn't find crlfcrlf!\n",(char*)RawBuffer->Buffer());
         BString error="<html><head><title>Server Error!</title></head><body><h1>Error! Webserver returned either invalid header, or no header at all!</h1><br>The server sent:<br><pre>";
         error << (char*)resp << "</pre></br></body></html>";
         buffer->WriteAt(buffer->Position(),error.String(),error.Length());
        }
       else
        {
         buffer->WriteAt(buffer->Position(),resp,size);
        }
      }
   }//gather header info
  else
   {//process incoming data normally
    if (chunked)
     {
     }
    else
     {
      buffer->WriteAt(buffer->Position(),resp,size);
     }
   }//process incoming data normally 
 }
