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
//#include <Application.h>
#include <KernelKit.h>
#include <Entry.h>
#include "plugclass.h"

struct plugst
 {
  image_id sysid;
  //sysid is the id assigned to the plugin on load by BeOS.
  uint32 plugid;
  //a copy of the plug id for when it's not loaded.
  volatile bool inmemory;
  //is the plugin currently in memory
  PlugClass *pobj;//plugin object
  PlugClass *(*GetObject)(void);
  entry_ref ref;
  char path[B_PATH_NAME_LENGTH+B_FILE_NAME_LENGTH];
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
  plugst()
   {
    inmemory=false;
    pobj=NULL;
    sysid=0;
    next=prev=NULL;
   }
 };

#endif
