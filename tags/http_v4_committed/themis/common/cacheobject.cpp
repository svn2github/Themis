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
#include <Autolock.h>
CacheObject::CacheObject(int32 token,const char *URL) {
//	lock=new BLocker(true);
	id=token;
	creation_time=last_access_time=real_time_clock();
	
	if (URL!=NULL) {
	userlist=NULL;
	writelockowner=NULL;
	url=new char[strlen(URL)+1];
	memset(url,0,strlen(URL)+1);
	strcpy(url,URL);
	} else
	 url=NULL;
	 next=prev=NULL;
	 ulist=new BList();
	
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
	creation_time=last_access_time=0;
	next=prev=NULL;
	delete ulist;
	
//	delete lock;
//	lock=NULL;
}
bool CacheObject::IsUsedBy(uint32 usertoken) {
	BAutolock alock(lock);
	bool found=false;
	if (alock.IsLocked()) {
		CacheUser *cur;
		for (int32 i=0; i<ulist->CountItems(); i++) {
			cur=(CacheUser*)ulist->ItemAt(i);
			if (cur->Token()==usertoken) {
				found=true;
				break;
			}
			
		}
		
/*
		CacheUser *cur=userlist;
		while (cur!=NULL) {
			if (cur->Token()==usertoken) {
				found=true;
				break;
			}
			cur=cur->Next();
		}
*/
	}
	
	return found;
}
void CacheObject::RemoveUser(uint32 usertoken) {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if (IsUsedBy(usertoken)) {
			CacheUser *cur=FindUser(usertoken);
			if (cur!=NULL) {
				ulist->RemoveItem(cur);
				UpdateAccessTime();
				if (writelockowner==cur)
					writelockowner=NULL;
				delete cur;
			}
					
		}
		
/*		
		CacheUser *cur=userlist;
		while (cur!=NULL) {
			if (cur->Token()==usertoken) {
				if (cur->Previous()!=NULL) {
					cur->Previous()->SetNext(cur->Next());
					UpdateAccessTime();
					
					if (writelockowner==cur) {
						writelockowner=NULL;
					}
					
					delete cur;
				} else {
					UpdateAccessTime();
					
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
*/
	}
	
}
void CacheObject::AddUser(uint32 usertoken) {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if (!IsUsedBy(usertoken)) {
			ulist->AddItem(new CacheUser(usertoken));
			UpdateAccessTime();
		}
		
	/*
		if (userlist==NULL) {
			userlist=new CacheUser(usertoken);
			UpdateAccessTime();
			
		} else {
			if (!IsUsedBy(usertoken)) {
				
				userlist->SetNext((new CacheUser(usertoken)));
				UpdateAccessTime();
				
			}
			
		}
		*/
	}
	
}
void CacheObject::AddUser(CacheUser *user) 
{
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if (FindUser(user->Token())==NULL) {
			ulist->AddItem(new CacheUser(user));
			
			UpdateAccessTime();
			
		}
		
/*
		if (userlist==NULL) {
			userlist=new CacheUser(user);
			UpdateAccessTime();
			
		} else {
			if (!IsUsedBy(user->Token())) {
				
				userlist->SetNext((new CacheUser(user)));
				UpdateAccessTime();
				
			}
			
		}
	
*/
	}
		
}

bool CacheObject::AcquireWriteLock(uint32 usertoken) {
	bool successful=false;
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		CacheUser *user=FindUser(usertoken);
		
		if (user==NULL) {
			
			AddUser(usertoken);
			FindUser(usertoken);
		}
		
		if (writelockowner==NULL) {
			writelockowner=user;
			successful=true;
			
		} else {
			if (writelockowner==user) {
				successful=true;
				
			}
			
		}
		UpdateAccessTime();
		
		
	/*
		if (writelockowner==NULL) {
			CacheUser *cur=userlist;
			while (cur!=NULL) {
				if (cur->Token()==usertoken) {
					successful=true;
					writelockowner=cur;
					UpdateAccessTime();
					
					break;
				}
				cur=cur->Next();
			}
		} else {
			if (writelockowner->Token()==usertoken) {
				
				successful=true;
				UpdateAccessTime();
				
			}
			
		}
		*/
	}
	
	return successful;	
}
void CacheObject::ReleaseWriteLock(uint32 usertoken) {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if (writelockowner!=NULL) {
			if (writelockowner->Token()==usertoken) {
				writelockowner=NULL;
			}
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
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if (next==NULL) {
			next=nobject;
			if (next!=NULL)
				next->SetPrevious(this);
		} else {
			next->SetNext(nobject);
		}
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
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		count=ulist->CountItems();
		
	}
	
	return count;
}
CacheUser *CacheObject::GetUser(int32 which) 
{
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		CacheUser *user=(CacheUser*)ulist->ItemAt(which);
		
		return user;
	}
	return NULL;
}

CacheUser *CacheObject::FindUser(uint32 usertoken) 
{
	CacheUser *cur=NULL;
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if (IsUsedBy(usertoken)) {
			for (int32 i=0; i<ulist->CountItems(); i++) {
				cur=(CacheUser*)ulist->ItemAt(i);
				if (cur->Token()==usertoken)
					break;
			}
			
		}
		
	}
	
	return cur;
}
const char *CacheObject::URL() 
{
	return (const char*)url;
}
void CacheObject::SetURL(const char *URL) 
{
	BAutolock alock(lock);
	if (alock.IsLocked()) {
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
	
}
void CacheObject::Remove(CacheObject *target) 
{
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		UpdateAccessTime();
		if (next==NULL)
			return;
		if (target==this) {
			if (next!=NULL)
				next->SetPrevious(NULL);
			return;
			
		}
		
		if (next==target) {
			next=next->Next();
		} else 
			next->Remove(target);
	}
	
}
bool CacheObject::HasWriteLock(uint32 usertoken) 
{
	if (writelockowner!=NULL) {
		if (writelockowner->Token()==usertoken)
			return true;
	}
	return false;
}
void CacheObject::UpdateAccessTime()
{
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		last_access_time=real_time_clock();
	}	
}

time_t CacheObject::LastAccessTime()
{
	return last_access_time;
}
time_t CacheObject::CreationTime()
{
	return creation_time;
	
}

