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
#include "cacheuser.h"
#include <string.h>
#include <stdio.h>
CacheUser::CacheUser(uint32 broadcast_target,uint32 usertoken,const char *Name) {
	broadcast_id=broadcast_target;
	if (Name!=NULL) {
		name=new char[strlen(Name)+1];
		memset(name,0,strlen(Name)+1);
		strcpy(name,Name);
	} else
		name=NULL;
	id=usertoken;
//	printf("\tCacheUser: %ld %p - %s\n",id,name,name);
	next=prev=NULL;
	readpos=writepos=0L;
}
CacheUser::CacheUser(CacheUser *cu) {
		next=prev=NULL;
	if (cu!=NULL) {
		broadcast_id=cu->broadcast_id;
		id=cu->id;
		readpos=cu->readpos;
		writepos=cu->writepos;
		if (cu->name!=NULL){
			name=new char[strlen(cu->name)+1];
			memset(name,0,strlen(cu->name)+1);
			strcpy(name,cu->name);
		}
	} else {
		id=0;
		readpos=writepos=0;
		name=NULL;
	}
//	printf("\tCacheUser (copied): %ld %p - %s\n",id,name,name);
}

CacheUser::~CacheUser() {
	next=prev=NULL;
	readpos=writepos=0L;
	id=0;
	if (name!=NULL) {
		memset(name,0,strlen(name)+1);
		delete name;
		name=NULL;
	}
}
uint32 CacheUser::BroadcastID() {
	return broadcast_id;
}

CacheUser* CacheUser::Next() {
	return next;
}
void CacheUser::Remove(CacheUser *target) {
	if (next==NULL)
		return;
	if (next==target) {
		next=next->Next();
	} else 
		next->Remove(target);
	
}

CacheUser* CacheUser::SetNext(CacheUser *nextuser) {
	if (next==NULL) {
		next=nextuser;
		if (next!=NULL)
			next->prev=this;
	} else {
		next->SetNext(nextuser);
	}
	return nextuser;
}

CacheUser* CacheUser::Previous() {
	return prev;
}

CacheUser* CacheUser::SetPrevious(CacheUser *prevuser) {
		prev=prevuser;
		if (prev!=NULL)
			prev->next=this;
		return prev;
}

uint32 CacheUser::Token() {
	return id;
}

uint32 CacheUser::SetToken(uint32 token) {
	id=token;
}

void CacheUser::operator=(CacheUser &othercu) 
{
	id=othercu.id;
	if (othercu.name!=NULL) {
		if (name!=NULL) {
			memset(name,0,strlen(name)+1);
			delete name;
			name=NULL;
		}
		name=new char[strlen(othercu.name)+1];
		memset(name,0,strlen(othercu.name)+1);
		strcpy(name,othercu.name);
	} else {
		if (name!=NULL) {
			memset(name,0,strlen(name)+1);
			delete name;
			name=NULL;
		}
	}
}

off_t CacheUser::SetWritePosition(off_t offset) 
{
	writepos=offset;
	return writepos;
}
off_t CacheUser::WritePosition() 
{
	return writepos;
}

off_t CacheUser::SetReadPosition(off_t offset) 
{
	readpos=offset;
	return readpos;
}

off_t CacheUser::ReadPosition()
{
	return readpos;
}



