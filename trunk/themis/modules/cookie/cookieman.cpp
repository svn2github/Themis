/*
Copyright (c) 2002 Raymond "Z3R0 One" Rodgers. All Rights Reserved. 

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

Original Author & Project Manager: Raymond "Z3R0 One" Rodgers (z3r0_one@yahoo.com)
Project Start Date: October 18, 2000
*/
#include "cookieman.h"
#include "cookiedefs.h"
#include "commondefs.h"
#include "plugman.h"
#include <stdio.h>
cookieman::cookieman(BMessage *info)
	:BHandler("cookie_manager"),PlugClass(info,"cookie manager admin") {
	uses_heartbeat=false;
		
}

cookieman::~cookieman() {
}

void cookieman::MessageReceived(BMessage *msg) {
	printf("CookieMonster has received a BMessage\n");
	
	switch(msg->what) {
		default:
			BHandler::MessageReceived(msg);
	}
}
bool cookieman::IsHandler() {
	return true;
}
BHandler * cookieman::Handler() {
	return this;
}
bool cookieman::IsPersistent() {
	return false;
}
void cookieman::Heartbeat() {
	printf("CookieMonster's heartbeat\n");	
}
status_t cookieman::ReceiveBroadcast(BMessage *msg){
	int32 command=0;
	msg->FindInt32("command",&command);
	switch(command) {
		case COMMAND_INFO_REQUEST: {
			int32 replyto=0;
			msg->FindInt32("ReplyTo",&replyto);
			switch(msg->what) {
				case GetSupportedMIMEType: {
					
			if (msg->HasBool("supportedmimetypes")) {
				BMessage types(SupportedMIMEType);
				types.AddString("mimetype","text/plain");
				types.AddInt32("command",COMMAND_INFO);
				BMessage container;
				container.AddMessage("message",&types);
				PlugMan->Broadcast(PlugID(),replyto,&container);
			}
				}break;
			}
			
		}break;
		default: {
			msg->PrintToStream();
			return PLUG_DOESNT_HANDLE;
		}
	
	}
	return PLUG_HANDLE_GOOD;
}
uint32 cookieman::BroadcastTarget() 
{
	return MS_TARGET_UNKNOWN;
}
status_t cookieman::BroadcastReply(BMessage *msg) 
{
	return B_OK;
}

int32 cookieman::Type(){
	return COOKIE_HANDLER;
}
