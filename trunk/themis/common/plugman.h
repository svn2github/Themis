/*
Copyright (c) 2000 Z3R0 One. All Rights Reserved.

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

Original Author & Project Manager: Z3R0 One (z3r0_one@yahoo.com)
Project Start Date: October 18, 2000
*/

#ifndef _plugman
#define _plugman
#define PlugManVersion 2.0

#include <AppKit.h>
#include <Looper.h>
#include <KernelKit.h>
#include <SupportKit.h>
#include <StorageKit.h>
#include <Window.h>
#include <MessageRunner.h>
#include "../common/plugstruct.h"
#include "../common/protocol_plugin.h"
//#include "tcplayer.h"

class tcplayer;

class plugman: public BLooper {
	private:
		plugst *head,*tail;
		void AddPlug(plugst *plug);
		int32 heartcount;
		BMessageRunner *Heartbeat_mr;
	public:
		BMessage *InitInfo;
		tcplayer *TCPLayer;
		BMessage **AppSettings;
		BWindow *Window;
		BDirectory *appaddondir,*useraddondir;
		plugman(entry_ref &appdirref);
		~plugman();
		void MessageReceived(BMessage *msg);
		bool QuitRequested();
		void *FindPlugin(uint32 which,uint32 secondary=0);
		void *FindPlugin(node_ref &nref);
		status_t UnloadAllPlugins(bool clean=true);
		status_t UnloadPlugin(uint32 which);
		status_t LoadPlugin(uint32 which);
		status_t LoadPluginFor(const char *mimetype);
		status_t ReloadPlugin(uint32 which);
		status_t BuildRoster(bool clean=true);
};


#endif
