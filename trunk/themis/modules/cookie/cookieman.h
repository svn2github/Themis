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
/*!
\file 
This file contains the declaration of the cookie manager application/add-on. This
system allows cookies to be edited outside of Themis, and configures the cookie settings
with-in Themis.
*/
#ifndef cookie_man
#define cookie_man

#include "plugclass.h"
#include "cookiedefs.h"
#include <Handler.h>
#include <Message.h>
#include <SupportDefs.h>
#include <StorageKit.h>
 
class cookieman: public BHandler, public PlugClass {
	private:
	public:
		cookieman(BMessage *info=NULL);
		~cookieman();
		void MessageReceived(BMessage *msg);
		bool IsHandler();
		BHandler *Handler();
		bool IsPersistent();
		uint32 PlugID(){return PlugIDdef;};
		char *PlugName(){return PlugNamedef;};
		float PlugVersion(){return PlugVersdef;};
//		bool RequiresHeartbeat(){return true;};
		void Heartbeat();
		status_t ReceiveBroadcast(BMessage *msg);
		int32 Type();
};


#endif
