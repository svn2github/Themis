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

#include "plugman.h"
#include <stdio.h>
#define CheckFile 'chkf'
plugman::plugman(entry_ref &appdirref):BLooper("plug-in manager",B_LOW_PRIORITY)
 {
  //ref should be the application directory
  Run();
  head=tail=NULL;
  status_t stat=B_OK;
  appaddondir=new BDirectory(&appdirref);
  if (!appaddondir->Contains("add-ons",B_DIRECTORY_NODE))
   {
    if ((stat=appaddondir->CreateDirectory("add-ons",appaddondir))!=B_OK)
     {
      BString err; 
      switch(stat) 
       {
        case B_BAD_VALUE:
         err="B_BAD_VALUE";
         break;
        case B_BUSY:
         err="B_BUSY";
         break;
        case B_ENTRY_NOT_FOUND:
         err="B_ENTRY_NOT_FOUND";
         break;
        case B_FILE_ERROR:
         err="B_FILE_ERROR";
         break;
        case B_FILE_EXISTS:
         err="B_FILE_EXISTS";
         break;
        case B_LINK_LIMIT:
         err="B_LINK_LIMIT";
         break;
        case B_NAME_TOO_LONG:
         err="B_NAME_TOO_LONG";
         break;
        case B_NO_MEMORY:
         err="B_NO_MEMORY";
         break;
        case B_NO_MORE_FDS:
         err="B_NO_MORE_FDS";
         break;
        case B_IS_A_DIRECTORY:
         err="B_IS_A_DIRECTORY";
         break;
        case B_NOT_A_DIRECTORY:
         err="B_NOT_A_DIRECTORY";
         break;
        case B_NOT_ALLOWED:
         err="B_NOT_ALLOWED";
         break;
        case B_PERMISSION_DENIED:
         err="B_PERMISSION_DENIED";
         break;
        case E2BIG:
         err="E2BIG";
         break;
        default:
         err="Unknown Error";
       }
      printf("App add-ons directory missing, and couldn't be created! (%s)\n",err.String());
     }
    }
   if (appaddondir->InitCheck()==B_OK)
    {
     node_ref nref;
     appaddondir->SetTo(appaddondir,"add-ons");
     appaddondir->GetNodeRef(&nref);
     stat=watch_node(&nref,B_WATCH_DIRECTORY,this,this);
     BString status;
     switch(stat)
      {
       case B_OK:
        status="B_OK";
        break;
       case B_BAD_VALUE:
        status="B_BAD_VALUE";
        break;
       case B_NO_MEMORY:
        status="B_NO_MEMORY";
        break;
       case B_ERROR:
        status="B_ERROR";
        break;
       default:
        status="Unknown Error";
      }
     if (stat!=B_OK)
      printf("Themis couldn't set up node monitoring; new add-ons will require shutdown to enable. (%s)\n",status.String());
    }
   BPath useraddonpath;
   find_directory(B_USER_ADDONS_DIRECTORY,&useraddonpath);
   useraddondir=new BDirectory(useraddonpath.Path());
   printf("%s\n",useraddonpath.Path());
   if (!useraddondir->Contains("Themis",B_DIRECTORY_NODE))
    {
     printf("Themis doesn't exist as a folder at above path\n");
    if ((stat=appaddondir->CreateDirectory("Themis",NULL))!=B_OK)
     {
      BString err; 
      switch(stat) 
       {
        case B_BAD_VALUE:
         err="B_BAD_VALUE";
         break;
        case B_BUSY:
         err="B_BUSY";
         break;
        case B_ENTRY_NOT_FOUND:
         err="B_ENTRY_NOT_FOUND";
         break;
        case B_FILE_ERROR:
         err="B_FILE_ERROR";
         break;
        case B_FILE_EXISTS:
         err="B_FILE_EXISTS";
         break;
        case B_LINK_LIMIT:
         err="B_LINK_LIMIT";
         break;
        case B_NAME_TOO_LONG:
         err="B_NAME_TOO_LONG";
         break;
        case B_NO_MEMORY:
         err="B_NO_MEMORY";
         break;
        case B_NO_MORE_FDS:
         err="B_NO_MORE_FDS";
         break;
        case B_IS_A_DIRECTORY:
         err="B_IS_A_DIRECTORY";
         break;
        case B_NOT_A_DIRECTORY:
         err="B_NOT_A_DIRECTORY";
         break;
        case B_NOT_ALLOWED:
         err="B_NOT_ALLOWED";
         break;
        case B_PERMISSION_DENIED:
         err="B_PERMISSION_DENIED";
         break;
        case E2BIG:
         err="E2BIG";
         break;
        default:
         err="Unknown Error";
       }
      printf("User add-ons directory missing, and couldn't be created! (%s)\n",err.String());
     }
    }
   if (useraddondir->InitCheck()==B_OK)
    {
     node_ref nref;
     useraddondir->SetTo(useraddondir,"Themis");
     useraddondir->GetNodeRef(&nref);
     stat=watch_node(&nref,B_WATCH_DIRECTORY,this,this);
     BString status;
     switch(stat)
      {
       case B_OK:
        status="B_OK";
        break;
       case B_BAD_VALUE:
        status="B_BAD_VALUE";
        break;
       case B_NO_MEMORY:
        status="B_NO_MEMORY";
        break;
       case B_ERROR:
        status="B_ERROR";
        break;
       default:
        status="Unknown Error";
      }
     if (stat!=B_OK)
      printf("Themis couldn't set up node monitoring; new add-ons will require shutdown to enable. (%s)\n",status.String());
    }
  
 }
plugman::~plugman()
 {
  stop_watching(this,NULL);
  delete appaddondir;
  delete useraddondir;
 }
bool plugman::QuitRequested()
 {
  return true;
 }
void plugman::MessageReceived(BMessage *msg)
 {
  msg->PrintToStream();
  switch(msg->what)
   {
    case CheckFile:
     {
      int32 count=0;
      type_code code;
      BNode node;
      BNodeInfo ni;
      entry_ref ref;
      BEntry *ent=new BEntry;
      BPath path;
      char mtype[B_MIME_TYPE_LENGTH];
      msg->GetInfo("refs",&code,&count);
      for (int32 i=0; i<count ; i++)
       {
        msg->FindRef("refs",i,&ref);
        ent->SetTo(&ref,true);
        if (ent->IsDirectory())
         {//recurse into directories
          BDirectory *dir=new BDirectory(ent);
          BMessenger *msgr=new BMessenger(NULL,this,NULL);
          entry_ref ref2;
          BMessage *msg2=new BMessage(CheckFile);
          while(dir->GetNextRef(&ref2)==B_OK)
           msg2->AddRef("refs",&ref2);
          msgr->SendMessage(msg2);
          delete msgr;
          delete msg2;
          delete dir;
          continue;
         }
        node.SetTo(&ref);
        ni.SetTo(&node);
        ni.GetType(mtype);
        ni.SetTo(NULL);
        if (strstr(mtype,"executable")==NULL)
         continue;//all add-on's have an executable mimetype
        plugst *nuplug=new plugst;
        nuplug->ref=ref;
        ent->GetPath(&path);
        printf("Attempting to load plugin at %s\n",path.Path());
        nuplug->sysid=load_add_on(path.Path());
        if (nuplug->sysid<=B_ERROR)
         {
          printf("\t\tFailed.\n");
          delete nuplug;
          continue;
         }
        status_t (*Initialize)(bool go);
        if (get_image_symbol(nuplug->sysid,"Initialize",B_SYMBOL_TYPE_TEXT,(void**)&Initialize)==B_OK)
         {
          printf("\tInitializing...");
          fflush(stdout);
          if ((*Initialize)(false)!=B_OK)
           {
            printf("failure, aborting\n");
            unload_add_on(nuplug->sysid);
            delete nuplug;
            continue;
           }
          printf("success\n");
         }
        else
         {
          printf("\tUnable to load initializer function, aborting.\n");
          unload_add_on(nuplug->sysid);
          delete nuplug;
          continue;
         }
        printf("Attempting to get object...");
        fflush(stdout);
        status_t stat;
        if ((stat=get_image_symbol(nuplug->sysid,"GetObject",B_SYMBOL_TYPE_TEXT,(void**)&(nuplug->GetObject)))==B_OK)
         {
          printf("success.\n");
          nuplug->pobj=(*nuplug->GetObject)();
          if (FindPlugin(nuplug->pobj->PlugID())!=NULL)
           {
            printf("plug-in already loaded.\n");
            unload_add_on(nuplug->sysid);
            delete nuplug;
            continue;
           }
          strcpy(nuplug->path,path.Path());
          nuplug->plugid=nuplug->pobj->PlugID();
          printf("Loaded \"%s\" (%c%c%c%c) V. %1.2f\n",nuplug->pobj->PlugName(),
          (char)nuplug->pobj->PlugID()>>24,(char)nuplug->pobj->PlugID()>>16,(char)nuplug->pobj->PlugID()>>8,(char)nuplug->pobj->PlugID(),
          nuplug->pobj->PlugVersion());
          if (!nuplug->pobj->IsPersistant())
           {
           }
         }
        else
         {
          printf("failure, aborting %ld\n",stat);
          unload_add_on(nuplug->sysid);
          delete nuplug;
          continue;
         }
        AddPlug(nuplug);
        printf("\tPlug-in successfully loaded.\n");
        node.Unset();
        ent->Unset();
       }
      delete ent;
     }break;
    default:
     BLooper::MessageReceived(msg);
   }
 }
void *plugman::FindPlugin(uint32 which)
 {
  plugst *tmp=head;
  while(tmp!=NULL)
   {
    if (tmp->pobj->PlugID()==which)
     {
      printf("plugman::FindPlugin: found it\n");
      break;
     }
    tmp=tmp->next;
   }
  if (tmp==NULL)
   return NULL;
//  if (tmp->pobj!=NULL)
   return tmp->pobj;
//  return tmp->handler;
 }
void plugman::AddPlug(plugst *plug)
 {
  if (head==NULL)
   {
    head=plug;
   }
  else
   {
    plugst *tmp=head,*last=NULL;
    while (tmp->next!=NULL)
     {
      last=tmp;
      tmp=tmp->next;
      if (tmp->prev!=last)
       tmp->prev=last;
     }
    tail=plug;
    tmp->next=plug;
    plug->prev=last;
   }
 }
status_t plugman::UnloadAllPlugins(bool clean)
 {
  plugst *cur=head,*tmp;
  while (cur!=NULL)
   {
    if (clean)
     {
      tmp=cur->next;
      if (cur->inmemory)
       unload_add_on(cur->sysid);
      delete cur;
      cur=tmp;
      continue;
     }
   }
  return B_OK;
 }
   status_t plugman::UnloadPlugin(uint32 which)
    {
     return B_OK;
    }
   status_t plugman::LoadPlugin(uint32 which)
    {
     return B_OK;
    }
   status_t plugman::LoadPluginFor(const char *mimetype)
    {
     return B_OK;
    }
   status_t plugman::ReloadPlugin(uint32 which)
    {
     return B_OK;
    }
status_t plugman::BuildRoster(bool clean)
 {
  UnloadAllPlugins(clean);
  entry_ref ref;
  BDirectory *dir;
  BMessenger *msgr=new BMessenger(NULL,this,NULL);
  BMessage *msg=new BMessage(CheckFile);
  for (int8 i=0; i<2; i++)
   {
    if (i==0)
     dir=appaddondir;
    else
     dir=useraddondir;
    while (dir->GetNextRef(&ref)==B_OK)
     msg->AddRef("refs",&ref);
    dir->Rewind();
   }
  msgr->SendMessage(msg);
  delete msgr;
  delete msg;
  return B_OK;
 }
