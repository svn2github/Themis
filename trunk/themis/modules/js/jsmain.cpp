#include "jsdefs.h"
#include "jsmain.h"
#include "jsman.h"
#include <Message.h>
jsman *JSMan;
BMessage **AppSettings_p;
BMessage *AppSettings;

BMessage *JSSettings;

status_t Initialize(void *info) {
	JSMan=NULL;
  AppSettings=NULL;
	JSSettings=NULL;
	 
  if (info!=NULL) {
	  
	BMessage *imsg=(BMessage *)info;
	if (imsg->HasPointer("settings_message_ptr")) {
		
		imsg->FindPointer("settings_message_ptr",(void**)&AppSettings_p);
		AppSettings=*AppSettings_p;
		
	}
	
  }
 	if ((*AppSettings_p)!=NULL) {
		if (AppSettings->HasMessage("javascript_settings_message")) {
			JSSettings=new BMessage;
			AppSettings->FindMessage("javascript_settings_message",JSSettings);
		}
	}
 if (JSSettings==NULL) {
  	JSSettings=new BMessage;
//Add Default Settings
	 
  }
	JSMan=new jsman((BMessage*)info);
	return B_OK;
	
}
status_t Shutdown(bool now) {
//	if (now)
 	printf("1234 Javascript Being Shutdown\n");
	 if ((*AppSettings_p)!=NULL) {
		 
	if (!AppSettings->HasMessage("javascript_settings_message")) {
		AppSettings->AddMessage("javascript_settings_message",JSSettings);
	} else
		AppSettings->ReplaceMessage("javascript_settings_message",JSSettings);
	 }
	 delete JSSettings;
	
		if (JSMan!=NULL)
			delete JSMan;
	return B_OK;
	
}
PlugClass *GetObject(void) {
	return JSMan;
	
}
