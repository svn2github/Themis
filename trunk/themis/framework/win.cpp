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
#include "win.h"
extern plugman *PluginManager;
Win::Win(BRect frame,const char *title,window_type type,uint32 flags,uint32 wspace)
    :BWindow(frame,title,type,flags,wspace)
 {
  startup=true;
  view=new winview(Bounds(),"mainview",B_FOLLOW_ALL,B_WILL_DRAW|B_FRAME_EVENTS|B_ASYNCHRONOUS_CONTROLS|B_NAVIGABLE|B_NAVIGABLE_JUMP);
  AddChild(view);
  
  AddHandler((BHandler*)PluginManager->FindPlugin(CachePlugin));
 }
Win::Win(BRect frame,const char *title, window_look look,window_feel feel,uint32 flags,uint32 wspace)
    :BWindow(frame,title,look,feel,flags,wspace)
 {
 }
Win::Win(BMessage *archive)
    :BWindow(archive)
 {
 }
bool Win::QuitRequested()
 {
  RemoveHandler((BHandler*)PluginManager->FindPlugin(CachePlugin));
  return true;
 }
void Win::MessageReceived(BMessage *msg)
 {
  switch(msg->what)
   {
    default:
     {
      BWindow::MessageReceived(msg);
     }
   }
 }
void Win::WindowActivated(bool active)
 {
  if (startup)
   {
    BMessenger *msgr=new BMessenger((BHandler*)PluginManager->FindPlugin(CachePlugin),NULL,NULL);
    msgr->SendMessage(ClearCache);
    delete msgr;
    startup=false;
   }
 }
