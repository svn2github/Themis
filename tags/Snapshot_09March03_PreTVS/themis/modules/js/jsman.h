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
