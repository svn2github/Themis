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

#ifndef TESTPLUG_H
#define TESTPLUG_H
#include "plugclass.h"
extern "C" __declspec(dllexport)status_t Initialize(void *info=NULL);
extern "C" __declspec(dllexport)status_t Shutdown(bool now=false);
extern "C" __declspec(dllexport)PlugClass *GetObject(void);
#include <SupportDefs.h>
#include <Window.h>
#include <String.h>
#include <View.h>
#include <Bitmap.h>
#include "cacheplug.h"
class iview: public BView 
{
	public:
		iview(BRect frame,char *name,uint32 resize,uint32 flags);
		~iview();
		void Draw(BRect updr);
	
		BBitmap *image;
};
class imagewin: public BWindow  
{
	public:
		imagewin(char *title);
		~imagewin();
		iview *view;
		bool QuitRequested(){
			return true;
		}
		
	
};

struct iwind 
{
	imagewin *win;
	BString url;
	int32 objecttoken;
	iwind *next;
	iwind() {
		objecttoken=B_ERROR;
		win=NULL;
		next=NULL;
	}
	
};


class testplug: public PlugClass {
	private:
		iwind *whead;
	public:
		uint32 cache_user_token;
		CachePlug *CacheSys;
		testplug(BMessage *info=NULL);
		~testplug();
		void MessageReceived(BMessage *msg);
		bool IsHandler();
		BHandler *Handler();
		bool IsPersistent();
		uint32 PlugID();
		char *PlugName();
		float PlugVersion();
		void Heartbeat();
		status_t ReceiveBroadcast(BMessage *msg);
		int32 Type();
};

#endif

