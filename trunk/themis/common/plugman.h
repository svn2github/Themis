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
#ifndef _plugman
#define _plugman
//! The plug-in manager version.
#define PlugManVersion 3.0

#include <Application.h>
#include <Looper.h>
#include <File.h>
#include <Entry.h>
#include <Window.h>
#include <Message.h>
#include <MessageRunner.h>
#include <Messenger.h>
#include "plugstruct.h"
#include "plugclass.h"
class tcplayer;
class PlugClass;
/*!

This class is responsible for loading and unloading plug-ins, and routing "broadcast" messages between
plug-ins and application components.
*/
class plugman: public BLooper {
	private:
		//! The directory from which Themis was started.
		entry_ref startup_dir_ref;
		//! The initial directory in which to look for add-ons.
		entry_ref startup_addon_dir;
		//! The user's add-on directory in the home directory.
		entry_ref user_addon_dir;
		
		struct stat sad_stat,uad_stat;
		//! The initialization BMessage that is sent to plug-ins when loaded.
		BMessage *InitInfo;
		//! This triggers the heartbeat messages that are sent to plug-ins that require it.
		BMessageRunner *Heartbeat_mr;
		//! This keeps track of how many plug-ins require the heartbeat message.
		volatile int32 heartcount;
		plugst *head, *tail;
		//! Add a plug-in structure to the internal linked list.
		void AddPlug(plugst *plug);
	public:
		plugman(entry_ref &appdirref);
		~plugman();
		bool QuitRequested();
		//! Unload all plug-ins currently in memory.
		/*!
		\param clean
		When this parameter is true, as it is by default, the plug-in linked list is destroyed while unloading
		the plug-ins. Otherwise, the plug-ins are unloaded, but can be easily and quickly reloaded when/if needed.
		
		This function unloads all the plug-ins currently in memory.
		*/
		status_t UnloadAllPlugins(bool clean=true);
		//! Unload a specific plug-in from memory.
		status_t UnloadPlugin(uint32 which,bool clean=false);
		BWindow *Window;
		//! Locates a particular plug-in.
		PlugClass *FindPlugin(uint32);
		//! Locates a particular plug-in via its on disk node_ref.
		void *FindPlugin(node_ref &nref);
		void MessageReceived(BMessage *msg);
		//! Broadcasts a message to one or more plug-ins, and potentially some application portions.
		status_t Broadcast(int32 source,int32 targets,BMessage *msg);
		//! Builds a linked list of add-ons that are in the various add-on directories.
		void BuildRoster(bool clean=true);
		//! Loads a specific plug-in into memory.
		status_t LoadPlugin(uint32 which);
	
};

#endif

