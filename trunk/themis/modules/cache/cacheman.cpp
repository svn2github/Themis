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

cacheman::cacheman()
         :BHandler("cache_manager")
 {
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
                  cachepath.SetTo(&entry);
                 }
                else
                 stat=B_ERROR;
               }
              else
               {
                stat=create_directory(path.Path(),0700);
                cachepath.SetTo(&entry);
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
                          cachepath.SetTo(&entry);
                         }
                        else
                         stat=B_ERROR;
                       }
                      else
                       {
                        stat=create_directory(path.Path(),0700);
                        cachepath.SetTo(&entry);
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
                  cachepath.SetTo(&entry);
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
          return stat;
         }
       }
      return stat;
     }
    return stat;
   }
  return stat; 
 }
