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
#include <Button.h>
#include "protocol_plugin.h"
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
  r=Bounds();
  r.top=r.bottom-15;
  r.right=r.left+30;
  msg=new BMessage(B_CANCEL);
  AddChild(locline);
  stopbutton=new BButton(r,"cancelbutton","Stop",msg);
  stopbutton->SetEnabled(false);
  AddChild(stopbutton);
  
 }
void winview::AttachedToWindow()
 {
  stopbutton->SetTarget(this);
  locline->SetTarget(this);
 }
void winview::MessageReceived(BMessage *msg)
 {
  switch(msg->what)
   {
    case B_CANCEL:
     {
      printf("Ordering protocol to stop\n");
      protocol_plugin *pobj=(protocol_plugin*)PluginManager->FindPlugin(protocol);
      if (pobj!=NULL)
       pobj->Stop();
       status_t stat;
      wait_for_thread(pobj->Thread(),&stat);
      stopbutton->SetEnabled(false);
     }break;
    case OpenLocation:
     {
      stopbutton->SetEnabled(true);
      msg->PrintToStream();
      BString url=locline->Text();
      protocol=HTTPPlugin;
      if (url.Length()>0)
       {
        int32 pos=url.FindFirst("://");
        if ((pos>0) && (pos<6))
         {
           if (url.IFindFirst("https://")==0)
            {
             protocol=HTTPSPlugin;
             goto ContOpenLocation;
            }
           if (url.IFindFirst("http://")==0)
            {
             protocol=HTTPPlugin;
             goto ContOpenLocation;
            }
         }
        else
         {
          if (pos==-1)
           {
            BString tmp;
            tmp << "http://" << url.String();
            url=tmp;
            locline->SetText(url.String());
           }
         }
       }
      ContOpenLocation:
      ProtocolPlugClass *pobj=(ProtocolPlugClass*)PluginManager->FindPlugin(protocol);
      printf("pobj: %p\n",pobj);
      if (url.Length()==0)
       {
        return;
       }
      if (pobj!=NULL)
       {
        BMessage *info=new BMessage;
        info->AddPointer("top_view",this);
        info->AddPointer("window",Window());
        info->AddPointer("parser",Parser);
        info->AddPointer("plug_manager",PluginManager);
        info->AddString("target_url",url.String());
        printf("info: %p\n",info);
        info->PrintToStream();
        pobj->SpawnThread(info);
        pobj->StartThread();
//        delete info;
//    snooze(5000000);
//  port_id httpport=find_port("http_protocol_port");
//  BMessage *msg=new BMessage(FetchItem);
//  msg->AddString("target_url","http://127.0.0.1");
//  size_t size=msg->FlattenedSize();
//  char *data=new char[size+1];
//  memset(data,0,size+1);
//  msg->Flatten(data,size);
//  delete msg;
//  for (int32 i=0; i<1000;i++)
//   {
//    printf("writing message #%ld to port.\n",i);
//    write_port(httpport,FetchItem,data,size);
//   }
//  delete data;
//        pobj->SetURL(url.String());
        
//        obj->GetURL();
       }
     }break;
    default:
     BView::MessageReceived(msg);
   }
 }
