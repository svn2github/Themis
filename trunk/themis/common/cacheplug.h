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

#ifndef _cache_plug_base_class_
#define _cache_plug_base_class_

#include "plugclass.h"
#include <Message.h>
#include <SupportDefs.h>
#include "cacheuser.h"
#include <Locker.h>
#include <Autolock.h>


//!The basis for any and all of Themis' cache systems.
/*!
	CachePlug is the basis for any and all cache plug-ins to be added to the system. This
base class, when compiled and included (by linking) in any component, can give generic
access to any and all cache systems based on it, without needing to link against any
particular system.
*/
class CachePlug:public PlugClass {
	protected:
		uint32 user_token_value; //!< the current user token value; increments with each call to Register
		int32 object_token_value; //!< the current cache object token value; increments when requested URLs are found or cache items are created
		CacheUser *userlist;//!< the linked list of registered cache users
		BLocker *lock;
		BAutolock *autolock;
	public:
		CachePlug(BMessage *info=NULL);
		virtual ~CachePlug();
		// the register and unregister functions; which keep track of users and resources.
		virtual uint32 Register(uint32 broadcast_target,const char *name=NULL);
		virtual void Unregister(uint32 usertoken);
		
		//these three functions find, create, and destroy cache items.
		virtual int32 FindObject(uint32 usertoken, const char *URL);
		virtual int32 CreateObject(uint32 usertoken, const char *URL, uint32 type=TYPE_DISK);
		virtual void ClearCache(uint32 which=TYPE_ALL); //clear cache should not be touched by anything other than the settings view.
		
		//These six functions deal with the cache object's attributes: reading and writing an individual attribute in the middle two functions,
		//setting and retrieving all attributes simultaneously in the next two. The UpdateAttribute function will likely be removed
		//before the cache system is added to the cvs repository. Currently, only SetObjectAttr and GetInfo are implemented.
		virtual status_t UpdateAttr(uint32 usertoken, int32 objecttoken, const char *name, type_code type, void *data, size_t size);
		virtual status_t WriteAttr(uint32 usertoken, int32 objecttoken, const char *attrname, type_code type,void *data,size_t size)=0;
		virtual status_t ReadAttr(uint32 usertoken, int32 objecttoken, const char *attrname, type_code type, void *data, size_t size)=0;
		virtual status_t SetObjectAttr(uint32 usertoken,int32 objecttoken, BMessage *info)=0;
		virtual BMessage *GetInfo(uint32 usertoken, int32 objecttoken);
		virtual bool HasAttr(uint32 usertoken, int32 objecttoken, const char *name, type_code *type, size_t *size)=0;
		
		//These two functions write and read data in the specified cache object.
		virtual ssize_t Write(uint32 usertoken, int32 objecttoken, void *data, size_t size);
		virtual ssize_t Read(uint32 usertoken, int32 objecttoken, void *data, size_t size);

		//! returns the current object size, not counting its attributes
		virtual ssize_t GetObjectSize(uint32 usertoken, int32 objecttoken)=0;
		
		//! removes the contents (the data) of the file, setting its file size to zero.
		virtual void ClearContent(uint32 usertoken, int32 objecttoken)=0;
		
		//removes the CacheUser object for the specified user from one or all CacheObjects.
		//to access the data again, the cache user must do a FindObject with the object's URL.
		virtual void ClearRequests(uint32 usertoken, int32 objecttoken);
		virtual void ClearAllRequests(uint32 usertoken);
		
		//! get the total size of the files in the cache system specified.
		virtual ssize_t GetCacheSize(uint32 which=TYPE_ALL)=0;

		//! manipulation of the write lock privilleges for any given object.
		virtual bool AcquireWriteLock(uint32 usertoken,int32 objecttoken)=0;
		virtual void ReleaseWriteLock(uint32 usertoken, int32 objecttoken)=0;
};

#endif
