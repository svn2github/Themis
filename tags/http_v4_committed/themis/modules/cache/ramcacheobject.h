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
#ifndef _ram_cache_object_
#define _ram_cache_object_

#include "cacheobject.h"
#include <DataIO.h>
#include <Message.h>
class RAMCacheObject: public CacheObject {
	private:
		BMallocIO *databuffer;
		char *name, *host, *mimetype, *path, *etag, *lastmodified,*expires,*contentmd5;
		off_t contentlength;
	public:
		RAMCacheObject(int32 token,const char *URL);
		~RAMCacheObject();
		BPositionIO *IOPointer();
		virtual BMessage *GetInfo();
		ssize_t Read(uint32 usertoken, void *buffer, size_t size);
		ssize_t Write(uint32 usertoken, void *buffer, size_t size);
		ssize_t SetLength(uint32 usertoken, int32 objecttoken, size_t length);
		virtual ssize_t WriteAttr(uint32 usertoken, const char *attrname, type_code type,void *data,size_t size);
		virtual ssize_t ReadAttr(uint32 usertoken,  const char *attrname, type_code type, void *data, size_t size);
		status_t RemoveAttr(uint32 usertoken, const char *attrname);
		void ClearContent(uint32 usertoken);
		virtual off_t Size();
		void ClearFile();
		uint32 Type();
};



#endif

