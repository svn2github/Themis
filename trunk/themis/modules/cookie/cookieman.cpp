#include "cookieman.h"
#include "cookiedefs.h"
#include "commondefs.h"
#include "plugman.h"
#include <stdio.h>
cookieman::cookieman(BMessage *info)
	:BHandler("cookie_manager"),PlugClass(info) {
	uses_heartbeat=true;
		
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
				PlugMan->Broadcast(replyto,&container);
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

int32 cookieman::Type(){
	return COOKIE_HANDLER;
}
