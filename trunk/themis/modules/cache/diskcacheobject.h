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
#ifndef _disk_cache_object_
#define _disk_cache_object_

#include "cacheobject.h"
#include <File.h>
#include <Message.h>
#include <Entry.h>
class DiskCacheObject: public CacheObject {
	private:
		entry_ref ref;
		BFile *file;
		void OpenFile();
		void CloseFile();
		bool refset;
	public:
		DiskCacheObject(int32 objecttoken, const char *URL);
		DiskCacheObject(int32 objecttoken, const char *URL, entry_ref aref);
		~DiskCacheObject();
		void SetRef(entry_ref aref);
		ssize_t Read(uint32 usertoken, void *buffer, size_t size);
		ssize_t Write(uint32 usertoken, void *buffer, size_t size);
		ssize_t SetLength(uint32 usertoken, int32 objecttoken, size_t length);
		ssize_t WriteAttr(uint32 usertoken, const char *attrname, type_code type,void *data,size_t size);
		ssize_t ReadAttr(uint32 usertoken,  const char *attrname, type_code type, void *data, size_t size);
		status_t RemoveAttr(uint32 usertoken, const char *attrname);
		void ClearContent(uint32 usertoken);
		BMessage *GetInfo();
		void ClearFile();
		uint32 Type();
		off_t Size();
};

#endif

