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
#include <stdio.h>
#include <kernel/fs_index.h>
#include <string.h>
cacheman::cacheman()
         :BHandler("cache_manager")
 {
  if (FindCacheDir()==B_OK)
   printf("Found or created the cache directory.\n");
  printf("cache path: %s\n",cachepath.Path());
  CheckIndices();
 }
cacheman::~cacheman()
 {
 }
void cacheman::MessageReceived(BMessage *msg)
 {
  switch(msg->what)
   {
    default:
     {
      BHandler::MessageReceived(msg);
     }
   }
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
