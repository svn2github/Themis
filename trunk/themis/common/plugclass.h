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

/*
Include *both* plugclass.h *and* plugclass.cpp in your plugin! If necessary,
add a command to makelinks.sh to create this link in your plug-in's directory.
*/

#ifndef _PlugClass
#define _PlugClass

#include <Window.h>
#include <View.h>
#include <Looper.h>
#include <Handler.h>
#include <Message.h>
#include <Entry.h>
#include <OS.h>
#include <MenuBar.h>

#define ProtocolPlugin 'pplg'
#define ContentPlugin 'cplg'
#define HTMLPlugin 'hplg'
#define ImagePlugin 'iplg'
#define MediaPlugin 'mplg'

int32 strtoval(char *proto); //plug-in identifier converter 4 char string to int32

class PlugClass {
	private:
	public:
		PlugClass(BMessage *info=NULL);
		BMessage *InitInfo;
		virtual ~PlugClass();
		
		volatile int32 Cancel;
		virtual uint32 PlugID();
		virtual uint32 SecondaryID();
		/*
		plugid is the plugin's individual id, it's a 4 char constant #define'd.
		For example:
		#define JavaPlugin 'java'
		#define OpenSSLPlugin 'ossl'
		Some types of plugins might have a secondary ID number. In these cases, the
		primary ID number (PlugID) indicates the category of the plugin, and the
		secondary indicates a specific item. For example:
		
		HTML Plugins should return 'html' as the PlugID and a unique identifier
		  for each tag; the "<HTML>" tag plugin might return 'html' for both, while
		  the "<TITLE>" tag plugin might return 'html' and 'titl'.
		*/
		virtual char *PlugName();//returns a pointer to a string constant in each plugin
		virtual float PlugVersion();//returns the plugin version, if applicable
		
		virtual bool NeedsThread();
		virtual int32 SpawnThread(BMessage *info);
		virtual int32 StartThread();
		virtual thread_id Thread();
		virtual void Stop();
		thread_id thread;
		
		BWindow *Window;
		BLooper *PlugMan;
		
		virtual int32 TypePrimary();
		virtual int32 TypeSecondary();
		
		virtual bool IsHandler();//if the plugin has a BHandler object instead
		virtual BHandler *Handler();
		
		virtual bool IsPersistant();//does the plugin load and unload based on page?
		
		virtual bool IsLooper();
		virtual BLooper *Looper();
		virtual void Run();
		
		virtual bool IsView();
		virtual BView *View();
		virtual BView *Parent();
		
		entry_ref *SetRef(entry_ref *nuref);
		entry_ref *Ref();
		entry_ref *ref;//an entry for the plugin
		
		virtual void AddMenuItems(BMenu *menu);//this is a signal for plug-ins to add any menus or items
		virtual void RemoveMenuItems();
		
};

#endif 
