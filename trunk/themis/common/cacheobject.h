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
#ifndef _cache_object_class_
#define _cache_object_class_

#include <SupportDefs.h>
#include "cacheuser.h"
/*
	The CacheObject class represents the actual cached file, whether it is on disk
or in RAM or stored else-where (potentially even across a network). It contains the actual
data that will be utilized by other parts of the application even though it is not
directly accessible to those parts. 
*/
class CacheObject {
	protected:
		CacheUser *userlist;
		CacheUser *writelockowner;
		char *url;
		int32 id;
		CacheObject *next,*prev;
		CacheUser *FindUser(uint32 usertoken);
	public:
		CacheObject(int32 token,const char *URL);
		virtual ~CacheObject();
		virtual bool IsUsedBy(uint32 usertoken);
		virtual void RemoveUser(uint32 usertoken);
		virtual void AddUser(uint32 usertoken);
		virtual void AddUser(CacheUser *user);
		virtual bool AcquireWriteLock(uint32 usertoken);
		virtual bool HasWriteLock(uint32 usertoken);
		virtual void ReleaseWriteLock(uint32 usertoken);
		virtual ssize_t Read(uint32 usertoken, void *buffer, size_t size);
		virtual ssize_t Write(uint32 usertoken, void *buffer, size_t size);
		virtual off_t Size()=0;
		CacheObject *Next();
		void SetNext(CacheObject *nobject);
		CacheObject *Previous();
		void SetPrevious(CacheObject *pobject);
		int32 Token();
		int32 SetToken(int32 objecttoken);
		virtual int32 CountUsers();
		virtual const char *URL();
		virtual void SetURL(const char *URL);
		virtual void Remove(CacheObject *target);
		virtual void ClearFile()=0;
		virtual void ClearContent(uint32 usertoken)=0;
		virtual uint32 Type()=0;
		virtual BMessage *GetInfo()=0;
		virtual ssize_t WriteAttr(uint32 usertoken, const char *attrname, type_code type,void *data,size_t size)=0;
		virtual ssize_t ReadAttr(uint32 usertoken,  const char *attrname, type_code type, void *data, size_t size)=0;
};

#endif

