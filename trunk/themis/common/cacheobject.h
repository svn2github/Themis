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
#include <Message.h>
#include "cacheuser.h"
#include <Locker.h>
#include <DataIO.h>
/*!
\brief Object Oriented linked list representation of cached data.
	The CacheObject class represents the actual cached file, whether it is on disk
or in RAM or stored else-where (potentially even across a network). It contains the actual
data that will be utilized by other parts of the application even though it is not
directly accessible to those parts.
*/
class CacheObject {
	protected:
		//! Object oriented linked list containing cache user information.
		CacheUser *userlist;
		//! The cache user object that has write permission on this particular cache object.
		CacheUser *writelockowner;
		//! The URL of this particular cache object.
		char *url;
		//! A unique identifier for this cache object.
		int32 id;
		//! Pointers to the next and previous items in the linked list respectively.
		CacheObject *next,*prev;
		//! Finds the user object of a particular id if that user is accessing this object.
		CacheUser *FindUser(uint32 usertoken);
		BLocker *lock;
	public:
		//! The constructor.
		CacheObject(int32 token,const char *URL);
		//! The destructor.
		virtual ~CacheObject();
		virtual BPositionIO *IOPointer()=0;
	
		//! Determines if this cache object is used by a particular cache user object.
		virtual bool IsUsedBy(uint32 usertoken);
		//! Removes a particular cache user token from the CacheUser linked list.
		virtual void RemoveUser(uint32 usertoken);
		//! Adds a particular cache user token to the CacheUser linked list.
		virtual void AddUser(uint32 usertoken);
		//! Copies an existing CacheUser object, and adds it to the CacheUser linked list.
		virtual void AddUser(CacheUser *user);
		//! This function, if successful, gives the requesting cache user write permission on this cache object.
		virtual bool AcquireWriteLock(uint32 usertoken);
		//! This function checks to see if the specified user token has permission to write.
		virtual bool HasWriteLock(uint32 usertoken);
		//! This function lets the cache user that currently owns the write rights, to yield them to another object.
		virtual void ReleaseWriteLock(uint32 usertoken);
		//! Read data from this cache item.
		virtual ssize_t Read(uint32 usertoken, void *buffer, size_t size);
		//! Write data to this cache item.
		virtual ssize_t Write(uint32 usertoken, void *buffer, size_t size);
		virtual ssize_t SetLength(uint32 usertoken, int32 objecttoken, size_t length);
	
		//! Get the size of this cache item.
		virtual off_t Size()=0;
		//! Get the next cache object from the linked list.
		CacheObject *Next();
		//! Set the next cache object in the linked list.
		void SetNext(CacheObject *nobject);
		//! Get the previous cache object from the linked list.
		CacheObject *Previous();
		//! Set the previous cache object in the linked list.
		void SetPrevious(CacheObject *pobject);
		//! Get the cache object token.
		int32 Token();
		//! Set the cache object token.
		/*!
			This function will set the value of the cache object token.
		
			\note (z3r0_one) I don't believe I actually use this function any where, but
			but I wanted to add it to make sure that if I needed it, it would be there.
		*/
		int32 SetToken(int32 objecttoken);
		//! Counts the number of users that this cache object has.
		virtual int32 CountUsers();
		virtual CacheUser *GetUser(int32 which);
	
		//! Returns the URL of this particular cache object.
		virtual const char *URL();
		//! Set the URL for which this object contains data.
		virtual void SetURL(const char *URL);
		//! Remove the object from the linked list.
		virtual void Remove(CacheObject *target);
		//! Remove the cache object from it's medium.
		virtual void ClearFile()=0;
		//! Empty the file of its content, but do not remove it from its medium.
		virtual void ClearContent(uint32 usertoken)=0;
		//! Returns whether the cache object is on disk or in RAM.
		virtual uint32 Type()=0;
		//! Returns information about the cache object.
		virtual BMessage *GetInfo()=0;
		//! Write data to an attribute.
		virtual ssize_t WriteAttr(uint32 usertoken, const char *attrname, type_code type,void *data,size_t size)=0;
		//! Read data from an attribute.
		virtual ssize_t ReadAttr(uint32 usertoken,  const char *attrname, type_code type, void *data, size_t size)=0;
};

#endif

