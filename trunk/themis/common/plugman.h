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

// Standard C++ headers
#include <map>
#include <string>
#include <utility>

// BeOS headers
#include <Looper.h>
#include <Entry.h>

// Declarations of BeOS classes
class BMessage;
class BMessenger;
class BMessageRunner;

// Declarations of Themis classes
class PlugClass;
class MessageDaemon;
/*!

This class is responsible for loading and unloading plug-ins, and routing "broadcast" messages between
plug-ins and application components.
*/
class plugman : public BLooper {

	private:
		//! The directory from which Themis was started.
		entry_ref startup_dir_ref;
		//! The initial directory in which to look for add-ons.
		entry_ref startup_addon_dir;
		//! The user's add-on directory in the home directory.
		entry_ref user_addon_dir;
		
	//	struct stat sad_stat,uad_stat;
		//! The initialization BMessage that is sent to plug-ins when loaded.
		BMessage *InitInfo;
		//! This is the messenger used to send heartbeat messages
		BMessenger * mHeartBeatMessenger;
		//! This triggers the heartbeat messages that are sent to plug-ins that require it.
		BMessageRunner * mHeartBeatMessageRunner;
		
		map<string, pair<image_id, PlugClass *> > mPlugins;
		
		void LoadPlugin(BEntry & aEntry);
		void LoadDirectory(const char * aDirectory);

		void UnloadPlugin(image_id aId, PlugClass * aPlugin, const char * aPath);
		void UnloadPlugin(const char * aPath);
		//! Unload all plug-ins currently in memory.
		/*!
		
		This function unloads all the plug-ins currently in memory.
		*/
		void UnloadAllPlugins();
		
		void SendHeartBeat();

	public:
		plugman(entry_ref &appdirref, BMessage * aInitInfo);
		~plugman();
		bool QuitRequested();
		//! Locates a particular plug-in.
		PlugClass *FindPlugin(uint32 aPlugId);
		void MessageReceived(BMessage *msg);
		BMessage *GetPluginList();
	
};

#endif

