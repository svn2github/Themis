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
#include "diskcacheobject.h"
#include <Node.h>
#include <NodeInfo.h>
#include <stdio.h>
#include "commondefs.h"
#include "plugclass.h"
#include <string.h>
#include <Path.h>
#include <String.h>
#include <fs_attr.h>
DiskCacheObject::DiskCacheObject(int32 objecttoken, const char *URL)
	:CacheObject(objecttoken,URL)
{
	file=NULL;
	refset=false;
	userlist=NULL;
	writelockowner=NULL;
}

DiskCacheObject::DiskCacheObject(int32 objecttoken, const char *_URL, entry_ref aref)
	:CacheObject(objecttoken,_URL)
{
	file=NULL;
	ref=aref;
	refset=true;
	userlist=NULL;
	writelockowner=NULL;

	if (_URL==NULL) {
		BEntry ent(&ref,true);
		if (ent.Exists()) {
			BNode node(&ref);
			node.Lock();
			attr_info ai;
			node.GetAttrInfo("Themis:URL",&ai);
			char *myurl=new char[ai.size+1];
			memset(myurl,0,ai.size+1);
			node.ReadAttr("Themis:URL",ai.type,0,myurl,ai.size);
			node.Unlock();
			node.Unset();
			SetURL((const char*)myurl);
			memset(myurl,0,ai.size+1);
			delete myurl;
			myurl=NULL;
		}
	}
	BPath path(&ref);
//	printf("disk cache object @ %s: %s\n",path.Path(),URL());
}

DiskCacheObject::~DiskCacheObject()
{
	CloseFile();
}
void DiskCacheObject::SetRef(entry_ref aref) 
{
	ref=aref;
	refset=true;
	if (file!=NULL)
		CloseFile();
}
void DiskCacheObject::OpenFile() 
{
	if (refset) {
		if (file==NULL) {
			file=new BFile(&ref,B_READ_WRITE);
			if (file->InitCheck()==B_ENTRY_NOT_FOUND) {
				file->SetTo(&ref,B_READ_WRITE|B_CREATE_FILE);
				BNodeInfo ni(file);
				ni.SetType(ThemisCacheMIME);
			}
		}
	}
}
void DiskCacheObject::CloseFile() 
{
	if (file!=NULL) {
		file->Sync();
		delete file;
		file=NULL;
		CacheUser *cur=userlist;
		while(cur!=NULL) {
			cur->SetReadPosition(0);
			cur=cur->Next();
		}
		if (writelockowner!=NULL)
			writelockowner->SetWritePosition(0);
	}
	
}
void DiskCacheObject::ClearFile() 
{
	//delete the cached file... this is a prelude to the destruction of this object.
	BEntry ent(&ref);
	if (ent.Exists()) {
		ent.Remove();
	}
	ent.Unset();
}
void DiskCacheObject::ClearContent(uint32 usertoken) 
{
//	printf("Clear content.\n");
	if (writelockowner!=NULL) {
		if (writelockowner->Token()==usertoken) {
			if (file==NULL)
				OpenFile();
			file->SetSize(0L);
			file->Sync();
			writelockowner->SetWritePosition(0L);
		}
		
	}
	
}
uint32 DiskCacheObject::Type()
{
	return TYPE_DISK;
}

ssize_t DiskCacheObject::Read(uint32 usertoken, void *buffer, size_t size)
{
	CacheUser *user=FindUser(usertoken);
	ssize_t dsize=0;
	if (buffer==NULL)
		return B_ERROR;
	if (user!=NULL) {
		if (file==NULL) {
			OpenFile();
		}
		if (file!=NULL) {
			file->Lock();
			file->Seek(user->ReadPosition(),SEEK_SET);
			dsize=file->Read(buffer,size);
			user->SetReadPosition(file->Position());
			file->Unlock();
		}
		
	} else
		dsize=B_ERROR;
	return dsize;
}

ssize_t DiskCacheObject::Write(uint32 usertoken, void *buffer, size_t size)
{
	ssize_t bytes=-1;
	if (buffer==NULL)
		return B_ERROR;
	if (writelockowner!=NULL) {
		if (writelockowner->Token()==usertoken) {
			if (file==NULL) {
				OpenFile();
			}
			file->Lock();
			bytes=file->WriteAt(writelockowner->WritePosition(),buffer,size);
			file->Sync();
			writelockowner->SetWritePosition(writelockowner->WritePosition()+bytes);
			file->Unlock();
		}
	}
	return bytes;
}
BMessage *DiskCacheObject::GetInfo() 
{
//	printf("DiskCacheObject: Getting object info.\n");
	BMessage *attributes=new BMessage;
	attributes->AddInt64("file_size",Size());
	struct attr_info ai;
	char attname[B_ATTR_NAME_LENGTH+1];
	memset(attname,0,B_ATTR_NAME_LENGTH+1);
	BNode node(&ref);
	unsigned char *data=NULL;
	BString aname;
	node.Lock();
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
				if (strcasecmp(attname,"Themis:name")==0) {
					aname="name";
				}
				if (strcasecmp(attname,"Themis:host")==0) {
					aname="host";
				}
				if (strcasecmp(attname,"Themis:mime_type")==0) {
					aname="mime-type";
				}
				if (strcasecmp(attname,"Themis:path")==0) {
					aname="server-path";
				}
				if (strcasecmp(attname,"Themis:etag")==0) {
					aname="etag";
				}
				if (strcasecmp(attname,"Themis:last-modified")==0) {
					aname="last-modified";
				}
				if (strcasecmp(attname,"Themis:expires")==0) {
					aname="expires";
				}
				if (strcasecmp(attname,"Themis:content-md5")==0) {
					aname="content-md5";
				}
				printf("reading attribute %s (%ld)\n",attname,ai.size);
				node.ReadAttr(attname,B_STRING_TYPE,0,data,ai.size);
				attributes->AddString(aname.String(),(char*)data);
			}break;

			case B_INT64_TYPE:{
				if (strcasecmp(attname,"Themis:content-length")==0) {
					off_t clen=0;
					node.ReadAttr(attname,B_INT64_TYPE,0,&clen,sizeof(clen));
					attributes->AddInt64("content-length",clen);
					clen=0;
				}
			}break;
		}
		delete data;
		data=NULL;
		memset(attname,0,B_ATTR_NAME_LENGTH+1);
	}
	node.Unlock();
//	attributes->PrintToStream();
	return attributes;
}

ssize_t DiskCacheObject::WriteAttr(uint32 usertoken, const char *attrname, type_code type,void *data,size_t size) 
{
	ssize_t bytes=0;
	if (writelockowner!=NULL) {
		if (writelockowner->Token()==usertoken){
			if (file==NULL)
				OpenFile();
			BNode node(&ref);
			node.Lock();
			bytes=node.WriteAttr(attrname,type,0,data,size);
			node.Unlock();
		}
	}
	return bytes;
}

ssize_t DiskCacheObject::ReadAttr(uint32 usertoken,  const char *attrname, type_code type, void *data, size_t size)
{
	ssize_t bytes=0;
	BNode node(&ref);
	node.Lock();
	node.ReadAttr(attrname,type,0,data,size);
	node.Unlock();
	return bytes;
}
status_t DiskCacheObject::RemoveAttr(uint32 usertoken, const char *attrname) 
{
	status_t status=B_ERROR;
	if (writelockowner!=NULL) {
		if (writelockowner->Token()==usertoken){
			BNode node(&ref);
			node.Lock();
			status=node.RemoveAttr(attrname);
			node.Unlock();
		}
	}
	return status;
}


off_t DiskCacheObject::Size() 
{
	off_t size=0;
//	printf("DiskCacheObject::Size. File pointer %p\n",file);
	if (file==NULL) {
//		printf("Opening file\n");
		OpenFile();
//		printf("Getting size\n");
		file->GetSize(&size);
//		printf("Closing file.\n");
		CloseFile();
	} else
		file->GetSize(&size);
	return size;
}
