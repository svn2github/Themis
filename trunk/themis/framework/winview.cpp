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
#include "winview.h"
#define OpenLocation 'oloc'
#include <Handler.h>
#include <stdio.h>
extern plugman *PluginManager;

winview::winview(BRect frame,const char *name,uint32 resizem,uint32 flags)
        :BView(frame,name,resizem,flags)
 {
  SetViewColor(216,216,216);
  BRect r(Bounds());
  r.bottom=19;
  BMessage *msg=new BMessage(OpenLocation);
  locline=new BTextControl(r,"locationline","Location:",NULL,msg,B_FOLLOW_TOP|B_FOLLOW_LEFT_RIGHT,B_WILL_DRAW|B_FRAME_EVENTS|B_NAVIGABLE);
  locline->SetDivider(50);
  AddChild(locline);
  
 }
void winview::AttachedToWindow()
 {
  locline->SetTarget(this);
 }
void winview::MessageReceived(BMessage *msg)
 {
  switch(msg->what)
   {
    case OpenLocation:
     {
      msg->PrintToStream();
      protocol_plugin *pobj=(protocol_plugin*)PluginManager->FindPlugin(HTTPPlugin);
      printf("pobj: %p\n",pobj);
      if (strlen(locline->Text())==0)
       {
        return;
       }
      if (pobj!=NULL)
       printf("page: %s\n",pobj->GetURL(locline->Text()));
      
     }break;
    default:
     BView::MessageReceived(msg);
   }
 }