/*
Copyright (c) 2003 Z3R0 One. All Rights Reserved.

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

Original Author & Project Manager: Z3R0 One (z3r0_one@bbnk.dhs.org)
Project Start Date: October 18, 2000
*/
/*!
\file
\brief The network system's internal buffer class definition.


*/
#include "netbuffer.h"

#include <string.h>
#include <stdlib.h>
#include <Autolock.h>

using namespace _Themis_Networking_;

Buffer::Buffer() {
	Init();
	lock=new BLocker(true);
}
Buffer::Buffer(void *data, off_t size) {
	Init();
	lock=new BLocker(true);
	SetData(data,size);
}
Buffer::~Buffer() {
	if ((data_size!=0) || (buffer!=NULL)) {
		Empty();
	}
	delete lock;
}
void Buffer::Init() {
	data_size=0L;
	buffer=NULL;
	been_read=false;
}
off_t Buffer::Size() {
	off_t size=0L;
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		size=data_size;
	}
	return size;
}
bool Buffer::HasBeenRead() {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		return been_read;
	}
	return false;
}
bool Buffer::MarkRead(bool have_read) {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		been_read=have_read;
	}
	return been_read;
}
bool Buffer::IsEmpty() {
	bool isempty=false;
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if ((data_size==0L) && (buffer==NULL))
			isempty=true;
	}
	return isempty;
}
void Buffer::Empty() {
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if (buffer!=NULL) {
			memset(buffer,0,data_size);
			delete buffer;
			buffer=NULL;
		}
		been_read=false;
		data_size=0L;
	}
}
off_t Buffer::GetData(void *data, off_t max_size) {
	off_t num=0L;
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if (max_size<=0)
			num=data_size;
		else
			num=min_c(max_size,data_size);
		if ((num>0L) && (buffer!=NULL)) {
			memcpy((unsigned char*)data,buffer,num);
			been_read=true;
		} else
			num=0L;
	}
	return num;
}
off_t Buffer::SetData(void *data, off_t size) {
	off_t num=0L;
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if (buffer!=NULL) {
			if (size!=data_size) {
				memset(buffer,0,data_size);
				delete buffer;
				buffer=new unsigned char[size];
			}
			memset(buffer,0,size);
		} else {
			buffer=new unsigned char[size];
			memset(buffer,0,size);
		}
		memcpy(buffer,(unsigned char*)data,size);
		num=data_size=size;
		been_read=false;
	}
	return num;
}
