/*
Copyright (c) 2001 Z3R0 One. All Rights Reserved.

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

#include "htmlparser.h"
#include "commondefs.h"
#include <stdio.h>
#include "nuresource.h"
#include "plugman.h"
#include <DataIO.h>
#include <File.h>
#include <Entry.h>
extern plugman *PluginManager;

HTMLParser::HTMLParser()
	:BLooper("html parser",B_DISPLAY_PRIORITY){
	Run();
	BMessenger *msgr=new BMessenger(PluginManager);
		BMessage *msg=new BMessage(AddInitInfo);
		msg->AddPointer("html_parser",this);
		msgr->SendMessage(msg);
		delete msgr;
		delete msg;
		
}
HTMLParser::~HTMLParser(){
}
bool HTMLParser::QuitRequested(){
	printf("HTMLParser quitting\n");
	return true;
}
void HTMLParser::MessageReceived(BMessage *msg){
	switch(msg->what){
		case ProtocolResponse: {
			printf("HTML Parser!!!\n");
			msg->PrintToStream();
			BMessage cacheinfo;
			if (msg->HasMessage("cache_info")) {
				
				msg->FindMessage("cache_info",&cacheinfo);
				printf("Cache info BMessage\n");
				cacheinfo.PrintToStream();
			}
			
			BPositionIO *data=NULL;
			if ((!cacheinfo.IsEmpty()) || (msg->HasPointer("data_posit_io_ptr"))) {
				if (msg->HasPointer("data_posit_io_ptr")) {
					
					msg->FindPointer("data_posit_io_ptr",(void**)&data);
				} 
					if ((data==NULL) && (!cacheinfo.IsEmpty())) {
						entry_ref ref;
						cacheinfo.FindRef("ref",&ref);
						data=new BFile(&ref,B_READ_ONLY);
						
					}
					
				
				
				printf("Data Pointer: %p\n",data);
				data->Seek(0,SEEK_END);
				off_t fsize=data->Position();
				data->Seek(0,SEEK_SET);
				unsigned char *buff=new unsigned char[fsize+1];
				memset(buff,0,fsize+1);
				data->Read(buff,fsize);
				printf("data in buffer:\n%s\n",(char*)buff);
				delete buff;
			}
			
			
		}break;
		default:{
			BLooper::MessageReceived(msg);
		}
	}
}
