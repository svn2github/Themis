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
#ifndef _cache_user_class_
#define _cache_user_class_

#include <SupportDefs.h>
/*
	The CacheUser class is a support class which is never directly accessed by anything
other than the cache system. It stores information about the application component and/or
plug-in that wishes to use the cache system, as well as storing information read/write
position information for each cache object utilized. When the cache system creates a new
CacheObject object (either by searching and finding a desired URL or by creating an actual
file for a given URL), it is assigned one or more CacheUser objects. These objects are
copied from the original information obtained by registering with the cache system, and
each copy lives only as long as the user component needs the particular CacheObject or
the life of the CacheObject itself. Thus, this class has both a copy constructor as well
as the assignment operator =. 
*/
class CacheUser {
	private:
		uint32 id; //cache assigned id number for the component using the cache
		uint32 broadcast_id; //the broadcast system's target id number
		off_t readpos,writepos;//offsets of the various file pointers.
		CacheUser *next, *prev; //these are used as part of the linked list system
		char *name; //optional name of the component using the cache
	public:
		CacheUser(uint32 broadcast_target,uint32 usertoken=0, const char *Name=NULL);
		CacheUser(CacheUser *cu);
		~CacheUser();
		
		//the following five functions implement the linked list functionality of this class
		CacheUser *Next();
		CacheUser *SetNext(CacheUser *nextuser);
		CacheUser *Previous();
		CacheUser *SetPrevious(CacheUser *prevuser);
		void Remove(CacheUser *target); //remove the target object from the linked list, but don't delete it
		
		//these two functions deal with the user token/id; SetToken is not actually called
		//in the current implementation, though the function was added just in case.
		uint32 Token();
		uint32 SetToken(uint32 token);
		
		//these four functions manipulate the read and write offsets for whatever
		//CacheObject might be utilized with a copy of the original CacheUser object.
		off_t SetReadPosition(off_t offset);
		off_t ReadPosition();
		off_t SetWritePosition(off_t offset);
		off_t WritePosition();
		
		//the assignment operator = to quickly copy an instance of this class.
		void operator=(CacheUser &othercu);
		
		//the broadcast id of the user, for notification purposes.
		uint32 BroadcastID();
};

#endif

