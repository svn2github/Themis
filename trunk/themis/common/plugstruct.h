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

#ifndef _plugstruct
#define _plugstruct
#include <KernelKit.h>
#include <Entry.h>
#include "plugclass.h"
#include <string.h>
/*!

This is the linked list structure used by the plug-in manager plugman.
*/
struct plugst {
	//! the id assigned to the plugin on load by BeOS.
	image_id sysid;
	//! a copy of the plug id for when it's not loaded.
	uint32 plugid;
	//! is the plugin currently in memory
	volatile bool inmemory;
	PlugClass *pobj;//!< plugin object
	PlugClass *(*GetObject)(void); //!< function pointer to retrieve the plug-in's object
	entry_ref ref; //!< entry_ref of the add-on 
	node_ref nref; //!< node_ref of the add-on
	time_t mod_time; //!< the last modified time of the add-on
	char path[B_PATH_NAME_LENGTH+B_FILE_NAME_LENGTH]; //!< Path and file name of the plug-in
	int32 type; //!< The add-on's theoretical project type.
	char *name; //!< the add-on's internal name.
	plugst *next;
	plugst *prev;
	/*
	next and prev will enable a doubly linked list. A bit cumbersome perhaps,
	but it may allow for some performance increases here and there over a singly
	linked list. (For instance, when removing individual plugins from memory,
	it becomes possible to set the previous plugin's next pointer to the current's
	next pointer, and the next's prev pointer to current's prev pointer instead of
	having to loop through again to make sure the previous one knows where the
	next one is. Ex:
	
			previous->next=current->next;
			nextone->prev=current->prev;
			delete current;
	*/
	volatile bool uses_heartbeat; //!< a record of whether or not the plug-in needs a heartbeat
	plugst() {
		plugid=0;
		uses_heartbeat=false;
		inmemory=false;
		pobj=NULL;
		sysid=0;
		next=prev=NULL;
		name=NULL;
		
	}
	~plugst() {
		if (name!=NULL) {
			memset(name,0,strlen(name)+1);
			delete name;
		}
		
	}
	
};

#endif
