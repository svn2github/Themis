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
#include "cacheman.h"
#include "dfcacheobject.h"
#include "commondefs.h"
#include "cachesorts.h"
#include "plugman.h"
#include "ramcacheobject.h"
#include "PrefsDefs.h"
#include <kernel/fs_index.h>
#include <time.h>
#include <Autolock.h>
#include <stdio.h>
#include <kernel/OS.h>
#include <string.h>
#include <String.h>
#include <AppKit.h>
#include <Messenger.h>
#include <MessageRunner.h>
#include <Statable.h>
#include <File.h>
#include <fs_attr.h>
#include <Node.h>
#include <Path.h>

//this is to define an object that will be removed from memory almost immediately.
#define TemporaryObjectID -2

#define ONE_SECOND 1
#define ONE_MINUTE (ONE_SECOND*60)
#define ONE_BYTE 1
#define ONE_KILOBYTE (1024*ONE_BYTE)
#define ONE_MEGABYTE (1024*ONE_KILOBYTE)
#define MAX_RAM_CACHE_UNUSED_TIME (ONE_MINUTE*30.0)
#define MAX_DISK_CACHE_UNUSED_TIME (ONE_MINUTE*10.0)
#define MAX_DISK_FILE_CACHE_UNUSED_TIME (ONE_MINUTE*2.0)
#define MAX_RAM_CACHE_SIZE (15*ONE_MEGABYTE)
#define MAX_DISK_CACHE_SIZE (50*ONE_MEGABYTE)
cacheman::cacheman(BMessage *info,const char *msg_sys_name)
	:CachePlug(info,msg_sys_name) {
	lock=new BLocker(true);
	ASp=NULL;
	AppSettings=NULL;
	CSettings=NULL;
	max_disk_cache_kb=max_ram_cache_kb=0;
	/*
	Find and set the settings information if available.
	*/
	if (InitInfo!=NULL)
		InitInfo->FindPointer("settings_message_ptr",(void**)&ASp);
	if (ASp!=NULL) {
		AppSettings=*ASp;
		CSettings=new BMessage;
		if (AppSettings->HasMessage("disk_cache_settings")) {
			AppSettings->FindMessage("disk_cache_settings",CSettings);
			if (CSettings!=NULL) {
				BString temp;
//				CSettings->PrintToStream();
				CSettings->FindString("cache_path",&temp);
				cachepath.SetTo(temp.String());
				if (CSettings->HasInt32("max_disk_cache_kb"))
					max_disk_cache_kb=CSettings->FindInt32("max_disk_cache_kb");
				else
					max_disk_cache_kb=20480;
				if (CSettings->HasInt32("max_ram_cache_kb"))
					max_ram_cache_kb=CSettings->FindInt32("max_ram_cache_kb");
				else
					max_ram_cache_kb=10240;
			}
			
		} else {
			FindCacheDir();
			max_disk_cache_kb=20480;
			max_ram_cache_kb=10240;
			CSettings->AddInt32("max_ram_cache_kb",max_ram_cache_kb);
			CSettings->AddInt32("max_disk_cache_kb",max_disk_cache_kb);
			CSettings->AddString("cache_path",cachepath.Path());
		}
		
	} else {
			FindCacheDir();
	}
	InternalCacheUserToken=0;
	objlist=NULL;
	userlist=NULL;
	CheckIndices();
	CheckMIME();
	InternalCacheUserToken=Register(Type(),"Disk Cache Manager");
	BPath trashpath;
	if (find_directory(B_TRASH_DIRECTORY,&trashpath)==B_OK) {
		trashdir=new BDirectory(trashpath.Path());
	} else {
		trashdir=new BDirectory("/boot/home/Desktop/Trash");
	}
	printf("Total Cache Size: %ld\n",GetCacheSize());
	printf("RAM Cache Size: %ld\nDisk CacheSize: %ld\n",GetCacheSize(TYPE_RAM),GetCacheSize(TYPE_DISK));
}

cacheman::~cacheman() 
{
	Unregister(InternalCacheUserToken);
	if (userlist!=NULL) {
		CacheUser *cur=NULL;
		while (userlist!=NULL) {
			cur=userlist->Next();
			delete userlist;
			userlist=cur;
		}
	}
	if (objlist!=NULL) {
		CacheObject *cur=NULL;
		while (objlist!=NULL) {
			cur=objlist->Next();
			delete objlist;
			objlist=(DiskCacheObject*)cur;
		}
		
	}
 	if (trashdir!=NULL)
 		delete trashdir;
 	delete lock;
	
}
void cacheman::Unregister(uint32 usertoken) 
{
	ClearAllRequests(usertoken);
	CachePlug::Unregister(usertoken);
}
uint32 cacheman::BroadcastTarget(){
	printf("Cache\n");
	return MS_TARGET_CACHE_SYSTEM;
}
bool cacheman::RequiresHeartbeat()
{
	return true;
}
void cacheman::Heartbeat()
{
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		CacheObject *cur=objlist,*prev=NULL;
		time_t current_time=real_time_clock();
//		if (current_time%10==0)
//			printf("Total items in cache memory:\t%ld\n\tDisk:\t%ld\n\tRAM:\t%ld\n\tFile:\t%ld\n",CountCacheItems(),CountCacheItems(TYPE_DISK),CountCacheItems(TYPE_RAM),CountCacheItems(TYPE_DISK_FILE));
		double time_diff;
		/*
			Cache Usage Aging.
		
			This function attempts to minimize memory usage by removing cache items from memory
			after a specified length of time. In the case of items cached to disk, or disk files
			loaded into the cache system, only the in memory reference to the item is removed after
			it hasn't be utilized for the specified period of time. In the case of items cached
			to RAM, the memory reference and the data itself are removed from RAM; if the data is
			needed again after removal, it will need to be downloaded again! Optionally, this data
			may be saved to disk in a temporary file, and tracked with a smaller in memory structure.
			This latter option may be implemented later if it is determined that it is the better
			solution.
		*/
		while (cur!=NULL) {
			time_diff=difftime(current_time,cur->LastAccessTime());
			switch(cur->Type()) {
				case TYPE_RAM: {
					if (time_diff>=(MAX_RAM_CACHE_UNUSED_TIME)) {
						if (prev==NULL) {
							objlist=cur->Next();
							cur->Remove(cur);
							delete cur;
							cur=objlist;
						} else {
							cur->Remove(cur);
							delete cur;
							cur=prev->Next();
						}
						continue;
					}// else
					//	printf("Item has %2.2f seconds remaining in memory.\n",MAX_RAM_CACHE_UNUSED_TIME-time_diff);
				}break;
				case TYPE_DISK: {
					if (time_diff>=(MAX_DISK_CACHE_UNUSED_TIME)) {
						if (prev==NULL) {
							objlist=cur->Next();
							cur->Remove(cur);
							delete cur;
							cur=objlist;
						} else {
							cur->Remove(cur);
							delete cur;
							cur=prev->Next();
						}
						continue;
					}// else
					//	printf("Item has %2.2f seconds remaining in memory.\n",MAX_DISK_CACHE_UNUSED_TIME-time_diff);
				}break;
				case TYPE_DISK_FILE: {
					if (time_diff>=(MAX_DISK_FILE_CACHE_UNUSED_TIME)) {
						if (prev==NULL) {
							objlist=cur->Next();
							cur->Remove(cur);
							delete cur;
							cur=objlist;
						} else {
							cur->Remove(cur);
							delete cur;
							cur=prev->Next();
						}
						continue;
					} //else
					//	printf("Item has %2.2f seconds remaining in memory.\n",MAX_DISK_FILE_CACHE_UNUSED_TIME-time_diff);
				}break;
			}
			prev=cur;
			cur=cur->Next();
		}
		/*
			Cache Capacity Cull
		
			This code should remove files from the cache based on the set cache limits and current
			usage. If the RAM or Disk cache is over its specified limit, then trim the files
			first by age, then by capacity: older files should be removed first, then large newer
			files. This will remove items from disk and/or RAM.
		*/
		ssize_t ram_size=GetCacheSize(TYPE_RAM), disk_size=GetCacheSize(TYPE_DISK);
		CacheObject **objects=NULL;
		cur=objlist;
		int32 items=0;
		
		if (ram_size>=MAX_RAM_CACHE_SIZE) {
			items=CountCacheItems(TYPE_RAM);
			
			if (items>1) {//if a single file is taking up the entire limit, then leave it.
				objects=new CacheObject*[items];//create an array of the CacheObject pointers.
				int32 count=0;
				while (count<items) {
					if (cur->Type()==TYPE_RAM)
						objects[count++]=cur;//populate the array with pointers to RAMCacheObjects
					cur=cur->Next();
				}
				qsort(objects,items,sizeof(CacheObject*),sort_by_size);
				
				delete []objects;
				
			}
			
		}
		if (disk_size>=MAX_DISK_CACHE_SIZE) {
//			printf("counting disk cache items\n");
		
			items=CountCacheItems(TYPE_DISK);
			
			if (items>1) {//if a single file is taking up the entire limit, then leave it.
//				printf("creating cache array\n");
				
				objects=new CacheObject*[items];//create an array of the CacheObject pointers.
				int32 count=0;
//				printf("populating cache array\n");
				
				while (count<items) {
					if (cur->Type()==TYPE_DISK)
						objects[count++]=cur;//populate the array with pointers to RAMCacheObjects
					cur=cur->Next();
				}
//				for (int32 i=0; i<items; i++)
//					printf("\t%ld\t%s\n",i,objects[i]->URL());
				
//				printf("sorting array\n");
				
				qsort(objects,items,sizeof(CacheObject*),sort_by_size);
//				for (int32 i=0; i<items; i++)
//					printf("\t%ld\t%s\n",i,objects[i]->URL());

//				printf("clearing array\n");
				
				delete []objects;
				
			}
		}
		
	}
	
}
int32 cacheman::CountCacheItems(uint32 which) 
{
	int32 count=0;
	
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		CacheObject *cur=objlist;
		while (cur!=NULL) {
			switch(which) {
				case TYPE_RAM:
					if (cur->Type()==TYPE_RAM)
						count++;
					break;
				case TYPE_DISK:
					if (cur->Type()==TYPE_DISK)
						count++;
					break;
				case TYPE_DISK_FILE:
					if (cur->Type()==TYPE_DISK_FILE)
						count++;
					break;
				
				default:
					count++;
				
			}
			
			cur=cur->Next();
		}
			
	}
	return count;
}

status_t cacheman::BroadcastReply(BMessage *msg) 
{
}

CacheObject *cacheman::FindObject(int32 objecttoken) 
{
	CacheObject *cur=objlist;
	while (cur!=NULL) {
		if (cur->Token()==objecttoken)
			break;
		cur=cur->Next();
	}
	return (DiskCacheObject*)cur;
}
CacheObject *cacheman::FindObject(const char *URL) 
{
	CacheObject *cur=objlist;
	while (cur!=NULL) {
		if (strcmp(URL, cur->URL())==0){
			break;
		}
		cur=cur->Next();
	}
	return (DiskCacheObject*)cur;
}

void cacheman::ClearRequests(uint32 usertoken, int32 objecttoken) 
{
	CacheObject *object=FindObject(objecttoken);
	if (object!=NULL) {
		if (object->IsUsedBy(usertoken))
			object->RemoveUser(usertoken);
		if (object->CountUsers()==0) {
			if (object->Previous()!=NULL) {
				object->Previous()->Remove(object);
			} else {
				objlist=object->Next();
				objlist->SetPrevious(NULL);
			}
			delete object;
		}
	}
}
void cacheman::ClearAllRequests(uint32 usertoken) 
{
	CacheObject *object=objlist;
	while (object!=NULL) {
		if (object->IsUsedBy(usertoken))
			object->RemoveUser(usertoken);
		object=object->Next();
	}
	
}
void cacheman::ClearCache(uint32 which) 
{
/*
	mmsg->PrintToStream();
	printf("Clearing cache...\n");
	bool specific=false;
	BString url;
	if (mmsg->HasString("url")) {
		mmsg->FindString("url",&url);
		specific=true;
	}
*/	
	CacheObject *object=objlist;
	while(object!=NULL) {
		object->ClearFile();
		objlist=object->Next();
		delete object;
		object=objlist;
	}
	
	BVolumeRoster volr;
	BVolume vol;
	for (int i=0;i<5;i++)
	//the query repeats 5 times to make sure all appropriate files are removed
	{
		while (volr.GetNextVolume(&vol)==B_OK)
		{
			BQuery query;
			query.Clear();
			query.SetVolume(&vol);
			query.PushAttr("BEOS:TYPE");
			query.PushString(ThemisCacheMIME);
			query.PushOp(B_EQ);
/*
			if (specific)
			{
				query.PushAttr("Themis:URL");
				query.PushString(url.String());
				query.PushOp(B_EQ);
				query.PushOp(B_AND);
			}
*/			
			query.Fetch();
			snooze(100000);
			BEntry ent;
			char fname[B_FILE_NAME_LENGTH];
			while (query.GetNextEntry(&ent)==B_OK)
			{
				ent.GetName(fname);
				printf("\t\t%s: ",fname);
				if ((ent.InitCheck()==B_OK) && (ent.Exists()))
				{
					if(ent.Remove()==B_OK)
						printf("removed\n");
					else
						printf("error\n");
				}
			}
		}
		volr.Rewind();
	}
}
void cacheman::ClearContent(uint32 usertoken, int32 objecttoken) {
	CacheObject *object=FindObject(objecttoken);
	if (object!=NULL) {
		object->ClearContent(usertoken);
	}
	
}

BMessage *cacheman::GetInfo(uint32 usertoken, int32 objecttoken){
	CacheObject *object=FindObject(objecttoken);
	if (object!=NULL) {
		return object->GetInfo();
	}
	
	return NULL;
}


ssize_t cacheman::Write(uint32 usertoken, int32 objecttoken, void *data, size_t size){
	BAutolock alock(lock);
	if (alock.IsLocked()) {
	CacheObject *object=FindObject(objecttoken);
	if (object!=NULL) {
		return object->Write(usertoken,data,size);
	}
	}
	return B_ERROR;
}

ssize_t cacheman::Read(uint32 usertoken, int32 objecttoken, void *data, size_t size){
	BAutolock alock(lock);
	if (alock.IsLocked()) {
	CacheObject *object=FindObject(objecttoken);
	if (object!=NULL) {
		if (!object->IsUsedBy(usertoken))
			object->AddUser(FindUser(usertoken));
		return object->Read(usertoken,data,size);
	}
	}
	return B_ERROR;
}

int32 cacheman::Type() 
{
	return TYPE_DISK|TYPE_RAM|TARGET_CACHE;
}
bool cacheman::AcquireWriteLock(uint32 usertoken, int32 objecttoken) 
{
	CacheObject *object=FindObject(objecttoken);
	if (object!=NULL)
		return object->AcquireWriteLock(usertoken);
	return false;
}
void cacheman::ReleaseWriteLock(uint32 usertoken,int32 objecttoken) 
{
	printf("Releasing WriteLock: user: %lu object: %lu\n",usertoken,objecttoken);
	CacheObject *object=FindObject(objecttoken);
	if (object!=NULL)
		object->ReleaseWriteLock(usertoken);
}

void cacheman::Quit(bool fast) 
{
//	printf("cacheman::Quit(%s)\n",fast?"true":"false");
//	if (!fast) {
		if (AppSettings!=NULL) {
			if (!AppSettings->HasMessage("disk_cache_settings")) {
				AppSettings->AddMessage("disk_cache_settings",CSettings);
			} else {
				AppSettings->ReplaceMessage("disk_cache_settings",CSettings);
			}
		}
//	}
}

status_t cacheman::ReceiveBroadcast(BMessage *msg) 
{
//	printf("Cache ReceiveBroadcast\n");
	uint32 command=0;
	msg->FindInt32("command",(int32*)&command);
	switch(command) {
		case COMMAND_STORE: {
			switch(msg->what) {
				case CreateCacheObject:
				 {
					uint32 cache_type=TYPE_DISK;
					if (msg->HasInt32("cache_type")) {
						cache_type=msg->FindInt32("cache_type");
					}
					BString url;
					msg->FindString("url",&url);
					uint32 usertoken=0;
					usertoken=msg->FindInt32("cache_user_token");
					PlugClass *pobj=NULL;
					uint32 replyid=0;
					if (msg->HasPointer("ReplyToPointer"))
						msg->FindPointer("ReplyToPointer",(void**)&pobj);
					else
						replyid=msg->FindInt32("ReplyTo");
					if ((pobj==NULL) && (replyid!=0))
						pobj=PlugMan->FindPlugin(replyid);
					int32 objecttoken=CreateObject(usertoken,url.String(),cache_type);
					BMessage reply(CachedObject);
					reply.AddInt32("command", COMMAND_INFO);
					reply.AddInt32("cache_object_token",objecttoken);
					pobj->BroadcastReply(&reply);
				}break;				
			}
			
		}break;
		case COMMAND_INFO_REQUEST: {
			switch(msg->what) {

				case FindCachedObject: {
					printf("two\n");
					PlugClass *pobj=NULL;
					uint32 replyid=0;
					uint32 usertoken=0;
					BString url;
					if (msg->HasPointer("broadcaster_pointer"))
						msg->FindPointer("broadcaster_pointer",(void**)&pobj);
					else
						replyid=msg->FindInt32("ReplyTo");
					if ((pobj==NULL) && (replyid!=0))
						pobj=PlugMan->FindPlugin(replyid);
			//		if (msg->HasBool("cache_item")) {
						if (msg->HasInt32("cache_user_token")) {
							usertoken=msg->FindInt32("cache_user_token");
							msg->FindString("url",&url);
							int32 objecttoken=FindObject(usertoken,url.String());
							BMessage reply;
							reply.AddInt32("command",COMMAND_INFO);
//							reply.AddInt32("cache_system",Type());
							if (objecttoken>B_ERROR) {
								reply.what=CachedObject;
								reply.AddInt32("cache_object_token",objecttoken);
							} else {
								reply.what=CacheObjectNotFound;
							}
							printf("cache reply:\n");
							reply.PrintToStream();
							pobj->BroadcastReply(&reply);
							
						}
						
		//			}
					
				}break;
				default:
					return PLUG_DOESNT_HANDLE;
			}
			
		}break;
		default:
			return PLUG_DOESNT_HANDLE;
	}
	return PLUG_HANDLE_GOOD;
}
int32 cacheman::FindObject(uint32 usertoken, const char *URL) 
{
	CacheUser *user=FindUser(usertoken);
	CacheObject *object=FindObject(URL);
	if (object!=NULL) {
		if (!object->IsUsedBy(usertoken))
			object->AddUser(user);
		return object->Token();
	}
	BEntry ent(cachepath.Path(),true);
	struct stat devstat;
	ent.GetStat(&devstat);
	BVolume vol(devstat.st_dev);
	BQuery query;
	query.SetVolume(&vol);
	query.PushAttr("Themis:URL");
	query.PushString(URL,true);//set the second argument to false to make query case sensitive
	query.PushOp(B_EQ);
	/*
		Copy the predicate for future use; we do a quick query to determine if we have
	more than one file that meets the query's specifications, and decide what to do then.
	In general, and in my (Z3R0 One's) opinion, we should only take the first object that
	meets the query's specifications. However, what do we do if we encounter a two URLs
	that differ only by the capitalization of a letter or two? I made this query
	case insensitive because the server name shouldn't matter whether it's capitalized or
	not, however the actual URI generally does matter... What to do, what to do...
	*/
	size_t predlen=0;
	predlen=query.PredicateLength();
	char *predicate=NULL;
	predicate=new char[predlen+1];
	memset(predicate,0,predlen+1);
	query.GetPredicate(predicate,predlen);
	query.Fetch();
	ent.Unset();
	entry_ref ref;
	bool found=false;
	int32 count=0;
	while (query.GetNextEntry(&ent,false)==B_OK) {
		if (trashdir->Contains(&ent))
			continue;
		ent.GetRef(&ref);
		count++;
	}
	if (count>0) {
		query.Clear();
		query.SetPredicate(predicate);
		query.Fetch();
		if (count==1) {
//			query.GetNextRef(&ref);
			BPath path(&ref);
			printf("cache man: found url at %s\n",path.Path());
			object=new DiskCacheObject(object_token_value++,URL, ref);
			object->AddUser(user);
			if (objlist==NULL) {
				objlist=object;
			} else {
				objlist->SetNext(object);
			}
			
		} else {
			//just get the first one... or the best match...
//			query.GetNextRef(&ref);
			object=new DiskCacheObject(object_token_value++,URL, ref);
			object->AddUser(user);
			if (objlist==NULL) {
				objlist=object;
			} else {
				objlist->SetNext(object);
			}
		}
		
	}
	
	memset(predicate,0,predlen);
	delete predicate;
	predicate=NULL;
	printf("Cache: cache object found: %p\n",object);
	if (object!=NULL)
		return object->Token();
	return B_ENTRY_NOT_FOUND;
}
CacheUser *cacheman::FindUser(uint32 usertoken) 
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

int32 cacheman::CreateObject(uint32 usertoken, const char *URL, uint32 type) 
{
	CacheUser *user=FindUser(usertoken);
	CacheObject *object=FindObject(URL);
	if (object!=NULL) {
		if (!object->IsUsedBy(usertoken))
			object->AddUser(user);
		return object->Token();
	}
        bigtime_t reqtime=real_time_clock_usecs();
        BPath pth(cachepath);
        BString fname;
        fname <<reqtime;
        pth.Append(fname.String());
        entry_ref ref;
         {
          BEntry ent(pth.Path(),true);
          ent.GetRef(&ref);
         }
	switch (type) {
		case TYPE_RAM: {
			object=new RAMCacheObject(object_token_value++,URL);
		}break;
		case TYPE_DISK_FILE: {
			object=new DFCacheObject(object_token_value++,URL);
		}break;
		default:{
			object=new DiskCacheObject(object_token_value++,URL,ref);
		}break;
	}
	
	object->AddUser(user);
	object->AcquireWriteLock(usertoken);
	if (objlist==NULL) {
		objlist=object;
	} else {
		objlist->SetNext(object);
	}
	return object->Token();
}
status_t cacheman::UpdateAttr(uint32 usertoken, int32 objecttoken, const char *name, type_code type, void *data, size_t size) 
{
}
status_t cacheman::WriteAttr(uint32 usertoken, int32 objecttoken, const char *attrname, type_code type,void *data,size_t size)
{
	BAutolock alock(lock);
	if (alock.IsLocked()) {
	CacheObject *object=FindObject(objecttoken);
	if (object!=NULL) {
		if (!object->IsUsedBy(usertoken))
			object->AddUser(FindUser(usertoken));
		if (object->HasWriteLock(usertoken))	
			return object->WriteAttr(usertoken,attrname,type,data,size);
	}
	}
	return B_ERROR;
}

status_t cacheman::ReadAttr(uint32 usertoken, int32 objecttoken, const char *attrname, type_code type, void *data, size_t size)
{
	BAutolock alock(lock);
	if (alock.IsLocked()) {
	CacheObject *object=FindObject(objecttoken);
	if (object!=NULL) {
		if (!object->IsUsedBy(usertoken))
			object->AddUser(FindUser(usertoken));
		return object->ReadAttr(usertoken,attrname,type,data,size);
	}
	}
	return B_ERROR;
}
ssize_t cacheman::SetLength(uint32 usertoken, int32 objecttoken, size_t length)
{
	ssize_t size=0;
	
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		CacheObject *object=FindObject(objecttoken);
		if (object!=NULL) {
			if (!object->IsUsedBy(usertoken))
				object->AddUser(FindUser(usertoken));
			if (object->HasWriteLock(usertoken)) {
				size=object->SetLength(usertoken,objecttoken,length);
				
			}
				
		}
	}
	return size;
	
}
status_t cacheman::RemoveObject(uint32 usertoken, int32 objecttoken) 
{
	status_t status=B_ERROR;
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		CacheObject *object=FindObject(objecttoken);
		if (object!=NULL) {
			if (object->HasWriteLock(usertoken)) {
				CacheUser *user=NULL;
				int32 users=object->CountUsers();
				BMessage msg(CACHE_OBJECT_REMOVED);
				msg.AddInt32("command",COMMAND_INFO);
				msg.AddInt32("cache_object_token",objecttoken);
				
				for (int32 i=0; i<object->CountUsers(); i++) {
					user=object->GetUser(i);
					if (user!=NULL)
						Broadcast(user->BroadcastID(),&msg);
					
				}
				
				object->ClearFile();
				delete object;
				
			}
			
		} else {
			status=B_ENTRY_NOT_FOUND;
			
		}
		
		
	}
	return status;
	
}

bool cacheman::HasAttr(uint32 usertoken, int32 objecttoken, const char *name, type_code *type, size_t *size)
{
}


status_t cacheman::CheckMIME()
 {
  //MIME type goodness...
  BMimeType mime(ThemisCacheMIME);
  if (!mime.IsInstalled())
   {
    mime.Install();
    app_info ai;
    be_app->GetAppInfo(&ai);
    mime.SetAppHint(&ai.ref);
   }
  char type[B_MIME_TYPE_LENGTH];
  mime.GetShortDescription(type);
  if (strcasecmp(type,"Themis Cache File")!=0)
   mime.SetShortDescription("Themis Cache File");
  mime.GetLongDescription(type);
  if (strcasecmp(type,"Themis Cache File")!=0)
   mime.SetLongDescription("Themis Cache File");
  mime.GetPreferredApp(type);
  if (strcasecmp(type,ThemisAppSig)!=0)
   mime.SetPreferredApp(ThemisAppSig);
  BMessage attrinf;
  mime.GetAttrInfo(&attrinf);
  bool installall=false;
  int32 attrcount=0;
  if (attrinf.IsEmpty())
   installall=true;
  else
   {
    type_code typec;
    attrinf.GetInfo("attr:name",&typec,&attrcount);
   }
  bool found=false;
  //Make Themis:URL a visible attribute
  for (int32 i=0;i<attrcount;i++)
   {
    BString item;
    attrinf.FindString("attr:name",i,&item);
    if (item=="Themis:URL")
     {
      found=true;
      break;
     }
   }
  if ((!found) || (installall))
   {
    attrinf.AddString("attr:name","Themis:URL");
    attrinf.AddString("attr:public_name","URL");
    attrinf.AddInt32("attr:type",B_STRING_TYPE);
    attrinf.AddInt32("attr:width",200);
    attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
    attrinf.AddBool("attr:public",true);
    attrinf.AddBool("attr:editable",true);
    attrinf.AddBool("attr:viewable",true);
    attrinf.AddBool("attr:extra",false);
   }
  else
   found=false;
  //Make Themis:URL a visible attribute; done
  //Make Themis:mime_type a visible attribute
  for (int32 i=0;i<attrcount;i++)
   {
    BString item;
    attrinf.FindString("attr:name",i,&item);
    if (item=="Themis:mime_type")
     {
      found=true;
      break;
     }
   }
  if ((!found) || (installall))
   {
    attrinf.AddString("attr:name","Themis:mime_type");
    attrinf.AddString("attr:public_name","MIME Type");
    attrinf.AddInt32("attr:type",B_STRING_TYPE);
    attrinf.AddInt32("attr:width",200);
    attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
    attrinf.AddBool("attr:public",true);
    attrinf.AddBool("attr:editable",true);
    attrinf.AddBool("attr:viewable",true);
    attrinf.AddBool("attr:extra",false);
   }
  else
   found=false;
  //Make Themis:mime_type a visible attribute; done
  //Make Themis:name a visible attribute
  for (int32 i=0;i<attrcount;i++)
   {
    BString item;
    attrinf.FindString("attr:name",i,&item);
    if (item=="Themis:name")
     {
      found=true;
      break;
     }
   }
  if ((!found) || (installall))
   {
    attrinf.AddString("attr:name","Themis:name");
    attrinf.AddString("attr:public_name","Real Name");
    attrinf.AddInt32("attr:type",B_STRING_TYPE);
    attrinf.AddInt32("attr:width",200);
    attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
    attrinf.AddBool("attr:public",true);
    attrinf.AddBool("attr:editable",true);
    attrinf.AddBool("attr:viewable",true);
    attrinf.AddBool("attr:extra",false);
   }
  else
   found=false;
  //Make Themis:name a visible attribute; done
  //Make Themis:host a visible attribute
  for (int32 i=0;i<attrcount;i++)
   {
    BString item;
    attrinf.FindString("attr:name",i,&item);
    if (item=="Themis:host")
     {
      found=true;
      break;
     }
   }
  if ((!found) || (installall))
   {
    attrinf.AddString("attr:name","Themis:host");
    attrinf.AddString("attr:public_name","Host Name");
    attrinf.AddInt32("attr:type",B_STRING_TYPE);
    attrinf.AddInt32("attr:width",200);
    attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
    attrinf.AddBool("attr:public",true);
    attrinf.AddBool("attr:editable",true);
    attrinf.AddBool("attr:viewable",true);
    attrinf.AddBool("attr:extra",false);
   }
  else
   found=false;
  //Make Themis:host a visible attribute; done
  //Make Themis:path a visible attribute
  for (int32 i=0;i<attrcount;i++)
   {
    BString item;
    attrinf.FindString("attr:name",i,&item);
    if (item=="Themis:path")
     {
      found=true;
      break;
     }
   }
  if ((!found) || (installall))
   {
    attrinf.AddString("attr:name","Themis:path");
    attrinf.AddString("attr:public_name","Server Path");
    attrinf.AddInt32("attr:type",B_STRING_TYPE);
    attrinf.AddInt32("attr:width",200);
    attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
    attrinf.AddBool("attr:public",true);
    attrinf.AddBool("attr:editable",true);
    attrinf.AddBool("attr:viewable",true);
    attrinf.AddBool("attr:extra",false);
   }
  else
   found=false;
  //Make Themis:path a visible attribute; done
  //Make Themis:etag a visible attribute
  for (int32 i=0;i<attrcount;i++)
   {
    BString item;
    attrinf.FindString("attr:name",i,&item);
    if (item=="Themis:etag")
     {
      found=true;
      break;
     }
   }
  if ((!found) || (installall))
   {
    attrinf.AddString("attr:name","Themis:etag");
    attrinf.AddString("attr:public_name","Entity Tag");
    attrinf.AddInt32("attr:type",B_STRING_TYPE);
    attrinf.AddInt32("attr:width",200);
    attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
    attrinf.AddBool("attr:public",true);
    attrinf.AddBool("attr:editable",false);
    attrinf.AddBool("attr:viewable",true);
    attrinf.AddBool("attr:extra",false);
   }
  else
   found=false;
  //Make Themis:etag a visible attribute; done
    //Make Themis:last-modified a visible attribute
  for (int32 i=0;i<attrcount;i++)
   {
    BString item;
    attrinf.FindString("attr:name",i,&item);
    if (item=="Themis:last-modified")
     {
      found=true;
      break;
     }
   }
  if ((!found) || (installall))
   {
    attrinf.AddString("attr:name","Themis:last-modified");
    attrinf.AddString("attr:public_name","Last Modified");
    attrinf.AddInt32("attr:type",B_STRING_TYPE);
    attrinf.AddInt32("attr:width",200);
    attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
    attrinf.AddBool("attr:public",true);
    attrinf.AddBool("attr:editable",false);
    attrinf.AddBool("attr:viewable",true);
    attrinf.AddBool("attr:extra",false);
   }
  else
   found=false;
  //Make Themis:last-modified a visible attribute; done
  //Make Themis:content-length a visible attribute
  for (int32 i=0;i<attrcount;i++)
   {
    BString item;
    attrinf.FindString("attr:name",i,&item);
    if (item=="Themis:content-length")
     {
      found=true;
      break;
     }
   }
  if ((!found) || (installall))
   {
    attrinf.AddString("attr:name","Themis:content-length");
    attrinf.AddString("attr:public_name","Content Length");
    attrinf.AddInt32("attr:type",B_INT64_TYPE);
    attrinf.AddInt32("attr:width",50);
    attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
    attrinf.AddBool("attr:public",true);
    attrinf.AddBool("attr:editable",false);
    attrinf.AddBool("attr:viewable",true);
    attrinf.AddBool("attr:extra",false);
   }
  else
   found=false;
  //Make Themis:content-length a visible attribute; done
  //Make Themis:expires a visible attribute
  for (int32 i=0;i<attrcount;i++)
   {
    BString item;
    attrinf.FindString("attr:name",i,&item);
    if (item=="Themis:expires")
     {
      found=true;
      break;
     }
   }
  if ((!found) || (installall))
   {
    attrinf.AddString("attr:name","Themis:expires");
    attrinf.AddString("attr:public_name","Expiry");
    attrinf.AddInt32("attr:type",B_INT64_TYPE);
    attrinf.AddInt32("attr:width",200);
    attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
    attrinf.AddBool("attr:public",true);
    attrinf.AddBool("attr:editable",false);
    attrinf.AddBool("attr:viewable",true);
    attrinf.AddBool("attr:extra",false);
   }
  else
   found=false;
  //Make Themis:expires a visible attribute; done
  //Make Themis:content-md5 a visible attribute
  for (int32 i=0;i<attrcount;i++)
   {
    BString item;
    attrinf.FindString("attr:name",i,&item);
    if (item=="Themis:content-md5")
     {
      found=true;
      break;
     }
   }
  if ((!found) || (installall))
   {
    attrinf.AddString("attr:name","Themis:content-md5");
    attrinf.AddString("attr:public_name","MD5 Checksum");
    attrinf.AddInt32("attr:type",B_INT64_TYPE);
    attrinf.AddInt32("attr:width",200);
    attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
    attrinf.AddBool("attr:public",true);
    attrinf.AddBool("attr:editable",false);
    attrinf.AddBool("attr:viewable",true);
    attrinf.AddBool("attr:extra",false);
   }
  else
   found=false;
  
  //Make Themis:content-md5 a visible attribute; done
  //Make Themis:clearonnew a visible attribute
  for (int32 i=0;i<attrcount;i++)
   {
    BString item;
    attrinf.FindString("attr:name",i,&item);
    if (item=="Themis:clearonnew")
     {
      found=true;
      break;
     }
   }
  if ((!found) || (installall))
   {
    attrinf.AddString("attr:name","Themis:clearonnew");
    attrinf.AddString("attr:public_name","Clear On New Page");
    attrinf.AddInt32("attr:type",B_INT32_TYPE);
    attrinf.AddInt32("attr:width",20);
    attrinf.AddInt32("attr:alignment",B_ALIGN_CENTER);
    attrinf.AddBool("attr:public",false);
    attrinf.AddBool("attr:editable",false);
    attrinf.AddBool("attr:viewable",true);
    attrinf.AddBool("attr:extra",false);
   }
  else
   found=false;
  
  //Make Themis:clearonnew a visible attribute; done
	type_code code=B_STRING_TYPE;
	int32 count=0;
	attrinf.GetInfo("type",&code,&count);
	if (code==B_STRING_TYPE) {
		for (int i=1; i<count;i++)
			attrinf.RemoveData("type",1);
	}
//  attrinf.PrintToStream();
  mime.SetAttrInfo(&attrinf);
  return B_OK;
 }
status_t cacheman::CheckIndices()
 {
//  status_t stat;
  BVolumeRoster *volr=new BVolumeRoster;
  BVolume vol;
  dirent *ent;
  DIR *d;
  char voln[256];
  bool found;
  while (volr->GetNextVolume(&vol)==B_NO_ERROR)
   {
    if ((vol.KnowsQuery()) && (vol.KnowsAttr()) && (!vol.IsReadOnly()))
     {
      memset(voln,0,256);
      vol.GetName(voln);
      printf("Checking %s...\n",voln);
//      fflush(stdout);
      d=fs_open_index_dir(vol.Device());
      if (!d)
       {
        printf("\tcouldn't open index directory.\n");
        continue;
       }
      found=false;
      //duplicate and modify the next block as necessary to add more indices
      //begin block
      //Themis:URL is where the file was originally found
      printf("\t\tlooking for \"Themis:URL\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:URL")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:URL",B_STRING_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:mime_type is the file's real mime_type.
      printf("\t\tlooking for \"Themis:mime_type\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:mime_type")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:mime_type",B_STRING_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:name is the file's actual name, as stored on the server
      //minus the remainder of the URL.
      printf("\t\tlooking for \"Themis:name\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:name")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:name",B_STRING_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:host is the server name or ip address where the resource was
      //found.
      printf("\t\tlooking for \"Themis:host\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:host")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:host",B_STRING_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:path is the path (minus filename) on the server where
      //the file was originally found.
      printf("\t\tlooking for \"Themis:path\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:path")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:path",B_STRING_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:etag 
      printf("\t\tlooking for \"Themis:etag\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:etag")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:etag",B_STRING_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
       //begin block
      //Themis:last-modified is the file's real mime_type.
      printf("\t\tlooking for \"Themis:last-modified\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:last-modified")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:last-modified",B_STRING_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:content-length is the file's real mime_type.
      printf("\t\tlooking for \"Themis:mime_type\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:content-length")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:content-length",B_INT64_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:expires is the file's real mime_type.
      printf("\t\tlooking for \"Themis:expires\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:expires")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:expires",B_STRING_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:content-md5 is the file's real mime_type.
      printf("\t\tlooking for \"Themis:md5\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:content-md5")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:content-md5",B_STRING_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
      //begin block
      //Themis:clearonnew is the file's real mime_type.
      printf("\t\tlooking for \"Themis:clearonnew\" index...");
      fflush(stdout);
      while((ent=fs_read_index_dir(d)))
       {
        if (strcasecmp(ent->d_name,"Themis:clearonnew")==0)
         {
          printf("found it.\n");
          found=true;
          break;
         }
       }
      if (!found)
       {
        printf("created it.\n");
        fs_create_index(vol.Device(),"Themis:clearonnew",B_INT32_TYPE,0);
       }
      fs_rewind_index_dir(d);
      found=false;
      //end block
    fs_close_index_dir(d);
   }
   }
  delete volr;
  return B_OK;
 }
status_t cacheman::FindCacheDir()
 {
	status_t stat=B_ERROR;
	if (AppSettings!=NULL) {
		BPath path;
		BString temp;
		FindCache_SetEntry1:
		AppSettings->FindString(kPrefsSettingsDirectory,&temp);
		if (temp.Length()>0) {
			path.SetTo(temp.String());
			path.Append("cache");
			FindCache_SetEntry2:
			BEntry entry(path.Path(),true);
			if (entry.Exists()) {
				if (entry.IsDirectory()) {
					goto FindCache_SetPath;
				} else {
					BString t2=temp;
					t2<<"cache"<<time(NULL);
					path.SetTo(t2.String());
					goto FindCache_SetEntry2;
				}
			} else {
				FindCache_CreateDirectory:
				stat=create_directory(path.Path(),0700);
				FindCache_SetPath:
				cachepath=path;
				stat=B_OK;
			}
		} else {
			temp="/boot/home/config/settings/Themis/";
			goto FindCache_SetEntry1;
		}
	}
  /*
  if ((stat=find_directory(B_USER_SETTINGS_DIRECTORY,&path))==B_OK)
   {
	path.Append("Themis");
	FindCache_SetEntry:
	BEntry entry(path.Path(),true);
	if (entry.Exists()) {
		if (entry.IsDirectory()) {
			path.Append("cache");
			FindCache_SetEntry2:
			entry.SetTo(path.Path(),true);
			if (entry.Exists()) {
				if (entry.IsDirectory()) {
					goto FindCache_SetPath;
				} else {
					path.GetParent(&path);
					path.Append("_cache_");
					goto FindCache_SetEntry2;
				}
				
			} else {
				goto FindCache_CreateDirectory;
			}
			
		} else {
			path.GetParent(&path);
			path.Append("ThemisWeb");
			goto FindCache_SetEntry;
		}
		
	} else {
		path.Append("cache");
		FindCache_CreateDirectory:
		stat=create_directory(path.Path(),0700);
		FindCache_SetPath:
		cachepath=path;
		stat=B_OK;
	}
   }
   */
  return stat; 
 }
ssize_t cacheman::GetObjectSize(uint32 usertoken, int32 objecttoken){
	CacheObject *object=FindObject(objecttoken);
	if (object!=NULL)
		return object->Size();
}

ssize_t cacheman::GetCacheSize(uint32 which){
	off_t totalsize=0;
	CacheObject *object=objlist,*last=NULL;
	uint32 type=0;
	switch(which) {
		case TYPE_DISK:
			type=TYPE_DISK;
			
			break;
		case TYPE_RAM:
			type=TYPE_RAM;
			break;
		case TYPE_DISK_FILE:
			type=TYPE_DISK_FILE;
			break;
		
		default:
			type=TYPE_DISK|TYPE_RAM|TYPE_DISK_FILE;
	}
	if ((type&TYPE_DISK)!=0) {
		BEntry ent(cachepath.Path(),true);
		struct stat devstat;
		ent.GetStat(&devstat);
		BVolume vol(devstat.st_dev);
		BQuery query;
		query.SetVolume(&vol);
		query.PushAttr("BEOS:TYPE");
		query.PushString(ThemisCacheMIME);
		query.PushOp(B_EQ);
		query.Fetch();
		entry_ref ref;
		while (query.GetNextEntry(&ent)==B_OK) {
			if (trashdir->Contains(&ent))
				continue;
			ent.GetRef(&ref);
			ent.Unset();
			if (object==NULL) {
				object=new DiskCacheObject(TemporaryObjectID,NULL,ref);
			} else {
				object->SetNext(new DiskCacheObject(TemporaryObjectID,NULL,ref));
			}
		}
	}
	
	while (object!=NULL) {
		switch(type) {
			case TYPE_RAM:
				if (object->Type()==TYPE_RAM)
					totalsize+=object->Size();
				break;
			case TYPE_DISK:
				if (object->Type()==TYPE_DISK)
					totalsize+=object->Size();
					break;
			case TYPE_DISK_FILE:
				if (object->Type()==TYPE_DISK_FILE)
					totalsize+=object->Size();
					break;
			
			default:
				totalsize+=object->Size();
		}
		object=object->Next();
	}
	object=objlist;
	last=NULL;
	while (object!=NULL) {
		if (object->Token()==TemporaryObjectID) {
			if (last==NULL) {
				objlist=object->Next();
				delete object;
				object=objlist;
			} else {
				last->Remove(object);
			delete object;
			object=last->Next();
			}
			continue;
		}
		
		last=object;
		object=object->Next();
	}
	return (ssize_t)totalsize;
}

status_t cacheman::SetObjectAttr(uint32 usertoken,int32 objecttoken, BMessage *info){
	CacheObject *object=FindObject(objecttoken);
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		if (object!=NULL) {
			if (object->HasWriteLock(usertoken)) {
				status_t status=B_OK;
				if (info!=NULL) {
					printf("cacheman::SetObjectAttr\n");
					info->PrintToStream();
					type_code type;
					int32 ecount=0, tcount=0, index=0;
					char *name=NULL;
					ecount=info->CountNames(B_ANY_TYPE);
					BString aname,str;
					if (ecount>=1) {
	#if (B_BEOS_VERSION > 0x0504)
						for (index=0;info->GetInfo(B_ANY_TYPE,index,(const char**)&name,&type,&tcount)==B_OK;index++)
	#else
						for (index=0;info->GetInfo(B_ANY_TYPE,index,&name,&type,&tcount)==B_OK;index++)
	#endif
							{
								for (int32 i=0; i<tcount; i++) {
									switch(type) {
										case B_INT64_TYPE: {
											if (strcasecmp(name,"content-length")==0) {
												off_t clen=0L;
												info->FindInt64(name,i,&clen);
												object->WriteAttr(usertoken,"Themis:content-length",type,&clen,sizeof(clen));
											}
											
										}break;
										case B_STRING_TYPE:{
											str.Truncate(0);
											aname.Truncate(0);
											info->FindString(name,i,&str);
											if (strcasecmp(name,"url")==0) {
												aname="Themis:URL";
											}
											if (strcasecmp(name,"name")==0) {
												aname="Themis:name";
											}
											if (strcasecmp(name,"host")==0) {
												aname="Themis:host";
											}
											if ((strcasecmp(name,"content-type")==0) || (strcasecmp(name,"mime-type")==0)) {
												aname="Themis:mime_type";
											}
											if (strcasecmp(name,"path")==0) {
												aname="Themis:path";
											}
											if (strcasecmp(name,"etag")==0) {
												aname="Themis:etag";
											}
											if (strcasecmp(name,"last-modified")==0) {
												aname="Themis:last-modified";
											}
											if (strcasecmp(name,"expires")==0) {
												aname="Themis:expires";
											}
											if (strcasecmp(name,"content-md5")==0) {
												aname="Themis:content-md5";
											}
											printf("writing \"%s\":%s\n",aname.String(),str.String());
											object->WriteAttr(usertoken,aname.String(),type,(char*)str.String(),str.Length()+1);
										}break;
									}
									
								}
								
							}
	
					}
					
				} else
					status=B_ERROR;
				return status;
			}
			
		}
	}
	return B_ERROR;
}
off_t cacheman::SaveToDisk(uint32 usertoken, int32 objecttoken, entry_ref target_ref,bool overwrite)
{
	off_t bytes_written=0L;
	
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		CacheObject *object=FindObject(objecttoken);
		if (object!=NULL) {
			int32 write_opts=B_WRITE_ONLY|B_CREATE_FILE;
			if (overwrite)
				write_opts|=B_ERASE_FILE;
			else
				write_opts|=B_FAIL_IF_EXISTS;
			
			BFile *file=new BFile(&target_ref,write_opts);
			if ((file!=NULL) && (file->InitCheck()==B_OK)) {
				unsigned char *buff=new unsigned char[65536];
				off_t bytes_read=0L;
				off_t file_size=0L;
				file_size=object->Size();
				ssize_t chunk=0L;
				
				while (bytes_read<file_size) {
					memset(buff,0,65536);
					chunk=Read(usertoken,objecttoken,buff,65536);
					bytes_read+=chunk;
					bytes_written+=file->Write(buff,chunk);
				}
				file->Sync();
				delete file;
				
				memset(buff,0,65536);
				
				delete buff;
			} else {
				if (file!=NULL)
					delete file;
			}
			
			
		}
		
	}
	return bytes_written;
	
}

off_t cacheman::SaveToDisk(uint32 usertoken, int32 objecttoken, BPositionIO *target_obj, off_t offset)
{
	off_t bytes_written=0L;
	
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		CacheObject *object=FindObject(objecttoken);
		if (object!=NULL) {
			if (target_obj!=NULL) {
				unsigned char *buff=new unsigned char[65536];
				off_t bytes_read=0L;
				off_t file_size=0L;
				file_size=object->Size();
				ssize_t chunk=0L;
				if (offset==-1)
					target_obj->Seek(0,SEEK_END);
				else
					target_obj->Seek(offset,SEEK_SET);
				
				while (bytes_read<file_size) {
					memset(buff,0,65536);
					chunk=Read(usertoken,objecttoken,buff,65536);
					bytes_read+=chunk;
					bytes_written+=target_obj->Write(buff,chunk);
				}
				
				memset(buff,0,65536);
				
				delete buff;
			} 			
			
		}
		
	}
	return bytes_written;
}

off_t cacheman::SaveToDisk(uint32 usertoken, int32 objecttoken, const char *filepath, bool overwrite)
{
	printf("CACHE MAN:: SAVE TO DISK 3!!!\n");
	
	off_t bytes_written=0L;
	
	BAutolock alock(lock);
	if (alock.IsLocked()) {
		CacheObject *object=FindObject(objecttoken);
		if (object!=NULL) {
			int32 write_opts=B_WRITE_ONLY|B_CREATE_FILE;
			if (overwrite)
				write_opts|=B_ERASE_FILE;
			else
				write_opts|=B_FAIL_IF_EXISTS;
			
			BFile *file=new BFile(filepath,write_opts);
			if ((file!=NULL) && (file->InitCheck()==B_OK)) {
				unsigned char *buff=new unsigned char[65536];
				off_t bytes_read=0L;
				off_t file_size=0L;
				file_size=object->Size();
				ssize_t chunk=0L;
				printf("CACHE MAN:: WRITING FILE!!\n");
				
				while (bytes_read<file_size) {
					memset(buff,0,65536);
					printf("Reading data...\n");
					chunk=0;
					
					chunk=Read(usertoken,objecttoken,buff,65536);
					printf("%d bytes read.\n");
					
					if (chunk>0) {
						
					bytes_read+=chunk;
					bytes_written+=file->Write(buff,chunk);
					printf("\t%Ld bytes written\n",bytes_written);
					} else
						break;
					
					
				}
				printf("DONE WRITING FILE!\n");
				
				file->Sync();
				delete file;
				
				memset(buff,0,65536);
				
				delete buff;
			} else {
				if (file!=NULL)
					delete file;
			}
			
			
		}
		
	}
	return bytes_written;
}
BPositionIO *cacheman::ObjectIOPointer(uint32 usertoken, int32 objecttoken)
{
	BPositionIO *io_ptr=NULL;
	BAutolock alock(lock);
	
	if (alock.IsLocked()) {
		CacheObject *object=FindObject(objecttoken);
		
		if (object!=NULL) {
			io_ptr=object->IOPointer();
			
		}
	
	}
	return io_ptr;
	
}
		 
