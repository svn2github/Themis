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
#ifndef _cache_manager
#define _cache_manager
#include <StorageKit.h>
#include <SupportDefs.h>
//#include <Handler.h>
#include <Message.h>
#include "plugclass.h"
#include "cache_defines.h"
#include <Locker.h>
#include "cacheplug.h"
#include "diskcacheobject.h"
class cacheman: public CachePlug {
	private:
		BLocker *lock;
		int32 max_disk_cache_kb,max_ram_cache_kb;
		BPath cachepath;
		BDirectory *trashdir;
		CacheObject *objlist;
		CacheObject *FindObject(int32 objecttoken);
		CacheObject *FindObject(const char *URL);
		uint32 InternalCacheUserToken;
		CacheUser *FindUser(uint32 usertoken);
		BMessage *CSettings;//cache settings.
		BMessage *AppSettings,**ASp; //Application Settings and a pointer to the pointer 
									//holding the BMessage
		status_t FindCacheDir();
		status_t CheckIndices();
		status_t CheckMIME();
	public:
		cacheman(BMessage *info=NULL);
		~cacheman();
//		void MessageReceived(BMessage *msg);
		uint32 PlugID(){return PlugIDdef;};
		char *PlugName(){return PlugNamedef;};
		float PlugVersion(){return PlugVersdef;};
//		bool IsHandler();
//		BHandler *Handler();
		void Quit(bool fast=false);
		bool IsPersistent(){return true;}
		int32 Type();
		void Unregister(uint32 usertoken);
		status_t ReceiveBroadcast(BMessage *msg);
//		virtual uint32 Register();
//		virtual void Unregister(uint32 usertoken);
		virtual int32 FindObject(uint32 usertoken, const char *URL);
		virtual int32 CreateObject(uint32 usertoken, const char *URL, uint32 type=DISK_CACHE);
		virtual status_t UpdateAttr(uint32 usertoken, int32 objecttoken, const char *name, type_code type, void *data, size_t size);
		virtual void ClearRequests(uint32 usertoken, int32 objecttoken);
		virtual void ClearAllRequests(uint32 usertoken);
		void ClearCache(uint32 which=TYPE_ALL);
		virtual BMessage *GetInfo(uint32 usertoken, int32 objecttoken);
		virtual ssize_t Write(uint32 usertoken, int32 objecttoken, void *data, size_t size);
		virtual ssize_t Read(uint32 usertoken, int32 objecttoken, void *data, size_t size);
		virtual status_t WriteAttr(uint32 usertoken, int32 objecttoken, const char *attrname, type_code type,void *data,size_t size);
		virtual status_t ReadAttr(uint32 usertoken, int32 objecttoken, const char *attrname, type_code type, void *data, size_t size);
		virtual bool HasAttr(uint32 usertoken, int32 objecttoken, const char *name, type_code *type, size_t *size);
 		virtual ssize_t GetObjectSize(uint32 usertoken, int32 objecttoken);
		virtual ssize_t GetCacheSize(uint32 which=TYPE_ALL);
		virtual status_t SetObjectAttr(uint32 usertoken,int32 objecttoken, BMessage *info);
		virtual bool AcquireWriteLock(uint32 usertoken,int32 objecttoken);
		virtual void ReleaseWriteLock(uint32 usertoken, int32 objecttoken);
		void ClearContent(uint32 usertoken, int32 objecttoken);
};

#endif
