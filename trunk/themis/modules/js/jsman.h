#ifndef javascript_manager
#define javascript_manager

#include "plugclass.h"
#include "jsdefs.h"
#include <Handler.h>
#include <Message.h>
#include <SupportDefs.h>
#include <StorageKit.h>

#include "jsapi.h"

struct jsscripts_st {
	JSContext *context;
	jsscripts_st *next;
	jsscripts_st() {
		context=NULL;
		next=NULL;
	}
};



class jsman: public BHandler, public PlugClass {
	private:
		JSVersion version;
		JSRuntime *rt;
		JSObject *glob, *it;
		JSBool builtins;
		JSContext *cx;
		bool js_enabled;
		jsscripts_st *script_head;
	public:
		jsman(BMessage *info=NULL);
		~jsman();
		void MessageReceived(BMessage *msg);
		bool IsHandler();
		BHandler *Handler();
		bool IsPersistent();
		uint32 PlugID(){return PlugIDdef;};
		char *PlugName(){return PlugNamedef;};
		float PlugVersion(){return PlugVersdef;};
		void Heartbeat();
		status_t ReceiveBroadcast(BMessage *msg);
		int32 Type();
};

#endif
