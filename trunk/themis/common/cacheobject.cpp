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
#include "cacheobject.h"
#include <string.h>
CacheObject::CacheObject(int32 token,const char *URL) {
	lock=new BLocker(true);
	id=token;
	if (URL!=NULL) {
	userlist=NULL;
	writelockowner=NULL;
	url=new char[strlen(URL)+1];
	memset(url,0,strlen(URL)+1);
	strcpy(url,URL);
	} else
	 url=NULL;
	 next=prev=NULL;
}
CacheObject::~CacheObject() {
	if (url!=NULL) {
		memset(url,0,strlen(url)+1);
		delete url;
	}
	id=0;
	if (writelockowner!=NULL)
		writelockowner=NULL;
	if (userlist!=NULL) {
		CacheUser *cur=NULL;
		while (userlist!=NULL) {
			cur=userlist->Next();
			delete userlist;
			userlist=cur;
		}
	}
	next=prev=NULL;
	delete lock;
	lock=NULL;
}
bool CacheObject::IsUsedBy(uint32 usertoken) {
	bool found=false;
	CacheUser *cur=userlist;
	while (cur!=NULL) {
		if (cur->Token()==usertoken) {
			found=true;
			break;
		}
		cur=cur->Next();
	}
	return found;
}
void CacheObject::RemoveUser(uint32 usertoken) {
	CacheUser *cur=userlist;
	while (cur!=NULL) {
		if (cur->Token()==usertoken) {
			if (cur->Previous()!=NULL) {
				cur->Previous()->SetNext(cur->Next());
				if (writelockowner==cur) {
					writelockowner=NULL;
				}
				
				delete cur;
			} else {
				userlist=cur->Next();
				if (userlist!=NULL)
					userlist->SetPrevious(NULL);
				if (writelockowner==cur) {
					writelockowner=NULL;
				}
				delete cur;
			}
			break;
		}
		cur=cur->Next();
	}
}
void CacheObject::AddUser(uint32 usertoken) {
	if (userlist==NULL) {
		userlist=new CacheUser(usertoken);
	} else {
		if (!IsUsedBy(usertoken))
			userlist->SetNext((new CacheUser(usertoken)));
	}
}
void CacheObject::AddUser(CacheUser *user) 
{
	if (userlist==NULL) {
		userlist=new CacheUser(user);
	} else {
		if (!IsUsedBy(user->Token()))
			userlist->SetNext((new CacheUser(user)));
	}
	
	
}

bool CacheObject::AcquireWriteLock(uint32 usertoken) {
	bool successful=false;
	if (writelockowner==NULL) {
		CacheUser *cur=userlist;
		while (cur!=NULL) {
			if (cur->Token()==usertoken) {
				successful=true;
				writelockowner=cur;
				break;
			}
			cur=cur->Next();
		}
	} else {
		if (writelockowner->Token()==usertoken)
			successful=true;
	}
	
	return successful;	
}
void CacheObject::ReleaseWriteLock(uint32 usertoken) {
	if (writelockowner!=NULL) {
		if (writelockowner->Token()==usertoken) {
			writelockowner=NULL;
		}
	}
}
ssize_t CacheObject::Read(uint32 usertoken, void *buffer, size_t size) {
	ssize_t bytesread=0;
	return bytesread;
}
ssize_t CacheObject::Write(uint32 usertoken, void *buffer, size_t size) {
	ssize_t byteswritten=0;
	if (writelockowner!=NULL) {
		if (writelockowner->Token()==usertoken) {
		}
	}
	return byteswritten;
}
ssize_t CacheObject::SetLength(uint32 usertoken, int32 objecttoken, size_t length)
{
	ssize_t size=0;
	if (writelockowner!=NULL) {
		if (writelockowner->Token()==usertoken) {
		}
	}
	return size;
}

CacheObject *CacheObject::Next() 
{
	return next;
}
void CacheObject::SetNext(CacheObject *nobject) 
{
	if (next==NULL) {
		next=nobject;
		if (next!=NULL)
			next->SetPrevious(this);
	} else {
		next->SetNext(nobject);
	}
	
	
}
CacheObject *CacheObject::Previous() 
{
	return prev;
}
void CacheObject::SetPrevious(CacheObject *pobject) 
{
	prev=pobject;
}

int32 CacheObject::Token() 
{
	return id;
}
int32 CacheObject::SetToken(int32 objecttoken) 
{
	id=objecttoken;
	return id;
}
int32 CacheObject::CountUsers() 
{
	int32 count=0;
	CacheUser *cur=userlist;
	while (cur!=NULL) {
		count++;
		cur=cur->Next();
	}
	return count;
}
CacheUser *CacheObject::GetUser(int32 which) 
{
	if (which>=CountUsers())
		return NULL;
	CacheUser *user=userlist;
	for (int32 i=0; i<which; i++)
		user=user->Next();
	return user;
	
}

CacheUser *CacheObject::FindUser(uint32 usertoken) 
{
	CacheUser *cur=userlist;
	while (cur!=NULL) {
		if (cur->Token()==usertoken) {
			break;
		}
		cur=cur->Next();
	}
	return cur;
}
const char *CacheObject::URL() 
{
	return (const char*)url;
}
void CacheObject::SetURL(const char *URL) 
{
	if (URL==NULL) {
		if (url!=NULL) {
			memset(url,0,strlen(url)+1);
			delete url;
			url=NULL;
		}
	}else {
		if (url!=NULL) {
			memset(url,0,strlen(url)+1);
			delete url;
			url=NULL;
		}
		url=new char[strlen(URL)+1];
		memset(url,0,strlen(URL)+1);
		strcpy(url,URL);
	}
}
void CacheObject::Remove(CacheObject *target) 
{
	if (next==NULL)
		return;
	if (next==target) {
		next=next->Next();
	} else 
		next->Remove(target);
	
}
bool CacheObject::HasWriteLock(uint32 usertoken) 
{
	if (writelockowner!=NULL) {
		if (writelockowner->Token()==usertoken)
			return true;
	}
	return false;
}


