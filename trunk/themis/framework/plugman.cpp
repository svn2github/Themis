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

plugman::plugman()
        :BLooper("plug-in manager",B_LOW_PRIORITY)
 {
  Run();
  head=tail=NULL;
  BPath path;
   {
    app_info ai;
    be_app->GetAppInfo(&ai);
    path.SetTo(&ai.ref);
    path.GetParent(&path);
    path.Append("add-ons");
   }
  addondir=new BDirectory(path.Path());
 }
plugman::~plugman()
 {
  if (head!=NULL)
   {
    plugst *tmp=head;
    while (tmp!=NULL)
     {
      printf("removing plugin: %s\n",tmp->plugname);
//      tmp=head;
      head=head->next;
      if (tmp->inmemory)
       {
        unload_add_on(tmp->sysid);
       }
      delete tmp;
      tmp=head;
     }
   }
  delete addondir;
 }
status_t plugman::UnloadAllPlugins()
 {
 }
status_t plugman::UnloadPlugin(int32 which)
 {
 }
status_t plugman::LoadPlugin(int32 which)
 {
 }
status_t plugman::LoadPluginFor(const char *mimetype)
 {
 }
status_t plugman::ReloadPlugin(int32 which)
 {
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
  switch(plug->plugid)
   {
    case 'http':
     {
      BMessage msg('http');
      msg.AddPointer("http",plug->pobj);
      be_app_messenger.SendMessage(&msg);
     }break;
    default:
     {
     }
   }
 }
status_t plugman::BuildRoster()
 {
  BPath path;
  BEntry ent;
  BNode node;
  BNodeInfo ni;
  while(addondir->GetNextEntry(&ent)==B_OK)
   {
    if (ent.IsDirectory())
     {
     }
    else
     {
      ent.GetPath(&path);
      node.SetTo(path.Path());
      node.Lock();
      ni.SetTo(&node);
      BString mtype;
       {
        char *type=mtype.LockBuffer(B_MIME_TYPE_LENGTH);
        ni.GetType(type);
        mtype.UnlockBuffer(-1);
       }
      node.Unlock();
      node.Unset();
      ni.SetTo(NULL);
      if (mtype.IFindFirst("executable")==B_ERROR)
       {//all add-on's have an executable mimetype
        continue;
       }
      plugst *nuplug=new plugst;
      nuplug->sysid=load_add_on(path.Path());
      if (nuplug->sysid<=B_ERROR)
       {//if we have an error, we don't have a plugin
        delete nuplug;
        continue;
       }
      printf("loading a plugin from: %s\n",path.Path());
      ent.GetRef(&nuplug->ref);
      status_t (*Initialize)(bool go);
      if (get_image_symbol(nuplug->sysid,"Initialize",B_SYMBOL_TYPE_TEXT,(void**)&Initialize)==B_OK)
       {
        printf("\tInitializing plugin...");
        fflush(stdout);
        if ((*Initialize)(false)==B_OK)
         printf("ok\n");
        else
         printf("error\n");
       }
      else
       {
        unload_add_on(nuplug->sysid);
        delete nuplug;
        continue;
       }
      protocol_plugin* (*GetObject)(void);
      printf("\tGetting plugin's object/handler...");
      fflush(stdout);
      status_t err=B_OK;
      if ((err=get_image_symbol(nuplug->sysid,"GetObject",B_SYMBOL_TYPE_TEXT,(void**)&GetObject))==B_OK)
       {
        nuplug->pobj=(*GetObject)();
        printf("ok %p\n",nuplug->pobj);
        nuplug->plugname=nuplug->pobj->GetPluginName();
        nuplug->plugid=nuplug->pobj->GetPluginID();
        nuplug->persistant=nuplug->pobj->IsPersistant();
        AddPlug(nuplug);
        if (!nuplug->persistant)
         {
          status_t (*Shutdown)();
          get_image_symbol(nuplug->sysid,"Shutdown",B_SYMBOL_TYPE_TEXT,(void**)&Shutdown);
          if ((*Shutdown)()==B_OK)
           {
            unload_add_on(nuplug->sysid);
            nuplug->pobj=NULL;
           }
         }
        else
         nuplug->inmemory=true;
       }
      else
       {
        BHandler* (*GetHandler)(void);
        if ((err=get_image_symbol(nuplug->sysid,"GetHandler",B_SYMBOL_TYPE_TEXT,(void**)&GetHandler))==B_OK)
         {
          nuplug->handler=(*GetHandler)();
          char* (*GetPluginName)();
          int32 (*GetPluginID)();
          float (*GetPluginVers)();
          bool (*IsPersistant)();
          get_image_symbol(nuplug->sysid,"GetPluginName",B_SYMBOL_TYPE_TEXT,(void**)&GetPluginName);
          get_image_symbol(nuplug->sysid,"GetPluginID",B_SYMBOL_TYPE_TEXT,(void**)&GetPluginID);
          get_image_symbol(nuplug->sysid,"GetPluginVers",B_SYMBOL_TYPE_TEXT,(void**)&GetPluginVers);
          get_image_symbol(nuplug->sysid,"IsPersistant",B_SYMBOL_TYPE_TEXT,(void**)&IsPersistant);
        nuplug->plugname=(*GetPluginName)();
        nuplug->plugid=(*GetPluginID)();
        nuplug->persistant=(*IsPersistant)();
          printf("ok %p\n",nuplug->handler);
          AddPlug(nuplug);

          if (!nuplug->persistant)
           {
            status_t (*Shutdown)();
            get_image_symbol(nuplug->sysid,"Shutdown",B_SYMBOL_TYPE_TEXT,(void**)&Shutdown);
            if ((*Shutdown)()==B_OK)
             {
              unload_add_on(nuplug->sysid);
              nuplug->pobj=NULL;
             }
           }
          else
           nuplug->inmemory=true;

         }
        else
         {
          delete nuplug;
         }
       }
     }
   }
 }
