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
#ifndef _image_handler_
#define _image_handler_

#include <TranslationKit.h>
#include <Message.h>
#include <View.h>
#include "plugclass.h"
#include "smt.h"
#include "image.h"
#include "cacheplug.h"

extern "C" __declspec(dllexport)status_t Initialize(void *info=NULL);
extern "C" __declspec(dllexport)status_t Shutdown(bool now=false);
extern "C" __declspec(dllexport)PlugClass *GetObject(void);


class ImageMan: public PlugClass {
	private:
		BTranslatorRoster *TRoster;
		translator_id *translators;
		int32 transcount;
		translator_info *imagetrans;
		int32 imagetranscount;
		const char *TranslatorName(translator_id id);
		bool TypeSupported(char *type);
		smt_st *AddMIMEType(char *type);
		smt_st *mimes;
		Image *imagelist;
		uint32 cache_user_token;
		CachePlug *CacheSys;
		
	public:
		ImageMan(BMessage *info=NULL);
		~ImageMan();
		bool IsPersistent();
		uint32 PlugID();
		char *PlugName();
		float PlugVersion();
		status_t ReceiveBroadcast(BMessage *msg);
		status_t BroadcastReply(BMessage *msg);
		uint32 BroadcastTarget();
		int32 Type();
		char *SettingsViewLabel();
		BView *SettingsView();
};

#endif

