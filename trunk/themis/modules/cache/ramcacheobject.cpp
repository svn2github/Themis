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
#include "ramcacheobject.h"
#include "plugclass.h"
#include <string.h>
#include <stdio.h>
RAMCacheObject::RAMCacheObject(int32 token,const char *URL)
	:CacheObject(token,URL) {
	databuffer=new BMallocIO();
	databuffer->SetBlockSize(1);
	name=NULL;
	host=NULL;
	mimetype=NULL;
	path=NULL;
	etag=NULL;
	lastmodified=NULL;
	expires=NULL;
	contentmd5=NULL;
	contentlength=0L;
	userlist=NULL;
	writelockowner=NULL;
}
RAMCacheObject::~RAMCacheObject(){
	if (databuffer!=NULL)
		delete databuffer;
	if (name!=NULL) {
		memset(name,0,strlen(name)+1);
		delete name;
		name=NULL;
	}
	if (host!=NULL) {
		memset(host,0,strlen(host)+1);
		delete host;
		host=NULL;
	}
	if (mimetype!=NULL) {
		memset(mimetype,0,strlen(mimetype)+1);
		delete mimetype;
		mimetype=NULL;
	}
	if (path!=NULL) {
		memset(path,0,strlen(path)+1);
		delete path;
		path=NULL;
	}
	if (etag!=NULL) {
		memset(etag,0,strlen(etag)+1);
		delete etag;
		etag=NULL;
	}
	if (lastmodified!=NULL) {
		memset(lastmodified,0,strlen(lastmodified)+1);
		delete lastmodified;
		lastmodified=NULL;
	}
	if (expires!=NULL) {
		memset(expires,0,strlen(expires)+1);
		delete expires;
		expires=NULL;
	}
	if (contentmd5!=NULL) {
		memset(contentmd5,0,strlen(contentmd5)+1);
		delete contentmd5;
		contentmd5=NULL;
	}
	contentlength=0L;
}
ssize_t RAMCacheObject::Read(uint32 usertoken, void *buffer, size_t size)
{
	CacheUser *user=FindUser(usertoken);
	ssize_t dsize=0;
	if (buffer==NULL)
		return B_ERROR;
	if (user!=NULL) {
		if (databuffer!=NULL) {
//			buffer->Seek(user->ReadPosition(),SEEK_SET);
			dsize=databuffer->ReadAt(user->ReadPosition(),buffer,size);
			user->SetReadPosition(dsize+user->ReadPosition());
		}
		
	} else
		dsize=B_ERROR;
	return dsize;
}
ssize_t RAMCacheObject::Write(uint32 usertoken, void *buffer, size_t size)
{
	ssize_t bytes=-1;
	if (buffer==NULL)
		return B_ERROR;
	if (writelockowner==NULL)
		AcquireWriteLock(usertoken);
	if (writelockowner!=NULL) {
		if (writelockowner->Token()==usertoken) {
			bytes=databuffer->WriteAt(writelockowner->WritePosition(),buffer,size);
			writelockowner->SetWritePosition(writelockowner->WritePosition()+bytes);
		}
	}
	printf("RAM Cache: URL: %s\tSize: %ld\n",url,Size());
	return bytes;
}
ssize_t RAMCacheObject::SetLength(uint32 usertoken, int32 objecttoken, size_t length) 
{
	ssize_t size=0;
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if (writelockowner==NULL)
			AcquireWriteLock(usertoken);
		if (writelockowner!=NULL) {
			if (writelockowner->Token()==usertoken) {
				if (databuffer->SetSize(length)==B_OK)
					size=length;
				else
					size=databuffer->BufferLength();
				
			}
		}
		
	}
	return size;
	
}

ssize_t RAMCacheObject::WriteAttr(uint32 usertoken, const char *attrname, type_code type,void *data,size_t size)
{
	if (writelockowner!=NULL) {
		if (writelockowner->Token()==usertoken) {
				if (strcasecmp(attrname,"Themis:URL")==0) {
					//do nothing... we already know the URL.
				}
				if (strcasecmp(attrname,"Themis:name")==0) {
					if (name!=NULL) {
						memset(name,0,strlen(name)+1);
						delete name;
						name=NULL;
					}
					name=new char[size+1];
					memset(name,0,size+1);
					memcpy(name,data,size);
				}
				if (strcasecmp(attrname,"Themis:host")==0) {
					if (host!=NULL) {
						memset(host,0,strlen(host)+1);
						delete host;
						host=NULL;
					}
					host=new char[size+1];
					memset(host,0,size+1);
					memcpy(host,data,size);
				}
				if (strcasecmp(attrname,"Themis:mime_type")==0) {
					if (mimetype!=NULL) {
						memset(mimetype,0,strlen(mimetype)+1);
						delete mimetype;
						mimetype=NULL;
					}
					mimetype=new char[size+1];
					memset(mimetype,0,size+1);
					memcpy(mimetype,data,size);
				}
				if (strcasecmp(attrname,"Themis:path")==0) {
					if (path!=NULL) {
						memset(path,0,strlen(path)+1);
						delete path;
						path=NULL;
					}
					path=new char[size+1];
					memset(path,0,size+1);
					memcpy(path,data,size);
				}
				if (strcasecmp(attrname,"Themis:etag")==0) {
					if (etag!=NULL) {
						memset(etag,0,strlen(etag)+1);
						delete etag;
						etag=NULL;
					}
					etag=new char[size+1];
					memset(etag,0,size+1);
					memcpy(etag,data,size);
				}
				if (strcasecmp(attrname,"Themis:last-modified")==0) {
					if (lastmodified!=NULL) {
						memset(lastmodified,0,strlen(lastmodified)+1);
						delete lastmodified;
						lastmodified=NULL;
					}
					lastmodified=new char[size+1];
					memset(lastmodified,0,size+1);
					memcpy(lastmodified,data,size);
				}
				if (strcasecmp(attrname,"Themis:expires")==0) {
					if (expires!=NULL) {
						memset(expires,0,strlen(expires)+1);
						delete expires;
						expires=NULL;
					}
					expires=new char[size+1];
					memset(expires,0,size+1);
					memcpy(expires,data,size);
				}
				if (strcasecmp(attrname,"Themis:content-md5")==0) {
					if (contentmd5!=NULL) {
						memset(contentmd5,0,strlen(contentmd5)+1);
						delete contentmd5;
						contentmd5=NULL;
					}
					contentmd5=new char[size+1];
					memset(contentmd5,0,size+1);
					memcpy(contentmd5,data,size);
				}
				if (strcasecmp(attrname,"Themis:content-length")==0) {
					contentlength=0L;
					memcpy(&contentlength,data,size);
				}
				
		}
		
	}
	return size;
}
ssize_t RAMCacheObject::ReadAttr(uint32 usertoken,  const char *attrname, type_code type, void *data, size_t size)
{
	ssize_t actualsize=0;
	if (strcasecmp(attrname,"Themis:URL")==0) {
		actualsize=strlen(url);
		if (actualsize>size)
			actualsize=size;
		memcpy(data,url,actualsize);
	}
	if (strcasecmp(attrname,"Themis:name")==0) {
		if (name==NULL) {
			actualsize=0L;
		} else {
			actualsize=strlen(name);
			if (actualsize>size)
				actualsize=size;
			memcpy(data,name,actualsize);
		}
	}
	if (strcasecmp(attrname,"Themis:host")==0) {
		if (host==NULL) {
			actualsize=0L;
		} else {
			actualsize=strlen(host);
			if (actualsize>size)
				actualsize=size;
			memcpy(data,host,actualsize);
		}
	}
	if (strcasecmp(attrname,"Themis:mime_type")==0) {
		if (mimetype==NULL) {
			actualsize=0L;
		} else {
			actualsize=strlen(mimetype);
			if (actualsize>size)
				actualsize=size;
			memcpy(data,mimetype,actualsize);
		}
	}
	if (strcasecmp(attrname,"Themis:path")==0) {
		if (path==NULL) {
			actualsize=0L;
		} else {
			actualsize=strlen(path);
			if (actualsize>size)
				actualsize=size;
			memcpy(data,path,actualsize);
		}
	}
	if (strcasecmp(attrname,"Themis:etag")==0) {
		if (etag==NULL) {
			actualsize=0L;
		} else {
			actualsize=strlen(etag);
			if (actualsize>size)
				actualsize=size;
			memcpy(data,etag,actualsize);
		}
	}
	if (strcasecmp(attrname,"Themis:last-modified")==0) {
		if (lastmodified==NULL) {
			actualsize=0L;
		} else {
			actualsize=strlen(lastmodified);
			if (actualsize>size)
				actualsize=size;
			memcpy(data,lastmodified,actualsize);
		}
	}
	if (strcasecmp(attrname,"Themis:expires")==0) {
		if (expires==NULL) {
			actualsize=0L;
		} else {
			actualsize=strlen(expires);
			if (actualsize>size)
				actualsize=size;
			memcpy(data,expires,actualsize);
		}
	}
	if (strcasecmp(attrname,"Themis:content-md5")==0) {
		if (contentmd5==NULL) {
			actualsize=0L;
		} else {
			actualsize=strlen(contentmd5);
			if (actualsize>size)
				actualsize=size;
			memcpy(data,contentmd5,actualsize);
		}
	}
	if (strcasecmp(attrname,"Themis:content-length")==0) {
		contentlength=0L;
		memcpy(data,&contentlength,sizeof(contentlength));
		actualsize=sizeof(contentlength);
	}
	return actualsize;
}
status_t RAMCacheObject::RemoveAttr(uint32 usertoken, const char *attrname)
{
	status_t status=B_ERROR;
	if (writelockowner!=NULL) {
		if (writelockowner->Token()==usertoken){
			if (strcasecmp(attrname,"Themis:URL")==0) {
				//do nothing
			}
			if (strcasecmp(attrname,"Themis:name")==0) {
				if (name!=NULL) {
					memset(name,0,strlen(name)+1);
					delete name;
					name=NULL;
				}
			}
			if (strcasecmp(attrname,"Themis:host")==0) {
				if (host!=NULL) {
					memset(host,0,strlen(host)+1);
					delete host;
					host=NULL;
				}
			}
			if (strcasecmp(attrname,"Themis:mime_type")==0) {
				if (mimetype!=NULL) {
					memset(mimetype,0,strlen(mimetype)+1);
					delete mimetype;
					mimetype=NULL;
				}
			}
			if (strcasecmp(attrname,"Themis:path")==0) {
				if (path!=NULL) {
					memset(path,0,strlen(path)+1);
					delete path;
					path=NULL;
				}
			}
			if (strcasecmp(attrname,"Themis:etag")==0) {
				if (etag!=NULL) {
					memset(etag,0,strlen(etag)+1);
					delete etag;
					etag=NULL;
				}
			}
			if (strcasecmp(attrname,"Themis:last-modified")==0) {
				if (lastmodified!=NULL) {
					memset(lastmodified,0,strlen(lastmodified)+1);
					delete lastmodified;
					lastmodified=NULL;
				}
			}
			if (strcasecmp(attrname,"Themis:expires")==0) {
				if (expires!=NULL) {
					memset(expires,0,strlen(expires)+1);
					delete expires;
					expires=NULL;
				}
			}
			if (strcasecmp(attrname,"Themis:content-md5")==0) {
				if (contentmd5!=NULL) {
					memset(contentmd5,0,strlen(contentmd5)+1);
					delete contentmd5;
					contentmd5=NULL;
				}
			}
			if (strcasecmp(attrname,"Themis:content-length")==0) {
				contentlength=0L;
			}
		}
	}
	
	return status;
}

uint32 RAMCacheObject::Type()
{
	return TYPE_RAM;
}

off_t RAMCacheObject::Size()
{
	if (databuffer!=NULL)
		return databuffer->BufferLength();
	return 0L;
}
void RAMCacheObject::ClearFile() 
{
	if (databuffer!=NULL) {
		databuffer->SetSize(0);
	} 	
}
BMessage *RAMCacheObject::GetInfo()
{
	BMessage *attributes=new BMessage;
	attributes->AddInt64("file_size",Size());
	if (name!=NULL)
		attributes->AddString("name",name);
	if (url!=NULL)
		attributes->AddString("url",url);
	if (host!=NULL)
		attributes->AddString("host",host);
	if (mimetype!=NULL)
		attributes->AddString("mime-type",mimetype);
	if (path!=NULL)
		attributes->AddString("path", path);
	if (etag!=NULL)
		attributes->AddString("etag",etag);
	if (lastmodified!=NULL)
		attributes->AddString("last-modified",lastmodified);
	if (expires!=NULL)
		attributes->AddString("expires",expires);
	if (contentmd5!=NULL)
		attributes->AddString("content-md5",contentmd5);
	attributes->AddInt64("content-length",contentlength);
	return attributes;
}
void RAMCacheObject::ClearContent(uint32 usertoken) 
{
	if (writelockowner!=NULL) {
		if (writelockowner->Token()==usertoken)	
			databuffer->SetSize(0);
	}
	
}
