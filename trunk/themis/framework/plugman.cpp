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
   if (appaddondir->Contains("add-ons",B_DIRECTORY_NODE))
     appaddondir->SetTo(appaddondir,"add-ons");
   if (appaddondir->InitCheck()==B_OK)
    {
     node_ref nref;
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
    case B_NODE_MONITOR:
     {
      printf("Node monitor!\n");
      int32 opcode;
      dev_t device;
      ino_t directory, node;
      const char *name;
      entry_ref ref;
      node_ref nref;
      if (msg->FindInt32("opcode",&opcode)==B_OK)
       {
        switch(opcode)
         {
          case B_ENTRY_CREATED:
           {
            msg->FindInt32("device",&ref.device);
            msg->FindInt64("directory",&ref.directory);
            msg->FindString("name",&name);
            ref.set_name(name);
           }break;
          case B_ENTRY_REMOVED:
           {
            msg->FindInt32("device",&nref.device);
            msg->FindInt64("node",&nref.node);
            plugst *tmp=(plugst*)FindPlugin(nref);
            if (tmp!=NULL)
             {
              printf("Unloading plugin %s\n",tmp->pobj->PlugName());
              UnloadPlugin(tmp->plugid);
             }
           }break;
         }
       }
     }break;
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
        node.GetNodeRef(&nuplug->nref);
        ent->GetPath(&path);
        printf("Attempting to load plugin at %s\n",path.Path());
        nuplug->sysid=load_add_on(path.Path());
        if (nuplug->sysid<=B_ERROR)
         {
          printf("\t\tFailed.\n");
          delete nuplug;
          continue;
         }
        status_t (*Initialize)(void *info);
        if (get_image_symbol(nuplug->sysid,"Initialize",B_SYMBOL_TYPE_TEXT,(void**)&Initialize)==B_OK)
         {
          printf("\tInitializing...");
          fflush(stdout);
          if ((*Initialize)(NULL)!=B_OK)
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
            status_t (*Shutdown)(bool);
            if (get_image_symbol(nuplug->sysid,"Shutdown",B_SYMBOL_TYPE_TEXT,(void**)&Shutdown)==B_OK)
             (*Shutdown)(true);
            else
             printf("plug-in has no Shutdown function; couldn't shutdown nicely.\n");
            unload_add_on(nuplug->sysid);
            nuplug->pobj=NULL;//yeah, I know it's about to be deleted, but...
            delete nuplug;
            continue;
           }
          strcpy(nuplug->path,path.Path());
          nuplug->plugid=nuplug->pobj->PlugID();
         // printf("%c%c%c%c\n",nuplug->pobj->PlugID()>>24,nuplug->pobj->PlugID()>>16,nuplug->pobj->PlugID()>>8,nuplug->pobj->PlugID());
          printf("Loaded \"%s\" (%c%c%c%c) V. %1.2f\n",nuplug->pobj->PlugName(),
          nuplug->pobj->PlugID()>>24,nuplug->pobj->PlugID()>>16,nuplug->pobj->PlugID()>>8,nuplug->pobj->PlugID(),
          nuplug->pobj->PlugVersion());
          if (nuplug->pobj->SecondaryID()!=0)
           printf("\tSecondary ID: %c%c%c%c\n",nuplug->pobj->SecondaryID()>>24,nuplug->pobj->SecondaryID()>>16,nuplug->pobj->SecondaryID()>>8,nuplug->pobj->SecondaryID());
          if (!nuplug->pobj->IsPersistant())
           {
            status_t (*Shutdown)(bool);
            if (get_image_symbol(nuplug->sysid,"Shutdown",B_SYMBOL_TYPE_TEXT,(void**)&Shutdown)==B_OK)
             (*Shutdown)(true);
            else
             printf("plug-in has no Shutdown function; couldn't shutdown nicely.\n");
            unload_add_on(nuplug->sysid);//this will probably trigger a crash
            nuplug->inmemory=false;
            nuplug->pobj=NULL;
           }
          else
           {
            nuplug->inmemory=true;
            if (nuplug->pobj->IsHandler())
             AddHandler((BHandler*)nuplug->pobj->Handler());
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
void *plugman::FindPlugin(uint32 which, uint32 secondary)
 {
  plugst *tmp=head;
  while(tmp!=NULL)
   {
    if ((tmp->plugid)==which)
     {
      printf("plugman::FindPlugin: found it\n");
      break;
     }
    tmp=tmp->next;
   }
  if (tmp==NULL)
   return NULL;
  if (!tmp->inmemory)
   {
  	if (LoadPlugin(tmp->plugid)!=B_OK)
  		return NULL;
   if (tmp->pobj->IsHandler())
    AddHandler((BHandler*)tmp->pobj->Handler());
   }
   return tmp->pobj;
 }
void *plugman::FindPlugin(node_ref &nref)
 {
  plugst *tmp=head;
  while (tmp!=NULL)
   {
    if (tmp->nref==nref)
     {
      break;
     }
    tmp=tmp->next;
   }
  return tmp;
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
       {
        status_t (*Shutdown)(bool);
        if (get_image_symbol(cur->sysid,"Shutdown",B_SYMBOL_TYPE_TEXT,(void**)&Shutdown)==B_OK)
          (*Shutdown)(true);
        unload_add_on(cur->sysid);
       }
      delete cur;
      cur=tmp;
      continue;
     }
   }
  return B_OK;
 }
   status_t plugman::UnloadPlugin(uint32 which)
    {
     plugst *cur=head,*prev=NULL;
     while (cur!=NULL)
      {
       if (cur->plugid==which)
        {
         if (prev==NULL)
          {
           head=cur->next;
           if (head!=NULL)
            head->prev=NULL;
          }
         else
          {
           prev->next=cur->next;
           if (cur->next!=NULL)
            cur->next->prev=prev;
          }
         if (cur->inmemory)
          {
           status_t (*Shutdown)(bool);
           if (get_image_symbol(cur->sysid,"Shutdown",B_SYMBOL_TYPE_TEXT,(void**)&Shutdown)==B_OK)
             (*Shutdown)(true);
           unload_add_on(cur->sysid);
          }
         delete cur;
         break;
        }
       prev=cur;
       cur=cur->next;
      }
     return B_OK;
    }
   status_t plugman::LoadPlugin(uint32 which)
    {
     plugst *cur=head,*tmp;
     bool found=false;
     while (cur!=NULL)
      {
       if (cur->plugid==which)
        {
         if (!cur->inmemory)
          {
           cur->sysid=load_add_on(cur->path);
           if (cur->sysid<=B_ERROR)
            {
             return B_ERROR;
            }
           status_t (*Initialize)(void *info);
           get_image_symbol(cur->sysid,"Initialize",B_SYMBOL_TYPE_TEXT,(void**)&Initialize);
           get_image_symbol(cur->sysid,"GetObject",B_SYMBOL_TYPE_TEXT,(void**)&(cur->GetObject));
           (*Initialize)(NULL);
           cur->pobj=(*cur->GetObject)();
          }
         else
          {
           //plugin is already in memory
          }
         found=true;
         break;
        }
       cur=cur->next;
      }
     return (found ? B_OK:B_ERROR);
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
