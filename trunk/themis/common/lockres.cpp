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

#include "lockres.h"
lockres::lockres()
	:BLocker(false),BPositionIO() {
	Lock();
		type=LOCK_RES_BASE_CLASS;
	Unlock();
}

lockres::~lockres() {
	Lock();
	Unlock();
}
int32 lockres::Acquire(bigtime_t timeout) {
	if (timeout>0) {
		return LockWithTimeout(timeout);
	}	else {
		if (Lock())
			return B_OK;
		return B_ERROR;
	}
}

void lockres::Release() {
	Unlock();
}

int32 lockres::Type() {
	return type;
}

lockfile::lockfile(const char *fname,uint32 mode)
	:lockres(),BFile(fname,mode){
		type=LOCK_RES_FILE_CLASS;
}
lockfile::lockfile(const entry_ref *ref,uint32 mode)
	:lockres(),BFile(ref,mode){
}
lockfile::lockfile(const BEntry *entry, uint32 mode)
	:lockres(),BFile(entry,mode){
}
lockfile::lockfile(BDirectory *dir,const char *path,uint32 mode)
	:lockres(),BFile(dir,path,mode){
}
lockfile::~lockfile() {
}

int32 lockfile::Acquire(bigtime_t timeout) {
	int32 ret=lockres::Acquire(timeout);
	Seek(0,SEEK_SET);
	if (ret==B_OK) {
		BFile::Lock();
	}
	return ret;
}
void lockfile::Release() {
	Seek(0,SEEK_SET);
	BFile::Unlock();
	lockres::Release();
	
}
off_t lockfile::Seek(off_t position,uint32 mode) {
	thread_id callingthread=find_thread(NULL),lockholder=LockingThread();
	if (lockholder==B_ERROR) {
		return B_ERROR;
	} else {
		if (lockholder!=callingthread)
			return B_ERROR;
	}
	return BFile::Seek(position,mode);
}

off_t lockfile::Position() const{
	thread_id callingthread=find_thread(NULL),lockholder=LockingThread();
	if (lockholder==B_ERROR) {
		return B_ERROR;
	} else {
		if (lockholder!=callingthread)
			return B_ERROR;
	}
	return BFile::Position();
}
ssize_t lockfile::Read(void *buffer,size_t bytes){
	thread_id callingthread=find_thread(NULL),lockholder=LockingThread();
	if (lockholder==B_ERROR) {
		return B_ERROR;
	} else {
		if (lockholder!=callingthread)
			return B_ERROR;
	}
	return BFile::Read(buffer,bytes);
}
ssize_t lockfile::Write(const void *buffer, size_t bytes){
	thread_id callingthread=find_thread(NULL),lockholder=LockingThread();
	if (lockholder==B_ERROR) {
		return B_ERROR;
	} else {
		if (lockholder!=callingthread)
			return B_ERROR;
	}
	return BFile::Write(buffer,bytes);
}
ssize_t lockfile::ReadAt(off_t position, void *buffer, size_t size){
	thread_id callingthread=find_thread(NULL),lockholder=LockingThread();
	if (lockholder==B_ERROR) {
		return B_ERROR;
	} else {
		if (lockholder!=callingthread)
			return B_ERROR;
	}
	return BFile::ReadAt(position,buffer,size);
}
		
ssize_t lockfile::WriteAt(off_t position, const void *buffer, size_t size){
	thread_id callingthread=find_thread(NULL),lockholder=LockingThread();
	if (lockholder==B_ERROR) {
		return B_ERROR;
	} else {
		if (lockholder!=callingthread)
			return B_ERROR;
	}
	return BFile::WriteAt(position,buffer,size);
}
status_t lockfile::SetSize(off_t bytes){
	thread_id callingthread=find_thread(NULL),lockholder=LockingThread();
	if (lockholder==B_ERROR) {
		return B_ERROR;
	} else {
		if (lockholder!=callingthread)
			return B_ERROR;
	}
	return BFile::SetSize(bytes);
}
		
		

lockmem::lockmem() 
	:lockres(),BMallocIO() {
	SetBlockSize(1);
	type=LOCK_RES_MEM_CLASS;
}
lockmem::~lockmem() {
}

off_t lockmem::Seek(off_t position,uint32 mode) {
	thread_id callingthread=find_thread(NULL),lockholder=LockingThread();
	if (lockholder==B_ERROR) {
		return B_ERROR;
	} else {
		if (lockholder!=callingthread)
			return B_ERROR;
	}
	return BMallocIO::Seek(position,mode);
}

off_t lockmem::Position() const {
	thread_id callingthread=find_thread(NULL),lockholder=LockingThread();
	if (lockholder==B_ERROR) {
		return B_ERROR;
	} else {
		if (lockholder!=callingthread)
			return B_ERROR;
	}
	return BMallocIO::Position();
}
ssize_t lockmem::Read(void *buffer,size_t bytes) {
	thread_id callingthread=find_thread(NULL),lockholder=LockingThread();
	if (lockholder==B_ERROR) {
		return B_ERROR;
	} else {
		if (lockholder!=callingthread)
			return B_ERROR;
	}
	return BMallocIO::Read(buffer,bytes);
}
ssize_t lockmem::Write(const void *buffer, size_t bytes) {
	thread_id callingthread=find_thread(NULL),lockholder=LockingThread();
	if (lockholder==B_ERROR) {
		return B_ERROR;
	} else {
		if (lockholder!=callingthread)
			return B_ERROR;
	}
	return BMallocIO::Write(buffer,bytes);
}
ssize_t lockmem::ReadAt(off_t position, void *buffer, size_t size){
	thread_id callingthread=find_thread(NULL),lockholder=LockingThread();
	if (lockholder==B_ERROR) {
		return B_ERROR;
	} else {
		if (lockholder!=callingthread)
			return B_ERROR;
	}
	return BMallocIO::ReadAt(position,buffer,size);
}
		
ssize_t lockmem::WriteAt(off_t position, const void *buffer, size_t size){
	thread_id callingthread=find_thread(NULL),lockholder=LockingThread();
	if (lockholder==B_ERROR) {
		return B_ERROR;
	} else {
		if (lockholder!=callingthread)
			return B_ERROR;
	}
	return BMallocIO::WriteAt(position,buffer,size);
}
status_t lockmem::SetSize(off_t bytes){
	thread_id callingthread=find_thread(NULL),lockholder=LockingThread();
	if (lockholder==B_ERROR) {
		return B_ERROR;
	} else {
		if (lockholder!=callingthread)
			return B_ERROR;
	}
	return BMallocIO::SetSize(bytes);
}
int32 lockmem::Acquire(bigtime_t timeout=0) {
	int32 ret=lockres::Acquire(timeout);
	Seek(0,SEEK_SET);
	return ret;
	
}

void lockmem::Release() {
	Seek(0,SEEK_SET);
	lockres::Release();
}

		
