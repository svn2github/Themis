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



/*
	Description:
		Below are three classes lockres, lockfile, and lockmem. lockres is a base
	class for the other two which will allow either to be used in any place where
	lockres is the expected class. This allows the Themis protocols to request
	a lockres object to write data to, and not have to code specifically for file
	or RAM access.
		The primary reason for creating these classes is a need in the new plug-in
	design to limit access to the data to a single thread at a time.

	Usage:
		In your code where you wish to have a generic sort of file/data access
	create a lockres pointer. From there, create either a lockfile or lockmem
	object and assign it to the lockres pointer. Be aware that you will not
	be permitted to read or write to the object using the standard BPositionIO
	functions WITHOUT first locking the object. It is recommended that you only
	lock the object with the Acquire() function, and unlock it with the Release()
	function. Doing otherwise might not lock/unlock necessary resources.

	Note: With each Acquire() or Release() function call, the object's read/write
	pointers are reset to offset 0, from the start of the file. (Seek(0,SEEK_SET)).
*/
#ifndef _lockable_resource_
#define _lockable_resource_

#include <File.h>
#include <DataIO.h>
#include <Locker.h>

#define LOCK_RES_BASE_CLASS 0x0
#define LOCK_RES_FILE_CLASS 0x1
#define LOCK_RES_MEM_CLASS 0x2

class lockres: public BLocker,public BPositionIO {
	public:
		lockres();
		~lockres();
		virtual int32 Acquire(bigtime_t timeout=0);
		virtual void Release();
		int32 type;
		int32 Type();
};

class lockfile: public lockres,virtual public BFile {
	public:
		lockfile(const char *fname,uint32 mode);
		lockfile(const entry_ref *ref,uint32 mode);
		lockfile(const BEntry *entry, uint32 mode);
		lockfile(BDirectory *dir,const char *path,uint32 openmode);
		~lockfile();
		int32 Acquire(bigtime_t timeout=0);
		void Release();
		off_t Seek(off_t position,uint32 mode);
		off_t Position() const;
		ssize_t Read(void *buffer,size_t bytes);
		ssize_t Write(const void *buffer, size_t bytes);
		ssize_t ReadAt(off_t position, void *buffer, size_t size);
		ssize_t WriteAt(off_t position, const void *buffer, size_t size);
		status_t SetSize(off_t bytes);
};

class lockmem: public lockres,virtual public BMallocIO {
	public:
		lockmem();
		~lockmem();
		int32 Acquire(bigtime_t timeout=0);
		void Release();
		off_t Seek(off_t position,uint32 mode);
		off_t Position() const;
		ssize_t Read(void *buffer,size_t bytes);
		ssize_t Write(const void *buffer, size_t bytes);
		ssize_t ReadAt(off_t position, void *buffer, size_t size);
		ssize_t WriteAt(off_t position, const void *buffer, size_t size);
		status_t SetSize(off_t bytes);
};

#endif
