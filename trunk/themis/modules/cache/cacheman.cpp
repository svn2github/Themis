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
cacheman::cacheman()
         :BHandler("cache_manager"),
          PlugClass()
 {
  if (FindCacheDir()==B_OK)
   printf("Found or created the cache directory.\n");
  printf("cache path: %s\n",cachepath.Path());
  CheckIndices();
  CheckMIME();
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
 }
void cacheman::MessageReceived(BMessage *msg)
 {
  switch(msg->what)
   {
    case FindCachedObject:
     {
     }break;
    case CreateCacheObject:
     {
      printf("CreateCacheObject\n");
      BMessage reply(B_ERROR);
      type_code type;
      int32 count,index;
      char name[B_OS_NAME_LENGTH];
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
        
        msg->PrintToStream();
        for (index=0;msg->GetInfo(B_ANY_TYPE,index,(const char**)name,&type,&count)==B_OK; index++)
         {
          for (int i=0;i<count;i++)
           switch(type)
            {
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
                 node->WriteAttr("Themis:URL",B_STRING_TYPE,0,fname.String(),fname.Length()+1);
                 continue;
                }
               if (strcasecmp(name,"name")==0)
                {
                 msg->FindString(name,&fname);
                 node->WriteAttr("Themis:name",B_STRING_TYPE,0,fname.String(),fname.Length()+1);
                 continue;
                }
               if (strcasecmp(name,"host")==0)
                {
                 msg->FindString(name,&fname);
                 node->WriteAttr("Themis:host",B_STRING_TYPE,0,fname.String(),fname.Length()+1);
                 continue;
                }
               if (strcasecmp(name,"mime")==0)
                {
                 msg->FindString(name,&fname);
                 node->WriteAttr("Themis:mime_type",B_STRING_TYPE,0,fname.String(),fname.Length()+1);
                 continue;
                }
               if (strcasecmp(name,"path")==0)
                {
                 msg->FindString(name,&fname);
                 node->WriteAttr("Themis:path",B_STRING_TYPE,0,fname.String(),fname.Length()+1);
                 continue;
                }
              }break;
            }
         }
        node->Unlock();
        node->Sync();
        delete node;
        delete file;
        reply.what=B_OK;
        reply.AddRef("ref",&ref);
       }
      msg->SendReply(&reply);
     }break;
    case UpdateCachedObject:
     {
     }break;
    case ClearCache:
     {
      msg->PrintToStream();
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
      BHandler::MessageReceived(msg);
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
  mime.SetAttrInfo(&attrinf);
  return B_OK;
 }
status_t cacheman::CheckIndices()
 {
  status_t stat;
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
