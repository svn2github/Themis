#include "jsman.h"
#include "jsdefs.h"
#include "commondefs.h"
#include <stdio.h>
#include <string.h>

extern BMessage *AppSettings;
extern BMessage *JSSettings;


JSClass globalClass= {
	"Global",0,JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub
};


jsman::jsman(BMessage *info)
	:BHandler("javascript_manager"),PlugClass(info) {
	js_enabled=false;
	script_head=NULL;
	JSSettings->FindBool("js_enabled",&js_enabled);
	uses_heartbeat=true;
	rt=JS_NewRuntime(8L*1024L*1024L);
	version = JSVERSION_DEFAULT;
		
	if (!rt) {
		printf("Javascript initialiation failed.\n");
		return;
	}
	cx=JS_NewContext(rt,8192);
	if (cx==NULL) {
		printf("Unable to create Javascript context.\n");
		return;
	}
	glob=JS_NewObject(cx,&globalClass,NULL,NULL);
	builtins=JS_InitStandardClasses(cx,glob);
	printf("JSMan: end of constructor. rt= %p\n",rt);
	printf("Javascript Implementation Version: %s\n",JS_GetImplementationVersion());
		
	printf("performing simple Javascript test\n");
	char *script="var today=Date();today.toString();\0";
	jsval rval;
		uintN lineno=0;
		JSBool ok=JS_EvaluateScript(cx,glob,script,strlen(script),"script",lineno,&rval);
		
		if (ok==JS_TRUE) {
			JSString *str=JS_ValueToString(cx,rval);
			
			printf("Script executed properly: %s\n",JS_GetStringBytes(str));
			
		} else {
			printf("Script failed.\n");
		}
		
}

jsman::~jsman() {
	if (JSSettings->HasBool("js_enabled")) {
		JSSettings->ReplaceBool("js_enabled",js_enabled);
	} else {
		JSSettings->AddBool("js_enabled",js_enabled);
	}
	if (script_head) {
		jsscripts_st *cur;
		while (script_head!=NULL) {
			cur=script_head->next;
			delete script_head;
			script_head=cur;
		}
	}
	
	JS_DestroyContext(cx);
	JS_DestroyRuntime(rt);
	
}

void jsman::MessageReceived(BMessage *msg) {
	printf("JS_Man has received a BMessage\n");
	
	switch(msg->what) {
		case DisableJavascript: {
			if (js_enabled)
				js_enabled=false;
		}break;
		case EnableJavascript: {
			if (!js_enabled)
				js_enabled=true;
		}break;
		case GetJavascriptStatus: {
			BMessage reply;
			reply.AddBool("js_enabled",js_enabled);
			msg->SendReply(&reply);
		}break;
		case ProcessJSScript: {
			if (js_enabled) {
			}
		}break;
		case ClearJSScripts: {
			if (script_head) {
				jsscripts_st *cur;
				while (script_head!=NULL) {
					cur=script_head->next;
					delete script_head;
					script_head=cur;
				}
			}
		}break;
		default:
			BHandler::MessageReceived(msg);
	}
}
bool jsman::IsHandler() {
	return true;
}
BHandler * jsman::Handler() {
	return this;
}
bool jsman::IsPersistent() {
	return true;
}
void jsman::Heartbeat() {
	printf("JS_Man's heartbeat\n");	
}
status_t jsman::ReceiveBroadcast(BMessage *msg){
	int32 command=0;
	msg->FindInt32("command",&command);
	switch(command) {
		case COMMAND_INFO_REQUEST: {
			if (msg->HasBool("supportedscripttypes")) {
				//reply with javascript as the supported type.
			}
			
		}break;
		default: {
			printf("JS addon doesn't handle this broadcast.\n");
			msg->PrintToStream();
			return PLUG_DOESNT_HANDLE;
		}
		
	}
	return PLUG_HANDLE_GOOD;
}

int32 jsman::Type(){
	return JAVASCRIPT_HANDLER;
}

