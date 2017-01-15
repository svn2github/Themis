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
#include "cacheplug.h"
#include <File.h>


CachePlug::CachePlug(BMessage *info,const char *msg_sys_name)
	:PlugClass(info,msg_sys_name)
{
	user_token_value=0;
	object_token_value=0;
	userlist=NULL;
}

CachePlug::~CachePlug() 
{
	if (userlist!=NULL) {
		CacheUser *cur=NULL;
		while (userlist!=NULL) {
			cur=userlist->Next();
			delete userlist;
			userlist=cur;
		}
	}
}
uint32 CachePlug::Register(uint32 broadcast_target,const char* name) 
{
	if (userlist==NULL) {
		userlist=new CacheUser(broadcast_target,user_token_value,name);
		user_token_value++;
		
	} else {
		userlist->SetNext(new CacheUser(broadcast_target,user_token_value,name));
		user_token_value++;
	}
	
	return (user_token_value-1);
}
void CachePlug::Unregister(uint32 usertoken) 
{
	CacheUser *cur=userlist;
	if (cur==NULL)
		return;
	
	while (cur!=NULL) {
		if (cur->Token()==usertoken) {
			if (cur->Previous()!=NULL) {
				cur->Previous()->Remove(cur);
				delete cur;
				break;
			} else {
				userlist=cur->Next();
				if (userlist!=NULL)
					userlist->SetPrevious(NULL);
				delete cur;
				break;
			}
			
		}
		cur=cur->Next();
	}
	
}

int32 CachePlug::FindObject(uint32 usertoken, const char *URL) 
{
	return B_ERROR;
}

int32 CachePlug::CreateObject(uint32 usertoken, const char *URL, uint32 type)
{
	return B_ERROR;
}
status_t CachePlug::UpdateAttr(uint32 usertoken, int32 objecttoken, const char *attrname, type_code type, void *data, size_t size)
{
	return B_ERROR;
}

void CachePlug::ClearRequests(uint32 usertoken, int32 objecttoken) 
{
}
void CachePlug::ClearAllRequests(uint32 usertoken)
{
}
BMessage *CachePlug::GetInfo(uint32 usertoken, int32 objecttoken){
	return NULL;
}

ssize_t CachePlug::Write(uint32 usertoken, int32 objecttoken, void *data, size_t size){
	return 0L;
}

ssize_t CachePlug::Read(uint32 usertoken, int32 objecttoken, void *data, size_t size,bool resetReadPosition, off_t newReadPosition){
	return 0L;
}
ssize_t CachePlug::SetLength(uint32 usertoken, int32 objecttoken, size_t length)
{
	return 0L;
}

void CachePlug::ClearCache(uint32 which){
}


