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
#include "aboutwin.h"
#include "app.h"
aboutwin::aboutwin(BRect frame, const char *title, window_type type, uint32 flags, uint32 wspaces)
	:BWindow(frame,title,type,flags,wspaces) 
{
	view=new aboutview(Bounds(),"aboutview",B_FOLLOW_ALL,B_WILL_DRAW|B_NAVIGABLE_JUMP);
	AddChild(view);
	SetSizeLimits(500,1000,200,400);
	Show();
}

aboutwin::~aboutwin() 
{
}

bool aboutwin::QuitRequested() 
{
	be_app_messenger.SendMessage( ABOUTWIN_CLOSE );
	return true;
}

void aboutwin::MessageReceived(BMessage *msg)
{
	switch(msg->what) {
		default:
			BWindow::MessageReceived(msg);
	}
	
}
