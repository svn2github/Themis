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
//  Parser=new HTMLParser;
  View=new winview(Bounds(),"mainview",B_FOLLOW_ALL,B_WILL_DRAW|B_FRAME_EVENTS|B_ASYNCHRONOUS_CONTROLS|B_NAVIGABLE|B_NAVIGABLE_JUMP);
//  Parser->View=View;
//  View->Parser=Parser;
  AddChild(View);
 }
bool Win::QuitRequested()
 {
//  BMessenger *msgr=new BMessenger(NULL,Parser,NULL);
//  msgr->SendMessage(B_QUIT_REQUESTED);
//  delete msgr;
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
//the following bmessages create and clear a cache file as a test...
//    BMessenger *msgr=new BMessenger((BHandler*)PluginManager->FindPlugin(CachePlugin),NULL,NULL);
//     {
//      BMessage msg(CreateCacheObject);
//      BMessage reply;
//      msg.AddString("URL","http://themis.sourceforge.net");
//      msg.AddString("name","index.html");
//      msg.AddString("host","themis.sourceforge.net:80");
//      msg.AddString("path","/");
//      msg.AddString("mime","text/html");
//      msgr->SendMessage(&msg,&reply);
//      if (reply.what==B_OK)
//       {
//        printf("cachefile created.\n");
//        reply.PrintToStream();
//       }
//     }
//    msgr->SendMessage(ClearCache);
//    delete msgr;
    startup=false;
   }
 }
