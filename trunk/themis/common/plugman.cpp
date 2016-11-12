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

// Standard C headers
#include <stdio.h>

// BeOS headers
#include <Path.h>
#include <Directory.h>
#include <storage/FindDirectory.h>
#include <NodeMonitor.h>
#include <String.h>
#include <Node.h>
#include <NodeInfo.h>
#include <Message.h>
#include <Messenger.h>
#include <MessageRunner.h>

// Themis headers
#include "commondefs.h"
#include "plugclass.h"
#include "plugman.h"

plugman :: plugman(entry_ref &appdirref,
				   BMessage * aInitInfo)
		: BLooper("plug-in manager",B_LOW_PRIORITY) {

	mHeartBeatMessenger = NULL;
	mHeartBeatMessageRunner = NULL;	
	
	startup_dir_ref=appdirref;
	InitInfo=aInitInfo;
	InitInfo->AddPointer("plug_manager",this);
	BDirectory *dir=new BDirectory(&startup_dir_ref);
	BEntry *entry=new BEntry;
	dir->FindEntry("add-ons",entry,true);
	if (entry->InitCheck()==B_OK) {
		//found (startup)/add-ons/
		if (entry->IsDirectory())
			entry->GetRef(&startup_addon_dir);
	} else {
		BDirectory subdir;
		//what the? There isn't an add-on directory in the location where the app
		//started. Create one...
		if (dir->CreateDirectory("add-ons", &subdir) == B_OK)
			subdir.FindEntry("./", entry, false);
		if (entry->InitCheck() == B_OK)
			entry->GetRef(&startup_addon_dir);
	}
	BPath *path=new BPath;
	entry->GetPath(path);
	LoadDirectory(path->Path());
	entry->Unset();
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
	entry->GetPath(path);
	LoadDirectory(path->Path());
	delete entry;
#ifdef DEBUG
	printf("Themis Add-on Directories:\n");
#endif
	path->SetTo(&startup_addon_dir);
#ifdef DEBUG
	printf("\t%s\n",path->Path());
#endif
	path->SetTo(&user_addon_dir);
#ifdef DEBUG
	printf("\t%s\n",path->Path());
#endif
#ifdef DEBUG
	printf("Setting up node watching on the addon directories\n");
#endif
	dir->SetTo(&startup_addon_dir);
	node_ref nref;
	dir->GetNodeRef(&nref);
	status_t stat=watch_node(&nref,B_WATCH_DIRECTORY,this,this);
//	dir->GetStatFor(path->Path(),&sad_stat);
	dir->SetTo(&user_addon_dir);
	dir->GetNodeRef(&nref);
//	dir->GetStatFor(path->Path(),&uad_stat);
	stat=watch_node(&nref,B_WATCH_DIRECTORY,this,this);
	delete path;
	delete dir;
	Run();
				
}

plugman::~plugman() {
	printf("Stop node watching.\n");
	stop_watching(this, this);
	delete mHeartBeatMessageRunner;
	delete mHeartBeatMessenger;
	printf("Plugin Manager Destructor complete.\n");
}

bool plugman::QuitRequested() {

	printf("Unloading all plugins\n");
	UnloadAllPlugins();
	printf("Done unloading plugins\n");
	return true;
}

void plugman :: LoadPlugin(BEntry & aEntry) {

	BPath path;
	status_t status;
	status = aEntry.GetPath(&path);
	if (status == B_OK) {
		image_id id = load_add_on(path.Path());
		if (id > 0) {
			printf("Succefully loaded file: %s\n", path.Path());
			status_t (*Initialize)(void *info);
			status_t symbolStatus = get_image_symbol(
				id,
				"Initialize",
				B_SYMBOL_TYPE_TEXT,
				(void**)&Initialize);
			if (symbolStatus == B_OK) {
				status_t initializeStatus = (*Initialize)(InitInfo);
				if (initializeStatus == B_OK) {
					printf("Succesfully initialized plugin\n");
					PlugClass * (*GetObject)(void);
					status_t objectStatus = get_image_symbol(
						id,
						"GetObject",
						B_SYMBOL_TYPE_TEXT,
						(void**)&GetObject);
					if (objectStatus == B_OK) {
						PlugClass * plugin = GetObject();
						string pathString(path.Path());
						pair<image_id, PlugClass *> pluginDetails(id, plugin);
						mPlugins.insert(map<string, pair<image_id, PlugClass *> >::value_type(pathString, pluginDetails));
						if (plugin->RequiresHeartbeat() && mHeartBeatMessageRunner == NULL) {
							// Creates the message runner.
							mHeartBeatMessenger = new BMessenger(this, NULL, NULL);
							BMessage message(HeartbeatMessage);
							mHeartBeatMessageRunner = new BMessageRunner(
								*mHeartBeatMessenger,
								&message,
								1000000,
								-1);
						}
						if (plugin->IsHandler()) {
							AddHandler(plugin->Handler());
						}

						printf("Succesfully got object %s\n", plugin->PlugName());
					}
					else {
						printf("Failed to get the object at %s\n", path.Path());
						unload_add_on(id);
					}
				}
				else {
					printf("Failed to initialize the object\n");
					unload_add_on(id);
				}
			}
		}
		else {
			printf("Failed to load file: %s\n", path.Path());
		}
	}
}

void plugman :: LoadDirectory(const char * aDirectory) {

	BDirectory addonDirectory(aDirectory);
	status_t nextEntryStatus = B_OK;
	BEntry entry;
	while (nextEntryStatus == B_OK) {
		nextEntryStatus = addonDirectory.GetNextEntry(&entry, false);
		if (nextEntryStatus == B_OK) {
			LoadPlugin(entry);
		}
	}

}

void plugman :: UnloadPlugin(image_id aId, PlugClass * aPlugin) {

	BMessage quitMessage(B_QUIT_REQUESTED);
	BMessage replyMessage(B_QUIT_REQUESTED);

	if (aPlugin->IsHandler()) {
		RemoveHandler(aPlugin->Handler());
		delete aPlugin;
		
	}
	else if (aPlugin->IsLooper()) {
		BMessenger messenger(NULL, aPlugin->Looper());
		// Use a synchronous message to quit. Don't care about the result?
		messenger.SendMessage(&quitMessage, &replyMessage);
	}
	else {
		status_t (*Shutdown)(bool);
		status_t shutdownStatus = get_image_symbol(
			aId,
			"Shutdown",
			B_SYMBOL_TYPE_TEXT,
			(void**)&Shutdown);
		if (shutdownStatus == B_OK) {
			(*Shutdown)(true);
		}
		else {
			delete aPlugin;
		}
	}
	unload_add_on(aId);

}

void plugman :: UnloadPlugin(const char * aPath) {

	string pathString(aPath);
	map<string, pair<image_id, PlugClass *> >::iterator i = mPlugins.find(pathString);
	UnloadPlugin((*i).second.first, (*i).second.second);

}

void plugman :: UnloadAllPlugins() {

	map<string, pair<image_id, PlugClass *> >::iterator i = mPlugins.begin();
	while (i != mPlugins.end()) {
		UnloadPlugin((*i).second.first, (*i).second.second);
		i++;
	}
	
}

void plugman :: SendHeartBeat() {

	PlugClass * plugin = NULL;

	map<string, pair<image_id, PlugClass *> >::iterator i = mPlugins.begin();
	while (i != mPlugins.end()) {
		plugin = (*i).second.second;
		plugin->Heartbeat();
		i++;
	}
}

PlugClass *plugman::FindPlugin(uint32 aPlugId) {
	
	PlugClass * result = NULL;
	
	map<string, pair<image_id, PlugClass *> >::iterator i = mPlugins.begin();
	while (i != mPlugins.end() && result == NULL) {
		PlugClass * plugin = (*i).second.second;
		if (plugin->PlugID() == aPlugId) {
			result = plugin;
		}
		else {
			i++;
		}
	}
	
	return result;

}

void plugman :: MessageReceived(BMessage *msg) {

	switch(msg->what) {
		case HeartbeatMessage: {
			SendHeartBeat();
		}break;
		case B_NODE_MONITOR: {
			int32 opcode;
			if (msg->FindInt32("opcode",&opcode)==B_OK) {
				const char *name;

				switch(opcode) {
					case B_ENTRY_CREATED: {
						msg->FindString("name", &name);
						BEntry entry(name);
						LoadPlugin(entry);
					}break;
					case B_ENTRY_REMOVED: {
						msg->FindString("name",&name);
						UnloadPlugin(name);
					}break;
					case B_ENTRY_MOVED: {
						entry_ref oldref, newref;
						msg->FindInt32("device",&oldref.device);
						msg->FindString("name",&name);
						msg->FindInt64("from directory",&oldref.directory);
						msg->FindInt64("to directory",&newref.directory);
						newref.device=oldref.device;
						newref.set_name(name);
						oldref.set_name(name);
						
						BPath path(&oldref);
						path.GetParent(&path);
						get_ref_for_path(path.Path(), &oldref);
						path.SetTo(&newref);
						path.GetParent(&path);
						get_ref_for_path(path.Path(), &newref);
						if (((oldref==startup_addon_dir) || (oldref==user_addon_dir))
							&&  ((newref==startup_addon_dir) || (newref==user_addon_dir))) {
							//um... the addon moved from one directory to the other... do nothing.
#ifdef DEBUG
							printf("add-on jumped from one Themis add-on directory to another... doing nothing.\n");
#endif
						}
						else {
							if ((oldref == startup_addon_dir) || (oldref == user_addon_dir)) {
								UnloadPlugin(name);
							} else {
								if ((newref == startup_addon_dir) || (newref == user_addon_dir)) {
									BEntry entry(name);
									LoadPlugin(entry);
								}
								
							}
						}
						
					}break;
					
					default: {
#ifdef DEBUG
						printf("Other opcode. Message:\n");
#endif
//						msg->PrintToStream();
					}
					
				}
			}
		}break;
		default:
			BLooper::MessageReceived(msg);
	}
	
//	Unlock();
}

BMessage * plugman :: GetPluginList() {

	BMessage *msg = new BMessage();
	Lock();
	if (IsLocked()) {
		PlugClass * plugin = NULL;
		
		map<string, pair<image_id, PlugClass *> >::iterator i = mPlugins.begin();
		while (i != mPlugins.end()) {
			plugin = (*i).second.second;
			
			msg->AddString("plug_name", plugin->PlugName());
			msg->AddInt32("plug_id", plugin->PlugID());
			msg->AddString("plug_path", (*i).first.c_str());
			msg->AddBool("plug_in_memory", true);
			msg->AddPointer("plug_object", plugin);
			msg->AddInt32("plug_broadcast_target", plugin->BroadcastTarget());
			i++;
		}

	}
	Unlock();
	return msg;
	
}


