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

class plugman: public BLooper {
	private:
		entry_ref startup_dir_ref;
		entry_ref startup_addon_dir;
		entry_ref user_addon_dir;
		struct stat sad_stat,uad_stat;
		BMessage *InitInfo;
		BMessageRunner *Heartbeat_mr;
		volatile int32 heartcount;
		plugst *head, *tail;
		void AddPlug(plugst *plug);
	public:
		plugman(entry_ref &appdirref);
		~plugman();
		bool QuitRequested();
		status_t UnloadAllPlugins(bool clean=true);
		status_t UnloadPlugin(uint32 which,bool clean=false);
		BWindow *Window;
		PlugClass *FindPlugin(uint32);
		void *FindPlugin(node_ref &nref);
		void MessageReceived(BMessage *msg);
		status_t Broadcast(int32 source,int32 targets,BMessage *msg);
		void BuildRoster(bool clean=true);
		status_t LoadPlugin(uint32 which);
	
};

#endif

