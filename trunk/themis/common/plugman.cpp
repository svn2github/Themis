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

#include "plugman.h"
#include <Path.h>
#include <Directory.h>
#include <storage/FindDirectory.h>
#include <stdio.h>
#include <math.h>
#include <NodeMonitor.h>
#include <String.h>
#include <Node.h>
#include <NodeInfo.h>
//#include "lockres.h"
#include "commondefs.h"
#define CheckFile 'chkf'

void showbits(int32 val) 
{
	for (int32 i=31; i>=0; i--) {
		printf("%d",(val&((int32)pow(2,i)))?1:0);
		fflush(stdout);
	}
}

plugman::plugman(entry_ref &appdirref)
	:BLooper("plug-in manager",B_LOW_PRIORITY) {
	Lock();
	heartcount=0;
	Heartbeat_mr=NULL;	
	head=NULL;
	tail=NULL;
	startup_dir_ref=appdirref;
	InitInfo=new BMessage();
	InitInfo->AddPointer("plug_manager",this);
	BDirectory *dir=new BDirectory(&startup_dir_ref);
	BEntry *entry=new BEntry;
	dir->FindEntry("add-ons",entry,true);
	if (entry->InitCheck()==B_OK) {
		//found (startup)/add-ons/
		if (entry->IsDirectory())
			entry->GetRef(&startup_addon_dir);
	} else {
		BDirectory *subdir=new BDirectory;
		//what the? There isn't an add-on directory in the location where the app
		//started. Create one...
		if (dir->CreateDirectory("add-ons",subdir)==B_OK)
			subdir->FindEntry("./",entry,false);
		delete subdir;
		if (entry->InitCheck()==B_OK)
			entry->GetRef(&startup_addon_dir);
	}
	entry->Unset();
	BPath *path=new BPath;
	if (find_directory(B_USER_ADDONS_DIRECTORY,path,false,NULL)==B_OK){
		dir->SetTo(path->Path());
		dir->FindEntry("Themis",entry,true);
		if (entry->InitCheck()==B_OK) {
			if (entry->IsDirectory())
				entry->GetRef(&user_addon_dir);
		} else {
			//There isn't a "Themis" directory at ~/config/addons/. Create one.
			BDirectory *subdir=new BDirectory;
			if (dir->CreateDirectory("Themis",subdir)==B_OK)
			subdir->FindEntry("./",entry,false);
			delete subdir;
			if (entry->InitCheck()==B_OK)
				entry->GetRef(&user_addon_dir);
		}
	} else {
		//what the? there is no ~/config/add-ons/ directory?
	}
	delete entry;
	printf("Themis Add-on Directories:\n");
	path->SetTo(&startup_addon_dir);
	printf("\t%s\n",path->Path());
	path->SetTo(&user_addon_dir);
	printf("\t%s\n",path->Path());
	printf("Setting up node watching:\n");
	dir->SetTo(&startup_addon_dir);
	node_ref nref;
	dir->GetNodeRef(&nref);
	status_t stat=watch_node(&nref,B_WATCH_DIRECTORY,this,this);
	path->SetTo(&startup_addon_dir);
	dir->GetStatFor(path->Path(),&sad_stat);
	printf("\t%s: %ld\n",path->Path(),stat);
	dir->SetTo(&user_addon_dir);
	dir->GetNodeRef(&nref);
	path->SetTo(&user_addon_dir);
	dir->GetStatFor(path->Path(),&uad_stat);
	stat=watch_node(&nref,B_WATCH_DIRECTORY,this,this);
	printf("\t%s: %ld\n",path->Path(),stat);
	delete path;
	delete dir;

/*
//A bit of test code to make sure that I have the bit mapping correct
	printf("TARGET_CACHE:\t\t%ld\t",TARGET_CACHE);
	showbits(TARGET_CACHE);
	printf("\nTARGET_PROTOCOL:\t%ld\t",TARGET_PROTOCOL);
	showbits(TARGET_PROTOCOL);
	printf("\nTARGET_HANDLER:\t\t%ld\t",TARGET_HANDLER);	
	showbits(TARGET_HANDLER);
	printf("\nTARGET_PARSER:\t\t%ld\t",TARGET_PARSER);
	showbits(TARGET_PARSER);
	printf("\nTARGET_DOM:\t\t%ld\t",TARGET_DOM);
	showbits(TARGET_DOM);
	printf("\nCONTENT_IMAGE:\t\t%ld\t",CONTENT_IMAGE);
	showbits(CONTENT_IMAGE);
	printf("\nCONTENT_TEXT:\t\t%ld\t",CONTENT_TEXT);
	showbits(CONTENT_TEXT);
	printf("\nCONTENT_SCRIPT:\t\t%ld\t",CONTENT_SCRIPT);
	showbits(CONTENT_SCRIPT);
	printf("\nCONTENT_AUDIO:\t\t%d\t",CONTENT_AUDIO);
	showbits(CONTENT_AUDIO);
	printf("\nCONTENT_VIDEO:\t\t%d\t",CONTENT_VIDEO);
	showbits(CONTENT_VIDEO);
	printf("\nCONTENT_COOKIE:\t\t%d\t",CONTENT_COOKIE);
	showbits(CONTENT_COOKIE);
	printf("\nCONTENT_DATA:\t\t%d\t",CONTENT_DATA);
	showbits(CONTENT_DATA);
	printf("\nTYPE_DISK:\t\t%d\t",TYPE_DISK);
	showbits(TYPE_DISK);
	printf("\nTYPE_RAM:\t\t%d\t",TYPE_RAM);
	showbits(TYPE_RAM);
	printf("\nPROTO_HTTP:\t\t%d\t",PROTO_HTTP);
	showbits(PROTO_HTTP);
	printf("\n\nMEMORY_CACHE:\t\t%d\t",MEMORY_CACHE);
	showbits(MEMORY_CACHE);
	printf("\nDISK_CACHE:\t\t%d\t",DISK_CACHE);
	showbits(DISK_CACHE);
	printf("\nHTTP_PROTOCOL:\t\t%d\t",HTTP_PROTOCOL);
	showbits(HTTP_PROTOCOL);
	printf("\nIMAGE_HANDLER:\t\t%d\t",IMAGE_HANDLER);
	showbits(IMAGE_HANDLER);
	printf("\nAUDIO_HANDLER:\t\t%d\t",AUDIO_HANDLER);
	showbits(AUDIO_HANDLER);
	printf("\nVIDEO_HANDLER:\t\t%d\t",VIDEO_HANDLER);
	showbits(VIDEO_HANDLER);
	printf("\nCOOKIE_HANDLER:\t\t%d\t",COOKIE_HANDLER);
	showbits(COOKIE_HANDLER);
	printf("\nJAVASCRIPT_HANDLER:\t%d\t",JAVASCRIPT_HANDLER);
	showbits(JAVASCRIPT_HANDLER);
	printf("\nHTML_PARSER:\t\t%d\t",HTML_PARSER);
	showbits(HTML_PARSER);
	printf("\nTEXT_HANDLER:\t\t%d\t",TEXT_HANDLER);
	showbits(TEXT_HANDLER);
	printf("\n");
*/
	Unlock();
	Run();
				
/*
//bit mapping and broadcast testing continued...
	printf("Building test linked list.\n");
	plugst *tmp;
	head=new plugst;
		head->name="TEXT_HANDLER";
		head->type=TEXT_HANDLER;
		tmp=new plugst;
		head->next=tmp;
		tmp->name="HTML_PARSER";
		tmp->type=HTML_PARSER;
		tmp->next=new plugst;
		tmp=tmp->next;
		tmp->name="JAVASCRIPT_HANDLER";
		tmp->type=JAVASCRIPT_HANDLER;
		tmp->next=new plugst;
		tmp=tmp->next;
		tmp->name="COOKIE_HANDLER";
		tmp->type=COOKIE_HANDLER;
		tmp->next=new plugst;
		tmp=tmp->next;
		tmp->name="VIDEO_HANDLER";
		tmp->type=VIDEO_HANDLER;
		tmp->next=new plugst;
		tmp=tmp->next;
		tmp->name="AUDIO_HANDLER";
		tmp->type=AUDIO_HANDLER;
		tmp->next=new plugst;
		tmp=tmp->next;
		tmp->name="IMAGE_HANDLER";
		tmp->type=IMAGE_HANDLER;
		tmp->next=new plugst;
		tmp=tmp->next;
		tmp->name="HTTP_PROTOCOL";
		tmp->type=HTTP_PROTOCOL;
		tmp->next=new plugst;
		tmp=tmp->next;
		tmp->name="DISK_CACHE";
		tmp->type=DISK_CACHE;
		tmp->next=new plugst;
		tmp=tmp->next;
		tmp->name="MEMORY_CACHE";
		tmp->type=MEMORY_CACHE;
		printf("Done.\nCache handlers:\n");
		Broadcast(TARGET_CACHE,NULL);
		printf("Protocols:\n");
		Broadcast(TARGET_PROTOCOL,NULL);
		printf("Content handlers:\n");
		Broadcast(TARGET_HANDLER,NULL);
		printf("Parsers:\n");
		Broadcast(TARGET_PARSER,NULL);
		printf("Everything:\n");
		Broadcast(ALL_TARGETS,NULL);
*/
}
plugman::~plugman() {
	stop_watching((BHandler *)this,(BLooper*)this);
	
}
bool plugman::QuitRequested() {
	UnloadAllPlugins(true);
	return true;
}
status_t plugman::UnloadAllPlugins(bool clean) {
	plugst *cur=NULL,*tmp=NULL;
	cur=head;
	while (cur!=NULL) {
		if (cur->inmemory) {
			status_t (*Shutdown)(bool);
				printf("Unloading plug-in: %s\n",cur->name);
				if (cur->pobj->IsHandler()) {
					Lock();
					RemoveHandler(cur->pobj->Handler());
					Unlock();
					
				}
				if (cur->pobj->IsLooper()) {
					BLooper *loop=cur->pobj->Looper();
					thread_id thread=loop->Thread();
					BMessenger *msgr=new BMessenger(NULL,loop,NULL);
					msgr->LockTarget();
					msgr->SendMessage(B_QUIT_REQUESTED);
					status_t stat;
					wait_for_thread(thread,&stat);
					delete msgr;
				}
				if (get_image_symbol(cur->sysid,"Shutdown",B_SYMBOL_TYPE_TEXT,(void**)&Shutdown)==B_OK)
					(*Shutdown)(true);
				unload_add_on(cur->sysid);
		} else {
			printf("Unloading plug-in: %s\n",cur->name);
		}
		
		tmp=cur->next;
		delete cur;
		cur=tmp;
	}
	heartcount=0;
	if (Heartbeat_mr!=NULL) {
		delete Heartbeat_mr;
		Heartbeat_mr=NULL;
	}
	
}
status_t plugman::Broadcast(int32 source,int32 targets,BMessage *msg) {
	plugst *cur=head;
	status_t ret=B_OK;
	BMessage *subm=new BMessage;
	bool iloaded=false;
	
	msg->FindMessage("message",subm);
	while (cur!=NULL) {
		if (source==cur->plugid) {
			cur=cur->next;
			continue;
		}
		
		if (targets!=ALL_TARGETS) {
			if ((cur->type&targets)>=1){
				iloaded=false;
				
				if (!cur->inmemory) {
					
					LoadPlugin(cur->plugid);
					iloaded=true;
					
				}
				
				ret=cur->pobj->ReceiveBroadcast(subm);
						if (ret!=PLUG_HANDLE_GOOD)
							if (iloaded==true)
								UnloadPlugin(cur->plugid);

				printf("Broadcast sent to: %s\t%ld\n",cur->pobj->PlugName(),ret);
			}
		} else {
			if (!cur->inmemory)
				LoadPlugin(cur->plugid);
				printf("Broadcast sent to: %s\t%ld\n",cur->pobj->PlugName(),ret);
			cur->pobj->ReceiveBroadcast(subm);
		}
		cur=cur->next; 
	}
	BMessenger *msgr=NULL;
	//send broadcast to other non-plugin targets
	if ((source!=TARGET_DOM) && ((targets&TARGET_DOM!=0) || (targets&ALL_TARGETS!=0))) {
		//send broadcast to DOM object.
	}
	if ((source!=TARGET_WINDOW) && ((targets&TARGET_WINDOW!=0) || (targets&ALL_TARGETS!=0))) {
		//send broadcast to Window object.
	}
	if ((source!=TARGET_VIEW) && ((targets&TARGET_VIEW!=0) || (targets&ALL_TARGETS!=0))) {
		//send broadcast to [top] View object.
	}
	if ((source!=TARGET_APPLICATION) && ((targets&TARGET_APPLICATION!=0) || (targets&ALL_TARGETS!=0))) {
		//send broadcast to App object.
		be_app_messenger.SendMessage(subm);
	}
	
	delete subm;
	return ret;
	
}
status_t plugman::LoadPlugin(uint32 which) 
{
	plugst *cur=head;//,*tmp;
	bool found=false;
	while (cur!=NULL) {
		if (cur->plugid==which) {
			if (!cur->inmemory) {
				cur->sysid=load_add_on(cur->path);
				if (cur->sysid<=B_ERROR) {
					return B_ERROR;
				}
				cur->inmemory=true;
				BEntry *ent=new BEntry(&cur->ref);
				ent->GetModificationTime(&cur->mod_time);
				ent->Unset();
				delete ent;
				status_t (*Initialize)(void *info);
				get_image_symbol(cur->sysid,"Initialize",B_SYMBOL_TYPE_TEXT,(void**)&Initialize);
				get_image_symbol(cur->sysid,"GetObject",B_SYMBOL_TYPE_TEXT,(void**)&(cur->GetObject));
				(*Initialize)(InitInfo);
				cur->pobj=(*cur->GetObject)();
				cur->uses_heartbeat=cur->pobj->RequiresHeartbeat();
				
				if (cur->uses_heartbeat) {
					if (Heartbeat_mr==NULL) {
						BMessenger *msgr=new BMessenger(this,NULL,NULL);
						BMessage *msg=new BMessage(HeartbeatMessage);
						Heartbeat_mr=new BMessageRunner(*msgr,msg,10000000,-1);
						delete msgr;
						delete msg;
					}
					atomic_add(&heartcount,1);
				}
				
			} else {
			//plugin is already in memory
			}
			found=true;
			break;
		}
		cur=cur->next;
	}
	return (found ? B_OK:B_ERROR);
}
void *plugman::FindPlugin(node_ref &nref) {
	plugst *tmp=head;
	while (tmp!=NULL) {
		if (tmp->nref==nref)
			break;
		tmp=tmp->next;
	}
	return tmp;
 }

PlugClass *plugman::FindPlugin(uint32 which) 
{
	plugst *tmp=head;
	while(tmp!=NULL) {
		if ((tmp->plugid)==which) {
			printf("plugman::FindPlugin: found it\n");
			break;
		}
		tmp=tmp->next;
	}
	if (tmp==NULL)
		return NULL;
	if (!tmp->inmemory) {
		if (LoadPlugin(tmp->plugid)!=B_OK)
			return NULL;
		tmp->inmemory=true;
		if (tmp->pobj->IsHandler())
			AddHandler((BHandler*)tmp->pobj->Handler());
	}
	return tmp->pobj;
}
void plugman::MessageReceived(BMessage *msg) {
	switch(msg->what) {
		case BroadcastMessage: {
			plugst *cur=head;
			int32 targets=0;
			int32 ret=0;
			
			bool iloaded=false;
			
			if (msg->HasInt32("targets")) {
				
			msg->FindInt32("targets",&targets);
			} else {
				//assume everyone is the target...
				targets=ALL_TARGETS;
			}
			
			BMessage *subm=new BMessage;
			BMessage *reply=new BMessage(BroadcastHandledBy);
			
			msg->FindMessage("message",subm);
			while (cur!=NULL) {
				iloaded=false;
				
				if (targets!=ALL_TARGETS) {
					if ((cur->type&targets)>=1){
						if (!cur->inmemory) {
							
							LoadPlugin(cur->plugid);
							iloaded=true;
						}
						
						ret=cur->pobj->ReceiveBroadcast(subm);
						if (ret==PLUG_HANDLE_GOOD) {
							reply->AddPointer("plugobject",cur->pobj);
							
						} else {
							if (iloaded==true)
								UnloadPlugin(cur->plugid);
						}
						
						
						printf("Broadcast sent to: %s\t%ld\n",cur->pobj->PlugName(),ret);
					}
				} else {
					if (!cur->inmemory)
						LoadPlugin(cur->plugid);
					cur->pobj->ReceiveBroadcast(subm);
				}
				cur=cur->next; 
			}
			msg->SendReply(reply,(BHandler*)this);
			delete reply;
			delete subm;
		}break;
		case HeartbeatMessage: {
			printf("Heartbeat\n");
			plugst *cur=head;
			while (cur!=NULL) {
				if (cur->uses_heartbeat) {
					if (cur->pobj!=NULL)
						cur->pobj->Heartbeat();
				}
				cur=cur->next;
			}
			
		}break;
		case AddInitInfo: {
			type_code type;
			int32 count=0,index=0;
			char *name=NULL;
			status_t stat=B_OK;
			BMessage reply;
#if (B_BEOS_VERSION > 0x0504)
			for (index=0;msg->GetInfo(B_ANY_TYPE,index,(const char**)&name,&type,&count)==B_OK; index++)
#else
			for (index=0;msg->GetInfo(B_ANY_TYPE,index,&name,&type,&count)==B_OK; index++)
#endif
				{
					switch(type) {
						case B_BOOL_TYPE: {
							bool value=false;
							for (int i=0; i<count; i++) {
								stat|=msg->FindBool(name,i,&value);
								stat|=InitInfo->AddBool(name,value);
							}
						}break;
						case B_MESSAGE_TYPE: {
							BMessage value;
							for (int i=0; i<count; i++) {
								stat|=msg->FindMessage(name,i,&value);
								stat|=InitInfo->AddMessage(name,&value);
							}
						}break;
						case B_MESSENGER_TYPE: {
							BMessenger value;
							for (int i=0; i<count; i++) {
								stat|=msg->FindMessenger(name,i,&value);
								stat|=InitInfo->AddMessenger(name,value);
							}
						}break;
						case B_POINTER_TYPE: {
							void *value=NULL;
							for (int i=0; i<count; i++) {
								stat|=msg->FindPointer(name,i,&value);
								stat|=InitInfo->AddPointer(name,value);
							}
						}break;
						case B_STRING_TYPE: {
							BString value;
							for (int i=0; i<count; i++) {
								stat|=msg->FindString(name,i,&value);
								stat|=InitInfo->AddString(name,value);
							}
						}break;
						case B_INT8_TYPE: {
							int8 value=0;
							for (int i=0; i<count; i++) {
								stat|=msg->FindInt8(name,i,&value);
								stat|=InitInfo->AddInt8(name,value);
								reply.AddInt32(name,count+1);
							}
						}break;
						case B_INT16_TYPE: {
							int16 value=0;
							for (int i=0; i<count; i++) {
								stat|=msg->FindInt16(name,i,&value);
								stat|=InitInfo->AddInt16(name,value);
							}
						}break;
						case B_INT32_TYPE: {
							if ((strcasecmp(name,"what")==0) || (strcasecmp(name,"when")==0)) {
								continue;
							}
							int32 value=0;
							for (int i=0; i<count; i++) {
								stat|=msg->FindInt32(name,i,&value);
								stat|=InitInfo->AddInt32(name,value);
							}
						}break;
						case B_INT64_TYPE: {
							int64 value=0;
							for (int i=0; i<count; i++) {
								stat|=msg->FindInt64(name,i,&value);
								stat|=InitInfo->AddInt64(name,value);
							}
						}break;
						
					}
					
				}
			
			if (stat!=B_OK)
				stat=B_ERROR;
			reply.what=stat;
			msg->SendReply(&reply);
			printf("Plug-Man: Add Init Info - %ld\n",stat);
			InitInfo->PrintToStream();
			
		}break;
		case RemoveInitInfo: {
		}break;
		case CheckFile: {
			msg->PrintToStream();
			int32 count=0;
			type_code code;
			BNode node;
			BNodeInfo ni;
			entry_ref ref;
			BEntry *ent=new BEntry;
			BPath path;
			char mtype[B_MIME_TYPE_LENGTH];
			msg->GetInfo("refs",&code,&count);
			for (int32 i=0; i<count ; i++) {
				
				msg->FindRef("refs",i,&ref);
				ent->SetTo(&ref,true);
				if (ent->IsDirectory()) {//recurse into directories
					BDirectory *dir=new BDirectory(ent);
					BMessenger *msgr=new BMessenger(NULL,this,NULL);
					entry_ref ref2;
					BMessage *msg2=new BMessage(CheckFile);
					while(dir->GetNextRef(&ref2)==B_OK)
						msg2->AddRef("refs",&ref2);
					msgr->SendMessage(msg2);
					delete msgr;
					delete msg2;
					delete dir;
					continue;
				}
				node.SetTo(&ref);
				ni.SetTo(&node);
				ni.GetType(mtype);
				ni.SetTo(NULL);
				if (strstr(mtype,"executable")==NULL) {
					printf("file isn't executable.\n");
					
					continue;//all add-on's have an executable mimetype
				}
				
				plugst *nuplug=new plugst;
				nuplug->ref=ref;
				node.GetNodeRef(&nuplug->nref);
				ent->GetPath(&path);
				ent->GetModificationTime(&nuplug->mod_time);
				printf("Attempting to load plugin at %s\n",path.Path());
				nuplug->sysid=load_add_on(path.Path());
				if (nuplug->sysid<=B_ERROR) {
					printf("\t\tFailed.\n");
					delete nuplug;
					continue;
				}
				status_t (*Initialize)(void *info);
				if (get_image_symbol(nuplug->sysid,"Initialize",B_SYMBOL_TYPE_TEXT,(void**)&Initialize)==B_OK) {
					printf("\tInitializing...");
					fflush(stdout);
					if ((*Initialize)(InitInfo)!=B_OK) {
						printf("failure, aborting\n");
						unload_add_on(nuplug->sysid);
						delete nuplug;
						continue;
					}
					printf("success\n");
					
				} else {
					printf("\tUnable to load initializer function, aborting.\n");
					unload_add_on(nuplug->sysid);
					delete nuplug;
					continue;
				}
				printf("Attempting to get object...");
				fflush(stdout);
				status_t stat;
				if ((stat=get_image_symbol(nuplug->sysid,"GetObject",B_SYMBOL_TYPE_TEXT,(void**)&(nuplug->GetObject)))==B_OK) {
					nuplug->pobj=(*nuplug->GetObject)();
					if (nuplug->pobj==NULL) {
						printf("plug-in does not have a plug-class object!\n");
						unload_add_on(nuplug->sysid);
						delete nuplug;
						continue;
						
					}
					
					printf("success.\n");
					nuplug->type=nuplug->pobj->Type();
					int32 namlen=strlen(nuplug->pobj->PlugName());
					
					nuplug->name=new char[namlen+1];
					memset(nuplug->name,0,namlen+1);
					strcpy(nuplug->name,nuplug->pobj->PlugName());
					nuplug->uses_heartbeat=nuplug->pobj->RequiresHeartbeat();
					if (nuplug->uses_heartbeat) {
						if (Heartbeat_mr==NULL) {
							BMessenger *msgr=new BMessenger(this,NULL,NULL);
							BMessage *hbmsg=new BMessage(HeartbeatMessage);
							Heartbeat_mr=new BMessageRunner(*msgr,hbmsg,10000000,-1);
							delete msgr;
							delete hbmsg;
						}
						
						atomic_add(&heartcount,1);
					}
					PlugClass *oldplug=NULL;
					
					if ((oldplug=FindPlugin(nuplug->pobj->PlugID()))!=NULL) {
						printf("\tplug-in already loaded.\n");
						
						//compare versions and modification time.
						float oldv=oldplug->PlugVersion(),nuv=nuplug->pobj->PlugVersion();
						
						if (nuv<oldv) {
							
						status_t (*Shutdown)(bool);
						if (get_image_symbol(nuplug->sysid,"Shutdown",B_SYMBOL_TYPE_TEXT,(void**)&Shutdown)==B_OK)
							(*Shutdown)(true);
						else
							printf("plug-in has no Shutdown function; couldn't shutdown nicely.\n");
						unload_add_on(nuplug->sysid);
						nuplug->pobj=NULL;//yeah, I know it's about to be deleted, but...
						delete nuplug;
						continue;
						} else {
							if (nuv==oldv) {
								struct plugst *oldst=head;
								while (oldst!=NULL) {
									if (oldst->pobj==oldplug)
										break;
									oldst=oldst->next;
								}
								//oldst should never be NULL. To be NULL would mean that
								//the plug-in got unloaded while we were doing this processing
								if (oldst->mod_time<=nuplug->mod_time) {
									//get rid of the older one... the new one might have been
									//very recently recompiled.
									UnloadPlugin(oldst->plugid,true);
								} else {
								//unload the new test subject...
									status_t (*Shutdown)(bool);
									if (get_image_symbol(nuplug->sysid,"Shutdown",B_SYMBOL_TYPE_TEXT,(void**)&Shutdown)==B_OK)
										(*Shutdown)(true);
									else
										printf("plug-in has no Shutdown function; couldn't shutdown nicely.\n");
									unload_add_on(nuplug->sysid);
									nuplug->pobj=NULL;//yeah, I know it's about to be deleted, but...
									delete nuplug;
									continue;
								}
								
							} else {
								//nuplug is newer than oldplug
								//unload the old version cleanly
								UnloadPlugin(oldplug->PlugID(),true);
								//then continue on this process to load the new version...
							}
						}
					}
					strcpy(nuplug->path,path.Path());
					nuplug->plugid=nuplug->pobj->PlugID();
					 printf("%c%c%c%c\n",nuplug->pobj->PlugID()>>24,nuplug->pobj->PlugID()>>16,nuplug->pobj->PlugID()>>8,nuplug->pobj->PlugID());
					          printf("Loaded \"%s\" (%c%c%c%c) V. %1.2f\n",nuplug->pobj->PlugName(),
					          nuplug->pobj->PlugID()>>24,nuplug->pobj->PlugID()>>16,nuplug->pobj->PlugID()>>8,nuplug->pobj->PlugID(),
					          nuplug->pobj->PlugVersion());
					if (nuplug->pobj->SecondaryID()!=0)
						printf("\tSecondary ID: %c%c%c%c\n",nuplug->pobj->SecondaryID()>>24,nuplug->pobj->SecondaryID()>>16,nuplug->pobj->SecondaryID()>>8,nuplug->pobj->SecondaryID());
					if (!nuplug->pobj->IsPersistent()) {
						status_t (*Shutdown)(bool);
						printf("Plug-in is not persistent.\n");
						
						if (get_image_symbol(nuplug->sysid,"Shutdown",B_SYMBOL_TYPE_TEXT,(void**)&Shutdown)==B_OK)
							(*Shutdown)(true);
						else
							printf("plug-in has no Shutdown function; couldn't shutdown nicely.\n");
						unload_add_on(nuplug->sysid);//this will probably trigger a crash
						nuplug->inmemory=false;
						nuplug->pobj=NULL;
					} else {
						nuplug->inmemory=true;
						if (nuplug->pobj->IsHandler())
							AddHandler((BHandler*)nuplug->pobj->Handler());
					}
				} else {
					printf("failure, aborting %ld\n",stat);
					unload_add_on(nuplug->sysid);
					delete nuplug;
					continue;
				}
				AddPlug(nuplug);
				printf("\tPlug-in successfully loaded.\n");
				node.Unset();
				ent->Unset();
			}
			delete ent;
		}break;
		case B_NODE_MONITOR: {
			int32 opcode;
			//     dev_t device;
			//      ino_t directory, node;
			const char *name;
			entry_ref ref;
			node_ref nref;
			if (msg->FindInt32("opcode",&opcode)==B_OK) {
				switch(opcode) {
					case B_ENTRY_CREATED: {
						msg->FindInt32("device",&ref.device);
						msg->FindInt64("directory",&ref.directory);
						msg->FindString("name",&name);
						ref.set_name(name);
						BMessage nmsg(CheckFile);
						nmsg.AddRef("refs",&ref);
						PostMessage(&nmsg,this);
					}break;
					case B_ENTRY_REMOVED: {
						msg->FindInt32("device",&nref.device);
						msg->FindInt64("node",&nref.node);
						plugst *tmp=(plugst*)FindPlugin(nref);
						if (tmp!=NULL) {
							printf("Unloading plugin %s\n",tmp->pobj->PlugName());
							UnloadPlugin(tmp->plugid,true);
						}
					}break;
					case B_ENTRY_MOVED: {
						entry_ref oldref,newref,o2,n2;
						msg->FindInt32("device",&oldref.device);
						msg->FindString("name",&name);
						msg->FindInt64("from directory",&oldref.directory);
						msg->FindInt64("to directory",&newref.directory);
						newref.device=oldref.device;
						newref.set_name(name);
						oldref.set_name(name);
						o2=oldref;
						n2=newref;
						
						BPath path(&oldref);
						path.GetParent(&path);
						get_ref_for_path(path.Path(), &oldref);
						path.SetTo(&newref);
						path.GetParent(&path);
						get_ref_for_path(path.Path(), &newref);
						if (((oldref==startup_addon_dir)||(oldref==user_addon_dir))
							&&  ((newref==startup_addon_dir)||(newref==user_addon_dir))) {
							//um... the addon moved from one directory to the other... do nothing.
							printf("add-on jumped from one Themis add-on directory to another... doing nothing.\n");
							return;
						}
							
						if ((oldref==startup_addon_dir)||(oldref==user_addon_dir)) {
							msg->FindInt32("device",&nref.device);
							msg->FindInt64("node",&nref.node);
							plugst *tmp=(plugst*)FindPlugin(nref);
							if (tmp!=NULL) {
								printf("Unloading plugin %s\n",tmp->pobj->PlugName());
								UnloadPlugin(tmp->plugid,true);
							}
						} else {
							if ((newref==startup_addon_dir)||(newref==user_addon_dir)) {
								BMessage nmsg(CheckFile);
								nmsg.AddRef("refs",&n2);
								PostMessage(&nmsg,this);
							}
							
						}
						
					}break;
					
					default: {
						printf("Other opcode. Message:\n");
						msg->PrintToStream();
					}
					
				}
			}
		}break;
		default:
			BLooper::MessageReceived(msg);
	}
	
//	Unlock();
}
status_t plugman::UnloadPlugin(uint32 which,bool clean) {
	printf("PlugMan: Unload plugin %c%c%c%c\n",which>>24,which>>16,which>>8,which);
	
		plugst *cur=head,*prev=NULL;
	while (cur!=NULL) {
		if (cur->plugid==which) {
			if (clean) {
				if (prev==NULL) {
					head=cur->next;
					if (head!=NULL)
						head->prev=NULL;
				} else {
					prev->next=cur->next;
					if (cur->next!=NULL)
						cur->next->prev=prev;
				}
				if (cur->inmemory) {
					status_t (*Shutdown)(bool);
					cur->inmemory=false;
					if (cur->pobj->IsHandler()) {
						Lock();
						RemoveHandler(cur->pobj->Handler());
						Unlock();
						
					}
					if (cur->pobj->IsLooper()) {
						BLooper *loop=cur->pobj->Looper();
						thread_id thread=loop->Thread();
						BMessenger *msgr=new BMessenger(NULL,loop,NULL);
						msgr->LockTarget();
						msgr->SendMessage(B_QUIT_REQUESTED);
						status_t stat;
						wait_for_thread(thread,&stat);
						delete msgr;
					}
					
					if (get_image_symbol(cur->sysid,"Shutdown",B_SYMBOL_TYPE_TEXT,(void**)&Shutdown)==B_OK)
						(*Shutdown)(true);
					if (cur->uses_heartbeat) {
						atomic_add(&heartcount,-1);
						cur->uses_heartbeat=false;
					}
					unload_add_on(cur->sysid);
				}
				delete cur;
				break;
			} else {
				if (cur->inmemory) {
					status_t (*Shutdown)(bool);
					cur->inmemory=false;
					if (cur->pobj->IsHandler()) {
						Lock();
						RemoveHandler(cur->pobj->Handler());
						Unlock();
						
					}
					if (cur->pobj->IsLooper()) {
						BLooper *loop=cur->pobj->Looper();
						thread_id thread=loop->Thread();
						BMessenger *msgr=new BMessenger(NULL,loop,NULL);
						msgr->LockTarget();
						msgr->SendMessage(B_QUIT_REQUESTED);
						status_t stat;
						wait_for_thread(thread,&stat);
						delete msgr;
					}
					if (get_image_symbol(cur->sysid,"Shutdown",B_SYMBOL_TYPE_TEXT,(void**)&Shutdown)==B_OK)
						(*Shutdown)(true);
					if (cur->uses_heartbeat) {
						atomic_add(&heartcount,-1);
						cur->uses_heartbeat=false;
					}
					unload_add_on(cur->sysid);
					cur->pobj=NULL;
				}
				
			}
			
		}
		prev=cur;
		cur=cur->next;
	}
						if (heartcount==0) {
							delete Heartbeat_mr;
							Heartbeat_mr=NULL;
						}
	return B_OK;
}
void plugman::AddPlug(plugst *plug) {
	if (head==NULL) {
		head=plug;
	} else {
		plugst *tmp=head,*last=NULL;
		while (tmp->next!=NULL) {
			last=tmp;
			tmp=tmp->next;
			if (tmp->prev!=last)
				tmp->prev=last;
		}
		tail=plug;
		tmp->next=plug;
		plug->prev=last;
	}
	BMessage *msg=new BMessage(PlugInLoaded);
	msg->AddInt32("type",plug->type);
	msg->AddInt32("command",COMMAND_INFO);
	msg->AddInt32("plugid",plug->plugid);
	if (plug->inmemory)
		msg->AddPointer("plugin",plug->pobj);
	BMessage container;
	container.AddMessage("message",msg);
	Broadcast(TARGET_PLUGMAN,ALL_TARGETS,&container);
	printf("Sending PlugInLoaded message to Window\n");
	
	BMessenger *msgr=new BMessenger(NULL,Window,NULL);
	msgr->SendMessage(msg);
	delete msgr;
	delete msg;
	
}
void plugman::BuildRoster(bool clean) {
	if (clean)
		UnloadAllPlugins(clean);
	entry_ref ref;
	BDirectory *dir=new BDirectory;
	BMessenger *msgr=new BMessenger(NULL,this,NULL);
	BMessage *msg=new BMessage(CheckFile);
	for (int8 i=0; i<2; i++) {
		if (i==0)
			dir->SetTo(&startup_addon_dir);
		else
			dir->SetTo(&user_addon_dir);

		while (dir->GetNextRef(&ref)==B_OK)
			msg->AddRef("refs",&ref);
		dir->Rewind();
	}
//	msgr->LockTarget();
	msgr->SendMessage(msg);
	delete msgr;
	delete msg;
	delete dir;
}
