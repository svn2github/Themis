#include "http_main.h"
#include "http_defines.h"
#include <stdlib.h>
#include <NetworkKit.h>
//using namespace Themis_Networking;
http_protocol *HTTP;
status_t Initialize(bool go)
 {
  HTTP=new http_protocol;
  return B_OK;
 }
status_t Shutdown(bool now)
 {
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
PlugType http_protocol::PluginType(void)
 {
  return ProtocolPlugin;
 }
bool http_protocol::IsPersistant(void)
 {
  return true;
 }
char* http_protocol::GetPluginName(void)
 {
  return PlugName;
 }
int32 http_protocol::GetPluginID(void)
 {
  return PlugID;
 }
float http_protocol::GetPluginVers(void)
 {
  return PlugVers;
 }
status_t http_protocol::Go(void)
 {
  return B_OK;
 }
http_protocol::http_protocol()
              :protocol_plugin()
 {
  RawBuffer=new BMallocIO;
  RawBuffer->SetBlockSize(B_PAGE_SIZE);
  buffer=new BMallocIO;
  buffer->SetBlockSize(B_PAGE_SIZE);
 }
http_protocol::~http_protocol()
 {
  delete buffer;
  delete RawBuffer;
 }
unsigned char* http_protocol::GetURL(const char*url)
 {
  if (url==NULL)
   return NULL;
  BString host,uri;
  int port=0;
  FindURI(url,host,port,uri);
  return GetDoc(host,port,uri);
 }
BMessage *http_protocol::SupportedTypes(void)
 {
  return NULL;
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
        buf=new BNetBuffer(B_PAGE_SIZE*10);
       }
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
    char crlf[3];
    crlf[0]=10;
    crlf[1]=13;
    crlf[2]=10;
     char *headend=strstr((char*)resp,crlf);//end of header
     char *databeg=headend+(3);
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
           headtext.MoveInto(param,0,pos);
           headtext.RemoveFirst(":");
//           printf("%s\n",param.String());
           if ((last=headtext.IFindFirst(lf))!=B_ERROR)
            {
             headtext.RemoveFirst(lf);
             headtext.MoveInto(value,0,last);
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
