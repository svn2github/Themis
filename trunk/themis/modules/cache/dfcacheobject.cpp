/*
Copyright (c) 2003 Raymond "Z3R0 One" Rodgers. All Rights Reserved.

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

Original Author & Project Manager: Raymond "Z3R0 One" Rodgers (z3r0_one@bbnk.dhs.org)
Project Start Date: October 18, 2000
*/
#include "dfcacheobject.h"
#include <stdio.h>
#include <string.h>
#include <Path.h>
#include <Node.h>
#include <String.h>
#include <Autolock.h>
#include "plugclass.h"
#include <fs_attr.h>
DFCacheObject::DFCacheObject(int32 token, const char *URL)
	:CacheObject(token,URL) {
	printf("Disk File Cache Object: URL - %s\n",url);
		file=NULL;
		memset(&ref,0,sizeof(ref));
	if ((url!=NULL) && (strlen(url)>1)) {
		if (get_ref_for_path(url,&ref)==B_OK)
			file=new BFile(url,B_READ_WRITE);
		
	}
	
	writelockowner=NULL;
		userlist=NULL;
		
}

DFCacheObject::~DFCacheObject() {
	if (file!=NULL) {
		delete file;
	}
	
}
BPositionIO *DFCacheObject::IOPointer() 
{
	return file;
}

ssize_t DFCacheObject::Read(uint32 usertoken, void *buffer, size_t size) {
	CacheUser *user=FindUser(usertoken);
	ssize_t dsize=0L;
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if (buffer==NULL)
			return B_ERROR;
		if (user!=NULL) {
			if (file==NULL) {
				file=new BFile(url,B_READ_WRITE|B_CREATE_FILE);
			}
			if (file!=NULL) {
			printf("file init check: %ld\n",file->InitCheck());
				
				file->Lock();
				file->Seek(user->ReadPosition(),SEEK_SET);
				dsize=file->Read((unsigned char*)buffer,size);
				printf("DFCacheObject: %ld bytes read\n",dsize);
				
				user->SetReadPosition(file->Position());
				file->Unlock();
			}
			
		} else
			dsize=B_ERROR;
	}
	
	return dsize;
}

ssize_t DFCacheObject::Write(uint32 usertoken, void *buffer, size_t size) {
	ssize_t bytes=-1;
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if (buffer==NULL)
			return B_ERROR;
		if (writelockowner==NULL)
			AcquireWriteLock(usertoken);
		if (writelockowner!=NULL) {
			if (writelockowner->Token()==usertoken) {
				if (file==NULL) {
					file=new BFile(url,B_READ_WRITE|B_CREATE_FILE);
				}
				file->Lock();
				bytes=file->WriteAt(writelockowner->WritePosition(),buffer,size);
				writelockowner->SetWritePosition(writelockowner->WritePosition()+bytes);
				file->Sync();
				file->Unlock();
			}
		}
	}
	return bytes;
}

off_t DFCacheObject::Size() 
{
	off_t size=0L;
	if (file!=NULL)
		file->GetSize(&size);
	return size;
	
}


void DFCacheObject::ClearFile()
{
}

void DFCacheObject::ClearContent(uint32 usertoken)
{
}
uint32 DFCacheObject::Type()
{
	return TYPE_DISK_FILE;
}

BMessage *DFCacheObject::GetInfo()
{
	BMessage *attributes=new BMessage;
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		attributes->AddInt64("file_size",Size());
		struct attr_info ai;
		char attname[B_ATTR_NAME_LENGTH+1];
		memset(attname,0,B_ATTR_NAME_LENGTH+1);
		BNode node(&ref);
		unsigned char *data=NULL;
		BString aname;
		node.Lock();
		attributes->AddString("host","file");
		attributes->AddString("server-path",url);
		BPath path(url);
		attributes->AddString("name",path.Leaf());
		BString URL;
		URL<<"file://"<<url;
		attributes->AddString("url",URL.String());
		
		while (node.GetNextAttrName(attname)==B_OK) {
	//		printf("loop!\n");
			node.GetAttrInfo(attname,&ai);
			aname.Truncate(0);
			if (data!=NULL)
				delete data;
			data=new unsigned char[ai.size+1];
			memset(data,0,ai.size+1);
			switch(ai.type) {
				case B_INT32_TYPE:{
				}break;
	
				case B_STRING_TYPE:{
					if (strcasecmp(attname,"Themis:URL")==0) {
						aname="url";
					}
					if (strcasecmp(attname,"BEOS:TYPE")==0) {
						aname="mime-type";
					}
					if (strcasecmp(attname,"last_modified")==0) {
						aname="last-modified";
					}
					printf("reading attribute %s (%ld)\n",attname,ai.size);
					node.ReadAttr(attname,B_STRING_TYPE,0,data,ai.size);
					attributes->AddString(aname.String(),(char*)data);
				}break;
	
				case B_INT64_TYPE:{
				}break;
			}
			delete data;
			data=NULL;
			memset(attname,0,B_ATTR_NAME_LENGTH+1);
		}
		node.Unlock();
	//	attributes->PrintToStream();
	}
	return attributes;
}
ssize_t DFCacheObject::WriteAttr(uint32 usertoken, const char *attrname, type_code type,void *data,size_t size)
{
}
ssize_t DFCacheObject::ReadAttr(uint32 usertoken,  const char *attrname, type_code type, void *data, size_t size)
{
}
