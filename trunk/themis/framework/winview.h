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
#ifndef _mainwinview
#define _mainwinview

#include <View.h>
#include <Menu.h>
#include <MenuItem.h>
#include <MenuBar.h>
#include <SupportKit.h>
#include "appdefines.h"
#include <TextControl.h>
#include <Button.h>
#include "plugman.h"
#include "plugclass.h"
//#include "htmlparser.h"

/*!
\brief Currently, the top level view for any window.

\note At this time, February 9, 2003, this class is the top level view for the Themis
main window. With the eventual integration of the new tabbed browsing system, and adapting
Themis to utilize multiple windows in a single execution of the application, this view
may be reimplemented to be the top view of any given tab.
*/
class winview:public BView {
	public:
		winview(BRect frame,const char *name,uint32 resizem,uint32 flags);
		~winview();
		void AttachedToWindow();
		void MessageReceived(BMessage *msg);
		//! The location line in the browser window.
		BTextControl *locline;
		BButton *stopbutton;
		uint32 protocol;
//		HTMLParser *Parser;
		sem_id proto_sem;
		//! The menu bar in the browser window.
		BMenuBar *menubar;
		//! The file menu.
		BMenu *filemenu;
		//! The options menu.
		BMenu *optionsmenu;
	
};

#endif
