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

#include "http_main.h"
#include "http_defines.h"
#include <stdlib.h>
//#include <NetworkKit.h>
#include <sys/socket.h>
#include <NetAddress.h>
#include <malloc.h>
#include <String.h>
#include <errno.h>
#include <File.h>
#include <Path.h>
#include "stripwhite.h"
#include <stdio.h>
#include "commondefs.h"
#include "plugman.h"
#define BUFFER_SIZE 16384
#ifdef BONE_VERSION
#define closesocket close
#endif
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
//  if (now)
	  if (HTTP!=NULL)
	   {
	    HTTP->Stop();
  		delete HTTP;
  	   }
  return B_OK;
 }
 
http_worker::http_worker(BMessage *info)
 {
   Info=info;
   Info->PrintToStream();
   die=0;
   benval=NULL;
   Info->FindInt32("http_sem_id",&httpsem);
   Info->FindInt32("http_proto_port",&http_proto_port);
//   Info->FindPointer("http_th_list",(void**)&list);
   Info->FindPointer("benaphore_counter",(void**)&benval);
   acquire_sem(httpsem);
   atomic_add(benval,1);
   thread=spawn_thread(GetIt,"http_retrieval_thread",B_LOW_PRIORITY,this);
   resume_thread(thread);
 }
http_worker::~http_worker()
 {
  atomic_add(benval,-1);
  release_sem(httpsem);
  delete Info;
  printf("http_worker destructor exiting\n");
 }
int32 http_worker::GetIt(void *target)
 {
  BMessage *info=((http_worker*)target)->GetHead(((http_worker*)target)->Info);

    if (info->HasString("Pragma"))
     {
      BString pragmactl;
      info->FindString("Pragma",&pragmactl);
      if ((pragmactl.ICompare("no-cache")==0) && (!info->HasString("Cache-Control")))
       info->AddString("Cache-Control","no-cache");
     }
    if (info->HasString("Cache-Control"))
     {
      BString cachectl;
      info->FindString("Cache-Control",&cachectl);
      if (cachectl.ICompare("no-cache")==0)
       {
       //pass the data directly back to the app...
       }
      else
       {
        //cache it!
        plugman *PlugMan;
        info->FindPointer("plug_manager",(void**)&PlugMan);
        PlugClass *cache=(PlugClass*)PlugMan->FindPlugin('cash');
        BMessage reply;
        BMessenger msgr(cache->Handler(),NULL,NULL);
        info->what=CreateCacheObject;
        msgr.SendMessage(info,&reply);
        if (reply.HasRef("ref"))
         {
          entry_ref ref;
          reply.FindRef("ref",&ref);
          uint64 size;
          info->AddRef("cacheddata",&ref);
//          data->FindInt64("bytes-received",(int64*)&size);
  //        unsigned char *buf=NULL;
//          BFile file(&ref,B_CREATE_FILE|B_ERASE_FILE|B_WRITE_ONLY);
//          data->FindData("returneddata",B_RAW_TYPE,(const void **)&buf,(ssize_t*)&size);
 //         file.Write(buf,size);
//          file.Sync();
 //         free(buf);
         }
       }
     }
    else
     {
      //nothing about cache control... so cache it!
      plugman *PlugMan;
      info->FindPointer("plug_manager",(void**)&PlugMan);
      PlugClass *cache=(PlugClass*)PlugMan->FindPlugin('cash');
      BMessage reply;
      BMessenger msgr(cache->Handler(),NULL,NULL);
      info->what=CreateCacheObject;
      msgr.SendMessage(info,&reply);
      if (reply.HasRef("ref"))
       {
        entry_ref ref;
        reply.FindRef("ref",&ref);
        info->AddRef("cacheddata",&ref);
        //uint64 size;
        //data->FindInt64("bytes-received",(int64*)&size);
  //      unsigned char *buf=NULL;//=(unsigned char*)malloc(size);
   //     BFile file(&ref,B_CREATE_FILE|B_ERASE_FILE|B_WRITE_ONLY);
   //     data->FindData("returneddata",B_RAW_TYPE,(const void **)&buf,(ssize_t*)&size);
   //     file.Write(buf,size);
    //    file.Sync();
    //    free(buf);
     }
     }
  //  delete data;

  if (!((http_worker*)target)->die)
   info=((http_worker*)target)->GetURL(info);
  BMessage *msg=new BMessage(ThreadDone);
  msg->AddPointer("which",target);
  ssize_t size=msg->FlattenedSize();
  char *buf=new char[size+1];
  memset(buf,0,size+1);
  msg->Flatten(buf,size);
  write_port(((http_worker*)target)->http_proto_port,ThreadDone,buf,size);
  delete buf;
  delete msg;
//  ((http_worker*)target)->list->RemoveItem((http_worker*)target);
//  delete (http_worker*)target;
printf("http_worker about to exit %p\n",(http_worker*)target);
  exit_thread(0);
  return 0;
 }
void http_worker::FindURI(const char *url,BString &host,uint16 &port,BString &uri)
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

BMessage *http_worker::ProcessRespLine(const char *data)
 {

  BMessage *response=new BMessage;
  char *str=(char*)malloc(strlen(data)+1);
  memset(str,0,strlen(data)+1);
  strcpy(str,data);
  char *parts[4];
  parts[0]=strtok(str," /");
  if (parts[0]==NULL)
   {
    printf("%p: um... parts[0] is null...\n",this);
    for (int32 i=0; i<strlen(data);i++)
     printf("%p - %ld: 0x%x - %c\n",this,i,data[i],data[i]);
    exit(0);
   }
//  printf("header response line part 0: %s\n",parts[0]);
  for (int i=1; i<4; i++)
   {
   parts[i]=NULL;
   parts[i]=strtok(NULL," /");
//   printf("header response line part %d: %s\n",i,parts[i]);
   }
//   printf("%s\n",parts[0]);
  if (parts[1]!=NULL){
   response->AddString("http_level",parts[1]);
   }
   else
    {
    printf("parts[1] is null!\n");
    for (int32 i=0; i<strlen(parts[1]);i++)
     printf("%p - %ld: 0x%x - %c\n",this,i,parts[1][i],parts[1][i]);
    exit(0);
   }
  if (parts[2]!=NULL){
  response->AddString("http_response",parts[2]);
 //  printf("%s - %d\n",parts[2],atoi(parts[2]));
  response->AddInt16("http_response",atoi(parts[2]));
  response->what=atoi(parts[2]);
  }
   else
    {
    printf("parts[2] is null!\n");
    for (int32 i=0; i<strlen(parts[2]);i++)
     printf("%p - %ld: 0x%x - %c\n",this,i,parts[2][i],parts[2][i]);
    exit(0);
   }
  if (parts[3]!=NULL){
 //  printf("%s\n",parts[3]);
  response->AddString("http_response_txt",parts[3]);
  }
   else
    {
    printf("parts[3] is null!\n");
    for (int32 i=0; i<strlen(parts[3]);i++)
     printf("%p - %ld: 0x%x - %c\n",this,i,parts[3][i],parts[3][i]);
    fflush(stdout);
    exit(0);
   }
  response->PrintToStream();
  free(str);
  return response;
  }

BMessage *http_worker::GetHead(BMessage *info,int32 use_sock)
	{
	struct sockaddr_in servaddr;
	BMessage *responsemsg=new BMessage(*info);
	bool headerreceived=false;
//	BNetEndpoint *ep;
	BString host;
	uint16 port=0;
	uint64 contentlength;
	uint64 datareceived;
	BString uri;
	if (use_sock<=-1)
		{
		//  	ep=new BNetEndpoint();
		sock=socket(AF_INET,SOCK_STREAM,0);
		BString url;
		info->FindString("target_url",&url);
		FindURI(url.String(),host,port,uri);
		responsemsg->AddString("url",url);
		responsemsg->AddString("host",host);
		responsemsg->AddInt16("port",port);
		responsemsg->AddString("uri",uri);
			{
			BString nme;
			int32 slash=0;
			if ( (slash=uri.FindLast("/"))>B_ERROR)
				{
				uri.CopyInto(nme,slash+1,uri.Length()-slash);
				printf("name: %s\n",nme.String());
				responsemsg->AddString("name",nme);
				responsemsg->AddString("path",uri);
				}
			}
		responsemsg->AddInt32("socket",sock);
		}
		else
		{
		responsemsg->AddInt32("socket",sock);
		sock=use_sock;
		}
	int32 zero=0;
//	setsockopt(sock,SOL_SOCKET,SO_NONBLOCK,&zero,sizeof(int32));
	BString request;
	request<<"HEAD "<<uri<< " HTTP/1.1\nUser-Agent: Mozilla/4.0 (compatible; Themis/pre-alpha, BeOS Rules!)\nAccept: text/html";
		{
		int32 count;
		type_code tcode;
		info->GetInfo("supported_types",&tcode,&count);
		BString type;
		for (int32 i=0 ; i<count; i++)
			{
			info->FindString("supported_types",i,&type);
			responsemsg->AddString("supported_types",type.String());
			request<<" "<<type;
			}
		}
	request<<"\nHost: "<<host.String()<<":"<<port<<"\nConnection: keep-alive\n\n";
	printf("request string:\n%s\n\n",request.String());
		{
		BNetAddress addr(host.String(),port);
		addr.GetAddr(servaddr);
		printf("**socket: %ld\n",sock);
		if (connect(sock,(sockaddr *)&servaddr,sizeof(servaddr))<0)
			{
			switch(errno)
				{//error catching needs to be fully implemented
				case EWOULDBLOCK:
					{
					 printf("");
					}break;
				case ECONNREFUSED:
					{
					 printf("");
					}break;
				case ENETUNREACH:
					{
					 printf("");
					}break;
				case EBADF:
					{
					 printf("");
					}break;
				case EISCONN:
					{
					 printf("");
					}break;
				default:
					{
					 
					}
				}
			//  	printf("Error opening connection for header request. \"%s\"\n",ErrorStr());
			delete responsemsg;
			responsemsg=new BMessage(ThreadDone);
			responsemsg->AddPointer("which",this);
			ssize_t size=responsemsg->FlattenedSize();
			char *buf=new char[size+1];
			memset(buf,0,size+1);
			responsemsg->Flatten(buf,size);
			write_port(http_proto_port,ThreadDone,buf,size);
			delete buf;
			delete responsemsg;
			exit_thread(errno);
			}
		}
	send(sock,request.String(),request.Length(),0);
	unsigned char *buf=(unsigned char*) malloc(BUFFER_SIZE);
	memset(buf,0,BUFFER_SIZE);
	int32 size;
	char crlf[5];
	crlf[0]=13;
	crlf[1]=10;
	crlf[2]=13;
	crlf[3]=10;
	crlf[4]=0;
	size=0;
	while ((size=recv(sock,buf,BUFFER_SIZE-1,0))>-1)
		{
		if (die)
		 break;
		if (!headerreceived)
		{
		char *headend=strstr((char*)buf,crlf);//end of header
		char *databeg=(headend+4);
		char *curp=NULL;
		int32 endofhead=0;
		if (headend==NULL)
		 {
		  printf("headend is NULL:\n%s\n",(char*)buf);
		  for (int16 i=0; i<size; i++)
		   {
		//         fprintf(stdout,"buf[%d]: 0x%x - %c\n",i,*(buf+i),*(buf+i));
		   if ((*(buf+i))=='\0')//NULL)
		    {
		     printf("NULL found... assuming that's the end of the header!\n");
		     headend=(char*)buf+(i-2);
		     endofhead=i;
		     break;
		    }
		   }
		 }
		 if (endofhead>0)
		  goto ProcHead;
		 for (int32 i=0;i<size;i++)
		  {
		   curp=(((char*)buf)+i);
		   if (curp==databeg)
		    {
		     datareceived=size-i;
		     break;
		    }
		   endofhead++;
		  }
		   ProcHead:
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
		     BMessage *statusmsg=ProcessRespLine(response.String());
		     switch((statusmsg->what/100))
		      {
		       case 1:
		        {
		         printf("response was 1xx class, continuing loop.\n");
		         memset(buf,0,BUFFER_SIZE);
		         headerreceived=false;
		         continue;
		        }
		       case 2:
		        break;
		       case 3:
		        break;
		       case 4:
		        break;
		       case 5:
		        break;
		       default:
		        {
		         printf("unknown http response code.\n");
		        }
		      }
		     BString httpver;
		     statusmsg->FindString("http_level",&httpver);
		     responsemsg->AddString("http_level",httpver);
		     delete statusmsg;
		    }
		   else
		    {
		     printf("no response line found...\n");
		    }
		   headtext.RemoveFirst(lf);
		     responsemsg->AddString("responseline",response);
		   while((pos=headtext.IFindFirst(":"))!=B_ERROR)
		    {
		     if (die)
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
					param.String();
					param=param.ToLower();
					responsemsg->AddInt64(param.String(),contentlength);
					}
				}
		     param=param.ToLower();
		     responsemsg->AddString(param.String(),value);
		     param="";
		     value="";
		    }
		headerreceived=true;
		}
		if (headerreceived)
		break;
		}
//	closesocket(sock);
	free(buf);
	responsemsg->AddBool("got_header",headerreceived);
	return responsemsg;
}

BMessage *http_worker::GetURL(BMessage *info)
{
	struct sockaddr_in servaddr;
	uint64 contentlength=0;
	info->FindInt64("content-length",(int64*)&contentlength);
	uint64 datareceived=0;
	BString url;
	info->FindString("target_url",&url);
	//  BMessage *responsemsg=new BMessage;
	BString host,uri;
	uint16 port=0;
	info->FindString("url",&url);
	info->FindString("host",&host);
	info->FindInt16("port",(int16*)&port);
	info->FindString("uri",&uri);
	BString httpver;
	info->FindString("http_level",&httpver);
	if (httpver.ICompare("1.0")==0)
		{
		closesocket(sock);
		sock=socket(AF_INET,SOCK_STREAM,0);
		BNetAddress addr(host.String(),port);
		addr.GetAddr(servaddr);
		connect(sock,(sockaddr *)&servaddr,sizeof(servaddr));
		}
	int32 zero=0;
	bool headerreceived=false;
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
	Get_SendRequest:
	if(send(sock,request.String(),request.Length(),0)<0)
		{
		printf("error! %ld\n",errno);
		closesocket(sock);
		switch(errno)
			{//error catching needs to be fully implemented
			case ENOTCONN:
				{
				printf("ENOTCONN\n");
				sock=socket(AF_INET,SOCK_STREAM,0);
				connect(sock,(sockaddr *)&servaddr,sizeof(servaddr));
				goto Get_SendRequest;
				}break;
			case ECONNRESET:
				{
				printf("ECONNRESET\n");
				sock=socket(AF_INET,SOCK_STREAM,0);
				connect(sock,(sockaddr *)&servaddr,sizeof(servaddr));
				goto Get_SendRequest;
				}break;
			case EWOULDBLOCK:
				{
				printf("EWOULDBLOCK\n");
				}break;
			case ECONNREFUSED:
				{
				printf("ECONNREFUSED\n");
				}break;
			case ENETUNREACH:
				{
				printf("ENETUNREACH\n");
				}break;
			case EBADF:
				{
				printf("EBADF\n");
				}break;
			case EISCONN:
				{
				printf("EISCONN\n");
				}break;
			default:
				{
				printf("Some other socket error\n");
				sock=socket(AF_INET,SOCK_STREAM,0);
				connect(sock,(sockaddr *)&servaddr,sizeof(servaddr));
				goto Get_SendRequest;
				}
			}
		}
	BFile *file=NULL;
	if (info->HasRef("cacheddata"))
		{
		entry_ref ref;
		info->FindRef("cacheddata",&ref);
		file=new BFile(&ref,B_WRITE_ONLY|B_ERASE_FILE);
		}
	unsigned char *buf=(unsigned char*) malloc(BUFFER_SIZE);
	memset(buf,0,BUFFER_SIZE);
	int32 size;
	char crlf[5];
	crlf[0]=0xd;
	crlf[1]=0xa;
	crlf[2]=0xd;
	crlf[3]=0xa;
	crlf[4]=0;
	size=0;
	printf("socket: %ld\n",sock);
	while ((size=recv(sock,buf,BUFFER_SIZE-1,0))>-1)
		{
		if (die)
			break;
		printf("bytes received: %ld\n",size);
		if (!headerreceived)
			{
			char *headend=NULL;
			bool nullmethod=false;
			unsigned char *databeg=NULL;
			int32 endofhead=0;
			if (headend==NULL)
				{
				headend=strstr((char*)buf,crlf);//end of header
				if (headend!=NULL)
					{
					printf("normal method!\n");
					for (int16 i=0; i<size; i++)
						{
						if ((buf+i)==((unsigned char*)headend))
							{
							endofhead=i;
							databeg=(unsigned char*)headend+4;
							printf("data:\n%s\n",(char*)databeg);
							datareceived=size-i;
							break;
							}
						}
					if (databeg!=NULL)
					goto ProcHead;
					}
				}
			if (headend==NULL)	 
				{
				char crcr[3];
				crcr[0]=0xa;
				crcr[1]=0xa;
				crcr[2]=0x0;
				headend=strstr((char*)buf,crcr);
				if (headend!=NULL)
					{
					for (int16 i=0; i<size;i++)
						{
						if ((buf+i)==((unsigned char*)headend))
							{
							databeg=(unsigned char*)headend+2;
							endofhead=i;
							datareceived=size-i;
							printf("crcr method!\n");
							break;
							}
						}
					 if (databeg!=NULL)
					  goto ProcHead;
					}
				}
			if (headend==NULL)
				{
				for (int16 i=0; i<size; i++)
					{
					if ((*(buf+i))==0x0)
						{
						datareceived=size-i;
						databeg=NULL;
						endofhead=i;
						printf("null method!!\n");
						break;
						}
					}
				}
			ProcHead:
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
				}
			headtext.RemoveFirst(lf);
			while((pos=headtext.IFindFirst(":"))!=B_ERROR)
				{
				if (die)
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
						}
					}
				}
			printf("header size: %Ld\n",size-datareceived);
			headerreceived=true;
			if (size==endofhead)
				{
				memset(buf,0,BUFFER_SIZE);
				continue;
				}
			memset(buf,0,endofhead);//lets make sure the header is gone...
			unsigned char *spotter=buf;
			int xdiff=0;
			if (databeg!=NULL)
				{
				xdiff=databeg-(unsigned char*)headend;
				//oooh... that's nasty, but it works. pointer math==scary
				}
			for (int16 i=0;i<size;i++)
			 if (*(buf+i)!=0x0)
			  {
			   if (databeg!=NULL)
				   spotter=buf+i+xdiff;
			   else
				   spotter=buf+i;
			   break;
			  }
			datareceived=size-(xdiff+endofhead);
			//Shall we dance the Memory Shuffle?
			unsigned char *dump=(unsigned char*)malloc(datareceived);
			memset(dump,0,datareceived);
			printf("data length: %ld\n",strlen((char*)(buf+(size-endofhead))));
			memmove(dump,spotter,datareceived);
			memset(buf,0,BUFFER_SIZE);
			memmove(buf,dump,datareceived);
			size-=(endofhead+xdiff);
			free(dump);
			if (nullmethod)
				{
				memset(buf,0,BUFFER_SIZE);
				continue;
				}
			}
		else
			datareceived+=size;
		if (file)
			file->Write(buf,size);
		memset(buf,0,BUFFER_SIZE);
		printf("data received: %Ld/%Ld\n",datareceived,contentlength);
		if (datareceived>=contentlength)
			{
			break;
			}
		}
	free(buf);
	off_t fsize=0L;
	if (file)
		{
		file->GetSize(&fsize);
		printf("file length: %Ld\n",fsize);
		}
	if (file)
		delete file;
	info->AddInt64("bytes-received",datareceived);
	printf("done fetching\n");
	info->PrintToStream();
	if ((!Info->HasBool("returnsocket")) && (!Info->HasBool("stream")))
		closesocket(sock);
	return info;
}

thread_id http_worker::Thread()
 {
 return thread;
 }

bool http_worker::Quit()
 {
  atomic_add(&die,1);
  return (bool)die;
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
  httpsem=create_sem(5,"http_thread_sem");
  benval=0;
  Cancel=0;
  Window=NULL;
  thread=-1;
  Threads=new BList(1);
  queue=new BList(1);

  http_proto_port=0;
 }
http_protocol::~http_protocol()
 {
  printf("http_protocol destructor\n");
  if (thread>-1)
   {
  Stop();
  status_t stat;
  wait_for_thread(thread,&stat);
  printf("http thread has stopped\n");
   }
  delete_sem(httpsem);
  if (!Threads->IsEmpty())
   {
    http_worker *cur;
    status_t stat;
    while (!Threads->IsEmpty())
     {
      cur=(http_worker*)Threads->RemoveItem((int32)0);
      if (cur!=NULL)
       {
        printf("killing a http thread\n");
        cur->Quit();
        wait_for_thread(cur->Thread(),&stat);
        delete cur;
       }
     }
   }
  delete Threads;
  if (!queue->IsEmpty())
   {
    BMessage *msg;
    while(!queue->IsEmpty())
     {
      msg=(BMessage*)queue->RemoveItem((int32)0);
      if (msg!=NULL)
       {
        delete msg;
        msg=NULL;
       }
     }
   }
  delete queue;
  InitInfo=NULL;
 }
int32 http_protocol::SpawnThread(BMessage *info)
 {
  Stop();
  InitInfo=new BMessage(*((BMessage *)info));
  thread=spawn_thread(ThreadFunc,"http thread",B_LOW_PRIORITY,InitInfo);
  return thread;
 }
void http_protocol::Stop()
 {
  close_port(http_proto_port);
  atomic_add(&Cancel,1);
  if (thread>-1)
   {
    status_t stat;
    wait_for_thread(thread,&stat);
    delete_port(http_proto_port);
   }
  if (!Threads->IsEmpty())
   {
    http_worker *item;
    for (int32 i=0; i<Threads->CountItems();i++)
     {
      item=(http_worker*)Threads->ItemAt(i);
      if (item!=NULL)
       item->Quit();
     }
   }
 }
int32 http_protocol::GetURL(BMessage *info)
 {
  uint32 lastupdatetime,currenttime;
  int32 portcount;
  currenttime=lastupdatetime=real_time_clock();
  http_proto_port=create_port(80,"http_protocol_port");
  {
   ssize_t flatsize=info->FlattenedSize();
   char *infoflat=new char[flatsize+1];
   memset(infoflat,0,flatsize+1);
   info->Flatten(infoflat,flatsize);
   write_port(http_proto_port,FetchItem,infoflat,flatsize);
   delete infoflat;
  }
  delete info;
  info=NULL;
  while ((currenttime-lastupdatetime)<10)
   {
    if ((portcount=port_count(http_proto_port))>0)
     {
      lastupdatetime=real_time_clock();
		printf("thread space available.\n");
		ssize_t size=port_buffer_size(http_proto_port);
		char *flatmsg=new char[size+1];
		int32 msgcode;
		memset(flatmsg,0,size+1);
		printf("reading message from port\n");
      read_port_etc(http_proto_port,&msgcode,flatmsg,size,B_TIMEOUT,5000000);
		BMessage *msg=NULL;
		if (size>0)
			{
			msg=new BMessage;
			msg->Unflatten(flatmsg);
			msg->PrintToStream();
			}
		printf("deleting flattened message\n");
		delete flatmsg;
		portcount--;
		switch(msgcode)
			{
			case FetchItem:
				{
				if (benval<5)
					{
					msg->AddInt32("http_sem_id",httpsem);
					msg->AddInt32("http_proto_port",http_proto_port);
					msg->AddPointer("benaphore_counter",(void**)&benval);
					if (queue->IsEmpty())
						{
						printf("spawning http_worker\n");
						Threads->AddItem(new http_worker(msg));
						}
					else
						{
						queue->AddItem(msg);
						}
					}
				else
					{
					msg->AddInt32("http_sem_id",httpsem);
					msg->AddInt32("http_proto_port",http_proto_port);
					msg->AddPointer("benaphore_counter",(void**)&benval);
					queue->AddItem(msg);
					}
				}break;
			case ThreadDone:
				{
				printf("http thread done message\n");
				http_worker *which;
				msg->FindPointer("which",(void**)&which);
				Threads->RemoveItem(which);
				printf("deleting http_worker object %p\n",which);
				delete which;
				delete msg;
				printf("done.\n");
				if (benval<5)
					{
					if (!queue->IsEmpty())
						{
						msg=(BMessage*)queue->RemoveItem((int32)0);
						Threads->AddItem(new http_worker(msg));
						}
					}
				}break;
			default:
				{
				if (msg!=NULL)
					{
					msg->PrintToStream();
					delete msg;
					}
				}break;
			}
     }
    else
     {
      if (!queue->IsEmpty())
       {
      				if (benval<5)
					{
					 BMessage *msg=(BMessage*)queue->RemoveItem((int32)0);
					 if (msg!=NULL)
					  {
					msg->AddInt32("http_sem_id",httpsem);
					msg->AddInt32("http_proto_port",http_proto_port);
					msg->AddPointer("benaphore_counter",(void**)&benval);
						printf("spawning http_worker\n");
						Threads->AddItem(new http_worker(msg));
					 }
					}
	   lastupdatetime=real_time_clock();
      }
     }
    currenttime=real_time_clock();
    if (Cancel==1)
     {
      printf("http thread quit ordered\n");
      break;
     }
    else
      snooze(50000);
   }
   printf("http thread stopping\n");
   thread=-1;
  close_port(http_proto_port);
  delete_port(http_proto_port);
  exit_thread(B_OK);
  return 0;
 }
int32 http_protocol::ThreadFunc(void *info)
 {
  return (HTTP->GetURL((BMessage *)info));
 }
