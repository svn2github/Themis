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