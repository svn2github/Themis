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
#include <StorageKit.h>
plugman::plugman()
 {
  head=tail=NULL;
 }
plugman::~plugman()
 {
 }
status_t plugman::UnloadAllPlugins()
 {
 }
status_t plugman::UnloadPlugin()
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
status_t plugman::BuildRoster()
 {
  app_info ai;
  be_app->GetAppInfo(&ai);
  BPath path(&ai.ref);
  path.GetParent(&path);
  path.Append("add-ons");
  BDirectory addondir(path.Path());
  BEntry ent;
  BNode node;
  BNodeInfo ni;
  BString mtype;
   {
    if (head!=NULL)
     {
      plugst *tmp=head;
      while(head!=NULL)
       {
        if (head->inmemory)
         {
          unload_add_on(head->sysid);
         }
        tmp=head->next;
        delete head;
        head=tmp;
       }
      head=tail=NULL;
     }
   }
  while(addondir.GetNextEntry(&ent)==B_OK)
   {
    if (ent.IsDirectory())
     {
     }
    else
     {
      plugst *tmp=new plugst;
      ent.GetPath(&path);
      node.SetTo(path.Path());
      ni.SetTo(&node);
      char *type=mtype.LockBuffer(B_MIME_TYPE_LENGTH);
      ni.GetType(type);
      mtype.UnlockBuffer(-1);
      ni.SetTo(NULL);
      node.Unset();
      if (mtype.IFindFirst("executable")==B_ERROR)
       {//make sure the mimetype contains "executable"
        delete tmp;
        continue;
       }
      tmp->sysid=load_add_on(path.Path());
      if (tmp->sysid<=B_ERROR)
       {//if the sysid is an error of some sort, it's not an add-on
        delete tmp;
        continue;
       }
     }
   }
 }