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
#include "cacheman.h"
#include "../../common/commondefs.h"
#include "cache_defines.h"
#include <stdio.h>
#include <kernel/fs_index.h>
#include <kernel/OS.h>
#include <string.h>
#include <String.h>
#include <AppKit.h>
#include <Message.h>
#include <Messenger.h>
#include <MessageRunner.h>
#include <Statable.h>
#include <File.h>
#include <fs_attr.h>
#include <Node.h>
#include <Path.h>
cacheman::cacheman(BMessage *info)
         :BHandler("cache_manager"),
          PlugClass(info)
 {
  if (FindCacheDir()==B_OK)
   printf("Found or created the cache directory.\n");
  printf("cache path: %s\n",cachepath.Path());
  CheckIndices();
  CheckMIME();
  BPath trashpath;
  if (find_directory(B_TRASH_DIRECTORY,&trashpath)==B_OK) {
  	trashdir=new BDirectory(trashpath.Path());
  } else {
  	trashdir=new BDirectory("/boot/home/Desktop/Trash");
  }
 }
bool cacheman::IsHandler()
 {
  return true;
 }
BHandler *cacheman::Handler()
 {
  return this;
 }
cacheman::~cacheman()
 {
 	if (trashdir!=NULL)
 		delete trashdir;
 }
int32 cacheman::Type() 
{
	return DISK_CACHE;
}
status_t cacheman::ReceiveBroadcast(BMessage *msg)
{
	MessageReceived(msg);
	return PLUG_HANDLE_GOOD;
	
}

void cacheman::MessageReceived(BMessage *mmsg)
 {
// 	mmsg->PrintToStream();
  switch(mmsg->what)
   {
   	case LoadingNewPage: {
   		//this clears the files marked true in Themis:clearonnew
   	printf("cache::MessageReceived Loading New Page\n");
      BEntry ent(cachepath.Path(),true);
      uint32 count=0;
      struct stat devstat;
      ent.GetStat(&devstat);
      BVolume vol(devstat.st_dev);
      BQuery query;
      query.SetVolume(&vol);
      query.PushAttr("BEOS:TYPE");
      query.PushString(ThemisCacheMIME);
      query.PushOp(B_EQ);
      query.PushAttr("Themis:clearonnew");
      query.PushInt32(1);
      query.PushOp(B_EQ);
      query.PushOp(B_AND);
      int32 plen=query.PredicateLength();
      char *pc=new char [plen+1];
      memset(pc,0,plen+1);
      query.GetPredicate(pc,plen);
      printf("predicate: %s\n",pc);
      delete pc;
      query.Fetch();
      ent.Unset();
      char fname[255];
      while (query.GetNextEntry(&ent,false)!=B_ENTRY_NOT_FOUND) {
      	count++;
      	printf("count %lu\t",count);
      	ent.GetName(fname);
      	printf("name: %s\n",fname);
//      	ent.Remove();
      	ent.Unset();
      }
      printf("\n%lu cached file(s) removed due to new URL entered by user.\n",count);
   	}break;
    case FindCachedObject:
     {
      BEntry ent(cachepath.Path(),true);
      struct stat devstat;
      ent.GetStat(&devstat);
      BVolume vol(devstat.st_dev);
      BQuery query;
      query.SetVolume(&vol);
      query.PushAttr("Themis:URL");
      BString url;
      mmsg->FindString("url",&url);
      query.PushString(url.String());
      query.PushOp(B_EQ);
      query.Fetch();
      ent.Unset();
      entry_ref ref;
      BNode node;
      BMessage reply(CacheObjectNotFound);
      struct attr_info ai;
      char attname[B_ATTR_NAME_LENGTH+1];
        memset(attname,0,B_ATTR_NAME_LENGTH+1);
      unsigned char *data=NULL;
      bool found=false;
      bool set=false;
      BString fname;//field name
      while (query.GetNextEntry(&ent,false)!=B_ENTRY_NOT_FOUND)
       {
       	if (trashdir->Contains(&ent))
       		continue;
       	if (!set) {
       		set=true;
       		found=true;
       		reply.what=CachedObject;
       	}
        ent.GetRef(&ref);
        reply.AddRef("ref",&ref);
        ent.Unset();
        node.SetTo(&ref);
        node.Lock();
        while (node.GetNextAttrName(attname)!=B_ENTRY_NOT_FOUND)
         {
          node.GetAttrInfo(attname,&ai);
          if (data!=NULL)
           delete data;
          data=new unsigned char[ai.size+1];
          memset(data,0,ai.size+1);
          switch(ai.type)
           {
            case B_INT64_TYPE:
             {
              if (strcasecmp(attname,"Themis:content-length")==0)
               {
                off_t clen=0;
                node.ReadAttr("Themis:content-length",B_INT64_TYPE,0,&clen,sizeof(clen));
                reply.AddInt64("content-length",clen);
                continue;
               }
             }break;
            case B_STRING_TYPE:
             {
               if (strcasecmp(attname,"Themis:URL")==0)
                {
                 node.ReadAttr("Themis:URL",B_STRING_TYPE,0,data,ai.size);
                 fname="url";
                }
               if (strcasecmp(attname,"Themis:name")==0)
                {
                 node.ReadAttr("Themis:name",B_STRING_TYPE,0,data,ai.size);
                 fname="name";
                }
               if (strcasecmp(attname,"Themis:host")==0)
                {
                 node.ReadAttr("Themis:host",B_STRING_TYPE,0,data,ai.size);
                 fname="host";
                }
               if (strcasecmp(attname,"Themis:mime_type")==0)
                {
                 node.ReadAttr("Themis:mime_type",B_STRING_TYPE,0,data,ai.size);
                 fname="mime_type";
                }
               if (strcasecmp(attname,"Themis:path")==0)
                {
                 node.ReadAttr("Themis:path",B_STRING_TYPE,0,data,ai.size);
                 fname="path";
                }
               if (strcasecmp(attname,"Themis:etag")==0)
                {
                 node.ReadAttr("Themis:etag",B_STRING_TYPE,0,data,ai.size);
                 fname="etag";
                }
               if (strcasecmp(attname,"Themis:last-modified")==0)
                {
                 node.ReadAttr("Themis:last-modified",B_STRING_TYPE,0,data,ai.size);
                 fname="last-modified";
                }
               if (strcasecmp(attname,"Themis:expires")==0)
                {
                 node.ReadAttr("Themis:expires",B_STRING_TYPE,0,data,ai.size);
                 fname="expires";
                }
               if (strcasecmp(attname,"Themis:content-md5")==0)
                {
                 node.ReadAttr(attname,B_STRING_TYPE,0,data,ai.size);
                 fname="content-md5";
                }
                 reply.AddString(fname.String(),(char*)data);
                 fname="";
                 continue;
             }break;
            case B_INT32_TYPE:
             {
             }break;
           }
          delete data;
          data=NULL;
         }
        node.Unlock();
        node.Unset();
        //fix this later so that it should get more specific before returning
       }
       mmsg->SendReply(&reply);
     }break;
    case CreateCacheObject:
     {
      BMessage *msg=new BMessage(*mmsg);
      printf("CreateCacheObject\n");
      BMessage reply(B_ERROR);
      type_code type;
      int32 count,index;
     // char name[B_OS_NAME_LENGTH];
      char *name;
      if (msg->CountNames(B_ANY_TYPE)>=1)
       {
        printf("msg contains items\n");
        bigtime_t reqtime=real_time_clock_usecs();
        BPath pth(cachepath);
        BString fname;
        fname <<reqtime;
        pth.Append(fname.String());
        entry_ref ref;
         {
          BEntry ent(pth.Path(),true);
          ent.GetRef(&ref);
         }
        BFile *file=new BFile(&ref,B_CREATE_FILE|B_ERASE_FILE|B_READ_WRITE);
        BNode *node=new BNode(&ref);
        node->Lock();
        BNodeInfo *ni=new BNodeInfo(node);
        ni->SetType(ThemisCacheMIME);
        delete ni;
        printf("cache: About to loop...\n");
 //       msg->PrintToStream();
 		//memset(name,0,B_OS_NAME_LENGTH);
#if (B_BEOS_VERSION > 0x0504)
        for (index=0;msg->GetInfo(B_ANY_TYPE,index,(const char**)&name,&type,&count)==B_OK; index++)
#else
        for (index=0;msg->GetInfo(B_ANY_TYPE,index,&name,&type,&count)==B_OK; index++)
#endif
         {
          printf("cache - message: %s %ld %ld\n",name,type,count);
          for (int i=0;i<count;i++)
           switch(type)
            {
			 case B_INT64_TYPE:
			  {
               if (strcasecmp(name,"content-length")==0)
                {
				 off_t clen;
                 msg->FindInt64(name,&clen);
                 printf("writing %s: %Ld\n",name,clen);
                 node->WriteAttr("Themis:content-length",B_INT64_TYPE,0,&clen,sizeof(clen));
                 continue;
                }
				  
			  }break;
             case B_INT32_TYPE:
              {
               if (strcasecmp(name,"what")==0)
                continue;
               if (strcasecmp(name,"when")==0)
                continue;
              }break;
             case B_STRING_TYPE:
              {
               if (strcasecmp(name,"url")==0)
                {
                 msg->FindString(name,&fname);//let's reuse stuff! :)
                 printf("writing %s: %s\n",name,fname.String());
                 node->WriteAttr("Themis:URL",B_STRING_TYPE,0,fname.String(),fname.Length()+1);
                 continue;
                }
               if (strcasecmp(name,"name")==0)
                {
                 msg->FindString(name,&fname);
                 printf("writing %s: %s\n",name,fname.String());
                 node->WriteAttr("Themis:name",B_STRING_TYPE,0,fname.String(),fname.Length()+1);
                 continue;
                }
               if (strcasecmp(name,"host")==0)
                {
                 msg->FindString(name,&fname);
                 printf("writing %s: %s\n",name,fname.String());
                 node->WriteAttr("Themis:host",B_STRING_TYPE,0,fname.String(),fname.Length()+1);
                 continue;
                }
               if (strcasecmp(name,"content-type")==0)
                {
                 msg->FindString(name,&fname);
                 printf("writing %s: %s\n",name,fname.String());
                 node->WriteAttr("Themis:mime_type",B_STRING_TYPE,0,fname.String(),fname.Length()+1);
                 continue;
                }
               if (strcasecmp(name,"path")==0)
                {
                 msg->FindString(name,&fname);
                 printf("writing %s: %s\n",name,fname.String());
                 node->WriteAttr("Themis:path",B_STRING_TYPE,0,fname.String(),fname.Length()+1);
                 continue;
                }
               if (strcasecmp(name,"etag")==0)
                {
                 msg->FindString(name,&fname);
                 printf("writing %s: %s\n",name,fname.String());
                 node->WriteAttr("Themis:etag",B_STRING_TYPE,0,fname.String(),fname.Length()+1);
                 continue;
                }
               if (strcasecmp(name,"last-modified")==0)
                {
                 msg->FindString(name,&fname);
                 printf("writing %s: %s\n",name,fname.String());
                 node->WriteAttr("Themis:last-modified",B_STRING_TYPE,0,fname.String(),fname.Length()+1);
                 continue;
                }
               if (strcasecmp(name,"expires")==0)
                {
                 msg->FindString(name,&fname);
                 printf("writing %s: %s\n",name,fname.String());
                 node->WriteAttr("Themis:expires",B_STRING_TYPE,0,fname.String(),fname.Length()+1);
                 continue;
                }
               if (strcasecmp(name,"content-md5")==0)
                {
                 msg->FindString(name,&fname);
                 printf("writing %s: %s\n",name,fname.String());
                 node->WriteAttr("Themis:content-md5",B_STRING_TYPE,0,fname.String(),fname.Length()+1);
                 continue;
                }
              }break;
            }
          // memset(name,0,B_OS_NAME_LENGTH); 
         }
        node->Unlock();
        node->Sync();
        delete node;
        delete file;
        reply.what=B_OK;
        reply.AddRef("ref",&ref);
       }
      delete msg;
      mmsg->SendReply(&reply);
     }break;
    case UpdateCachedObject:
     {
      type_code type;
      int32 count=0,index=0;
     	BMessage *msg=new BMessage(*mmsg);
     	entry_ref ref;
     	msg->FindRef("ref",&ref);
     	BFile *file=new BFile(&ref,B_READ_WRITE);
     	file->SetSize(0);
     	delete file;
        BNode *node=new BNode(&ref);
        node->Lock();
        BNodeInfo *ni=new BNodeInfo(node);
        ni->SetType(ThemisCacheMIME);
        delete ni;
        char *name;
        BMessage reply(*msg);
        printf("cache: About to loop...\n");
 //       msg->PrintToStream();
 		memset(name,0,B_OS_NAME_LENGTH);
 		BString fname;
#if (B_BEOS_VERSION > 0x0504)
        for (index=0;msg->GetInfo(B_ANY_TYPE,index,(const char**)&name,&type,&count)==B_OK; index++)
#else
        for (index=0;msg->GetInfo(B_ANY_TYPE,index,&name,&type,&count)==B_OK; index++)
#endif
         {
          printf("cache - message: %s %ld %ld\n",name,type,count);
          for (int i=0;i<count;i++)
           switch(type)
            {
			 case B_INT64_TYPE:
			  {
               if (strcasecmp(name,"content-length")==0)
                {
				 off_t clen;
                 msg->FindInt64(name,&clen);
                 printf("writing %s: %Ld\n",name,clen);
                 node->RemoveAttr("Themis:content-length");
                 node->WriteAttr("Themis:content-length",B_INT64_TYPE,0,&clen,sizeof(clen));
                 continue;
                }
				  
			  }break;
             case B_INT32_TYPE:
              {
               if (strcasecmp(name,"what")==0)
                continue;
               if (strcasecmp(name,"when")==0)
                continue;
              }break;
             case B_STRING_TYPE:
              {
               if (strcasecmp(name,"url")==0)
                {
                 msg->FindString(name,&fname);//let's reuse stuff! 
                 printf("writing %s: %s\n",name,fname.String());
                 node->RemoveAttr("Themis:URL");
                 node->WriteAttr("Themis:URL",B_STRING_TYPE,0,fname.String(),fname.Length()+1);
                 continue;
                }
               if (strcasecmp(name,"name")==0)
                {
                 msg->FindString(name,&fname);
                 printf("writing %s: %s\n",name,fname.String());
                 node->RemoveAttr("Themis:Name");
                 node->WriteAttr("Themis:name",B_STRING_TYPE,0,fname.String(),fname.Length()+1);
                 continue;
                }
               if (strcasecmp(name,"host")==0)
                {
                 msg->FindString(name,&fname);
                 printf("writing %s: %s\n",name,fname.String());
                 node->RemoveAttr("Themis:host");
                 node->WriteAttr("Themis:host",B_STRING_TYPE,0,fname.String(),fname.Length()+1);
                 continue;
                }
               if (strcasecmp(name,"content-type")==0)
                {
                 msg->FindString(name,&fname);
                 printf("writing %s: %s\n",name,fname.String());
                 node->RemoveAttr("Themis:mime_type");
                 node->WriteAttr("Themis:mime_type",B_STRING_TYPE,0,fname.String(),fname.Length()+1);
                 continue;
                }
               if (strcasecmp(name,"path")==0)
                {
                 msg->FindString(name,&fname);
                 printf("writing %s: %s\n",name,fname.String());
                 node->RemoveAttr("Themis:path");
                 node->WriteAttr("Themis:path",B_STRING_TYPE,0,fname.String(),fname.Length()+1);
                 continue;
                }
               if (strcasecmp(name,"etag")==0)
                {
                 msg->FindString(name,&fname);
                 printf("writing %s: %s\n",name,fname.String());
                 node->RemoveAttr("Themis:etag");
                 node->WriteAttr("Themis:etag",B_STRING_TYPE,0,fname.String(),fname.Length()+1);
                 continue;
                }
               if (strcasecmp(name,"last-modified")==0)
                {
                 msg->FindString(name,&fname);
                 printf("writing %s: %s\n",name,fname.String());
                 node->RemoveAttr("Themis:last-modified");
                 node->WriteAttr("Themis:last-modified",B_STRING_TYPE,0,fname.String(),fname.Length()+1);
                 continue;
                }
               if (strcasecmp(name,"expires")==0)
                {
                 msg->FindString(name,&fname);
                 printf("writing %s: %s\n",name,fname.String());
                 node->RemoveAttr("Themis:expires");
                 node->WriteAttr("Themis:expires",B_STRING_TYPE,0,fname.String(),fname.Length()+1);
                 continue;
                }
               if (strcasecmp(name,"content-md5")==0)
                {
                 msg->FindString(name,&fname);
                 printf("writing %s: %s\n",name,fname.String());
                 node->RemoveAttr("Themis:content-md5");
                 node->WriteAttr("Themis:content-md5",B_STRING_TYPE,0,fname.String(),fname.Length()+1);
                 continue;
                }
              }break;
            }
          // memset(name,0,B_OS_NAME_LENGTH); 
         }
        node->Unlock();
        node->Sync();
        delete node;
        reply.what=B_OK;
      delete msg;
      mmsg->SendReply(&reply);
     }break;
    case ClearCache:
     {
      mmsg->PrintToStream();
      printf("Clearing cache...\n");
      BVolumeRoster volr;
      BVolume vol;
      for (int i=0;i<5;i++)
      //the query repeats 5 times to make sure all appropriate files are removed
       {
        while (volr.GetNextVolume(&vol)==B_OK)
         {
          BQuery query;
          query.Clear();
          query.SetVolume(&vol);
          query.PushAttr("BEOS:TYPE");
          query.PushString(ThemisCacheMIME);
          query.PushOp(B_EQ);
          query.Fetch();
          snooze(100000);
          BEntry ent;
          char fname[B_FILE_NAME_LENGTH];
          while (query.GetNextEntry(&ent)==B_OK)
           {
            ent.GetName(fname);
            printf("\t\t%s: ",fname);
            if ((ent.InitCheck()==B_OK) && (ent.Exists()))
             {
              if(ent.Remove()==B_OK)
               printf("removed\n");
              else
               printf("error\n");
             }
           }
         }
        volr.Rewind();
       }
     }break;
    default:
     {
      BHandler::MessageReceived(mmsg);
     }
   }
 }
status_t cacheman::CheckMIME()
 {
  //MIME type goodness...
  BMimeType mime(ThemisCacheMIME);
  if (!mime.IsInstalled())
   {
    mime.Install();
    app_info ai;
    be_app->GetAppInfo(&ai);
    mime.SetAppHint(&ai.ref);
   }
  char type[B_MIME_TYPE_LENGTH];
  mime.GetShortDescription(type);
  if (strcasecmp(type,"Themis Cache File")!=0)
   mime.SetShortDescription("Themis Cache File");
  mime.GetLongDescription(type);
  if (strcasecmp(type,"Themis Cache File")!=0)
   mime.SetLongDescription("Themis Cache File");
  mime.GetPreferredApp(type);
  if (strcasecmp(type,ThemisAppSig)!=0)
   mime.SetPreferredApp(ThemisAppSig);
  BMessage attrinf;
  mime.GetAttrInfo(&attrinf);
  bool installall=false;
  int32 attrcount=0;
  if (attrinf.IsEmpty())
   installall=true;
  else
   {
    type_code typec;
    attrinf.GetInfo("attr:name",&typec,&attrcount);
   }
  bool found=false;
  //Make Themis:URL a visible attribute
  for (int32 i=0;i<attrcount;i++)
   {
    BString item;
    attrinf.FindString("attr:name",i,&item);
    if (item=="Themis:URL")
     {
      found=true;
      break;
     }
   }
  if ((!found) || (installall))
   {
    attrinf.AddString("attr:name","Themis:URL");
    attrinf.AddString("attr:public_name","URL");
    attrinf.AddInt32("attr:type",B_STRING_TYPE);
    attrinf.AddInt32("attr:width",200);
    attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
    attrinf.AddBool("attr:public",true);
    attrinf.AddBool("attr:editable",true);
    attrinf.AddBool("attr:viewable",true);
    attrinf.AddBool("attr:extra",false);
   }
  else
   found=false;
  //Make Themis:URL a visible attribute; done
  //Make Themis:mime_type a visible attribute
  for (int32 i=0;i<attrcount;i++)
   {
    BString item;
    attrinf.FindString("attr:name",i,&item);
    if (item=="Themis:mime_type")
     {
      found=true;
      break;
     }
   }
  if ((!found) || (installall))
   {
    attrinf.AddString("attr:name","Themis:mime_type");
    attrinf.AddString("attr:public_name","MIME Type");
    attrinf.AddInt32("attr:type",B_STRING_TYPE);
    attrinf.AddInt32("attr:width",200);
    attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
    attrinf.AddBool("attr:public",true);
    attrinf.AddBool("attr:editable",true);
    attrinf.AddBool("attr:viewable",true);
    attrinf.AddBool("attr:extra",false);
   }
  else
   found=false;
  //Make Themis:mime_type a visible attribute; done
  //Make Themis:name a visible attribute
  for (int32 i=0;i<attrcount;i++)
   {
    BString item;
    attrinf.FindString("attr:name",i,&item);
    if (item=="Themis:name")
     {
      found=true;
      break;
     }
   }
  if ((!found) || (installall))
   {
    attrinf.AddString("attr:name","Themis:name");
    attrinf.AddString("attr:public_name","Real Name");
    attrinf.AddInt32("attr:type",B_STRING_TYPE);
    attrinf.AddInt32("attr:width",200);
    attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
    attrinf.AddBool("attr:public",true);
    attrinf.AddBool("attr:editable",true);
    attrinf.AddBool("attr:viewable",true);
    attrinf.AddBool("attr:extra",false);
   }
  else
   found=false;
  //Make Themis:name a visible attribute; done
  //Make Themis:host a visible attribute
  for (int32 i=0;i<attrcount;i++)
   {
    BString item;
    attrinf.FindString("attr:name",i,&item);
    if (item=="Themis:host")
     {
      found=true;
      break;
     }
   }
  if ((!found) || (installall))
   {
    attrinf.AddString("attr:name","Themis:host");
    attrinf.AddString("attr:public_name","Host Name");
    attrinf.AddInt32("attr:type",B_STRING_TYPE);
    attrinf.AddInt32("attr:width",200);
    attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
    attrinf.AddBool("attr:public",true);
    attrinf.AddBool("attr:editable",true);
    attrinf.AddBool("attr:viewable",true);
    attrinf.AddBool("attr:extra",false);
   }
  else
   found=false;
  //Make Themis:host a visible attribute; done
  //Make Themis:path a visible attribute
  for (int32 i=0;i<attrcount;i++)
   {
    BString item;
    attrinf.FindString("attr:name",i,&item);
    if (item=="Themis:path")
     {
      found=true;
      break;
     }
   }
  if ((!found) || (installall))
   {
    attrinf.AddString("attr:name","Themis:path");
    attrinf.AddString("attr:public_name","Server Path");
    attrinf.AddInt32("attr:type",B_STRING_TYPE);
    attrinf.AddInt32("attr:width",200);
    attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
    attrinf.AddBool("attr:public",true);
    attrinf.AddBool("attr:editable",true);
    attrinf.AddBool("attr:viewable",true);
    attrinf.AddBool("attr:extra",false);
   }
  else
   found=false;
  //Make Themis:path a visible attribute; done
  //Make Themis:etag a visible attribute
  for (int32 i=0;i<attrcount;i++)
   {
    BString item;
    attrinf.FindString("attr:name",i,&item);
    if (item=="Themis:etag")
     {
      found=true;
      break;
     }
   }
  if ((!found) || (installall))
   {
    attrinf.AddString("attr:name","Themis:etag");
    attrinf.AddString("attr:public_name","Entity Tag");
    attrinf.AddInt32("attr:type",B_STRING_TYPE);
    attrinf.AddInt32("attr:width",200);
    attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
    attrinf.AddBool("attr:public",true);
    attrinf.AddBool("attr:editable",false);
    attrinf.AddBool("attr:viewable",true);
    attrinf.AddBool("attr:extra",false);
   }
  else
   found=false;
  //Make Themis:etag a visible attribute; done
    //Make Themis:last-modified a visible attribute
  for (int32 i=0;i<attrcount;i++)
   {
    BString item;
    attrinf.FindString("attr:name",i,&item);
    if (item=="Themis:last-modified")
     {
      found=true;
      break;
     }
   }
  if ((!found) || (installall))
   {
    attrinf.AddString("attr:name","Themis:last-modified");
    attrinf.AddString("attr:public_name","Last Modified");
    attrinf.AddInt32("attr:type",B_STRING_TYPE);
    attrinf.AddInt32("attr:width",200);
    attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
    attrinf.AddBool("attr:public",true);
    attrinf.AddBool("attr:editable",false);
    attrinf.AddBool("attr:viewable",true);
    attrinf.AddBool("attr:extra",false);
   }
  else
   found=false;
  //Make Themis:last-modified a visible attribute; done
  //Make Themis:content-length a visible attribute
  for (int32 i=0;i<attrcount;i++)
   {
    BString item;
    attrinf.FindString("attr:name",i,&item);
    if (item=="Themis:content-length")
     {
      found=true;
      break;
     }
   }
  if ((!found) || (installall))
   {
    attrinf.AddString("attr:name","Themis:content-length");
    attrinf.AddString("attr:public_name","Content Length");
    attrinf.AddInt32("attr:type",B_INT64_TYPE);
    attrinf.AddInt32("attr:width",50);
    attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
    attrinf.AddBool("attr:public",true);
    attrinf.AddBool("attr:editable",false);
    attrinf.AddBool("attr:viewable",true);
    attrinf.AddBool("attr:extra",false);
   }
  else
   found=false;
  //Make Themis:content-length a visible attribute; done
  //Make Themis:expires a visible attribute
  for (int32 i=0;i<attrcount;i++)
   {
    BString item;
    attrinf.FindString("attr:name",i,&item);
    if (item=="Themis:expires")
     {
      found=true;
      break;
     }
   }
  if ((!found) || (installall))
   {
    attrinf.AddString("attr:name","Themis:expires");
    attrinf.AddString("attr:public_name","Expiry");
    attrinf.AddInt32("attr:type",B_INT64_TYPE);
    attrinf.AddInt32("attr:width",200);
    attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
    attrinf.AddBool("attr:public",true);
    attrinf.AddBool("attr:editable",false);
    attrinf.AddBool("attr:viewable",true);
    attrinf.AddBool("attr:extra",false);
   }
  else
   found=false;
  //Make Themis:expires a visible attribute; done
  //Make Themis:content-md5 a visible attribute
  for (int32 i=0;i<attrcount;i++)
   {
    BString item;
    attrinf.FindString("attr:name",i,&item);
    if (item=="Themis:content-md5")
     {
      found=true;
      break;
     }
   }
  if ((!found) || (installall))
   {
    attrinf.AddString("attr:name","Themis:content-md5");
    attrinf.AddString("attr:public_name","MD5 Checksum");
    attrinf.AddInt32("attr:type",B_INT64_TYPE);
    attrinf.AddInt32("attr:width",200);
    attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
    attrinf.AddBool("attr:public",true);
    attrinf.AddBool("attr:editable",false);
    attrinf.AddBool("attr:viewable",true);
    attrinf.AddBool("attr:extra",false);
   }
  else
   found=false;
  
  //Make Themis:content-md5 a visible attribute; done
  //Make Themis:clearonnew a visible attribute
  for (int32 i=0;i<attrcount;i++)
   {
    BString item;
    attrinf.FindString("attr:name",i,&item);
    if (item=="Themis:clearonnew")
     {
      found=true;
      break;
     }
   }
  if ((!found) || (installall))
   {
    attrinf.AddString("attr:name","Themis:clearonnew");
    attrinf.AddString("attr:public_name","Clear On New Page");
    attrinf.AddInt32("attr:type",B_INT32_TYPE);
    attrinf.AddInt32("attr:width",20);
    attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
    attrinf.AddBool("attr:public",false);
    attrinf.AddBool("attr:editable",false);
    attrinf.AddBool("attr:viewable",true);
    attrinf.AddBool("attr:extra",false);
   }
  else
   found=false;
  
  //Make Themis:clearonnew a visible attribute; done
  mime.SetAttrInfo(&attrinf);
  return B_OK;
 }
status_t cacheman::CheckIndices()
 {
//  status_t stat;
  BVolumeRoster *volr=new BVolumeRoster;
  BVolume vol;
  dirent *ent;
  DIR *d;
  char voln[256];
  bool found;
  while (volr->GetNextVolume(&vol)==B_NO_ERROR)
   {
    if ((vol.KnowsQuery()) && (vol.KnowsAttr()) && (!vol.IsReadOnly()))
     {
      memset(voln,0,256);
      vol.GetName(voln);
      printf("Checking %s...\n",voln);
//      fflush(stdout);
      d=fs_open_index_dir(vol.Device());
      if (!d)
       {
        printf("\tcouldn't open index directory.\n");
        continue;
       }
      found=false;
      //duplicate and modify the next block as necessary to add more indices
      //begin block
      //BEOS:TYPE is where the file was originally found
      //double check to make sure that the mime type index exists
      printf("\t\tlooking for \"BEOS:TYPE\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"BEOS:TYPE")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"BEOS:TYPE",B_STRING_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:URL is where the file was originally found
      printf("\t\tlooking for \"Themis:URL\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:URL")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:URL",B_STRING_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:mime_type is the file's real mime_type.
      printf("\t\tlooking for \"Themis:mime_type\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:mime_type")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:mime_type",B_STRING_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:name is the file's actual name, as stored on the server
      //minus the remainder of the URL.
      printf("\t\tlooking for \"Themis:name\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:name")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:name",B_STRING_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:host is the server name or ip address where the resource was
      //found.
      printf("\t\tlooking for \"Themis:host\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:host")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:host",B_STRING_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:path is the path (minus filename) on the server where
      //the file was originally found.
      printf("\t\tlooking for \"Themis:path\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:path")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:path",B_STRING_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:etag 
      printf("\t\tlooking for \"Themis:etag\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:etag")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:etag",B_STRING_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
       //begin block
      //Themis:last-modified is the file's real mime_type.
      printf("\t\tlooking for \"Themis:last-modified\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:last-modified")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:last-modified",B_STRING_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:content-length is the file's real mime_type.
      printf("\t\tlooking for \"Themis:mime_type\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:content-length")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:content-length",B_INT64_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:expires is the file's real mime_type.
      printf("\t\tlooking for \"Themis:expires\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:expires")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:expires",B_STRING_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:content-md5 is the file's real mime_type.
      printf("\t\tlooking for \"Themis:md5\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:content-md5")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:content-md5",B_STRING_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:clearonnew is the file's real mime_type.
      printf("\t\tlooking for \"Themis:clearonnew\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:clearonnew")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:clearonnew",B_INT32_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
    fs_close_index_dir(d);
   }
   }
  delete volr;
  return B_OK;
 }
status_t cacheman::FindCacheDir()
 {
  BPath path;
  status_t stat;
  if ((stat=find_directory(B_USER_SETTINGS_DIRECTORY,&path))==B_OK)
   {
    if((stat=path.Append("Themis"))==B_OK)
     {
      BEntry entry(path.Path(),true);
      if ((stat=entry.InitCheck())==B_OK)
       {
        if (entry.Exists())
         {
          if (entry.IsDirectory())
           {
            if ((stat=path.Append("cache"))==B_OK)
             {
              if (entry.Exists())
               {
                if (entry.IsDirectory())
                 {
                  cachepath=path;
                 }
                else
                 stat=B_ERROR;
               }
              else
               {
                stat=create_directory(path.Path(),0700);
                cachepath=path;
               }
             }
            return stat;
           }
          else
           {
            path.GetParent(&path);
            if ((stat=path.Append("ThemisWeb"))==B_OK)
             {
              entry.SetTo(path.Path(),true);
              if ((stat=entry.InitCheck())==B_OK)
               {
                if (entry.Exists())
                 {
                  if (entry.IsDirectory())
                   {
                    if ((stat=path.Append("cache"))==B_OK)
                     {
                      entry.SetTo(path.Path(),true);
                      if (entry.Exists())
                       {
                        if (entry.IsDirectory())
                         {
                          cachepath=path;
                         }
                        else
                         stat=B_ERROR;
                       }
                      else
                       {
                        stat=create_directory(path.Path(),0700);
                        cachepath=path;
                       }
                     }
                    return stat;
                   }
                  else
                   stat=B_ERROR;
                  return stat;
                 }
                else
                 {
                  path.Append("cache");
                  stat=create_directory(path.Path(),0700);
                  cachepath=path;
                 }
               }
              return stat;
             }
            return stat;
           }
         }
        else
         {
          if ((stat=path.Append("cache"))==B_OK)
           {
            stat=create_directory(path.Path(),0700);
           }
          cachepath=path;
          return stat;
         }
       }
      return stat;
     }
    return stat;
   }
  return stat; 
 }
