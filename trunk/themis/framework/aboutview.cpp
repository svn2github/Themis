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
#include "aboutview.h"
#include <Messenger.h>
#include <Window.h>

aboutview::aboutview(BRect frame, const char *name, uint32 resizem, uint32 flags)
	:BView(frame,name,resizem,flags) 
{
	SetViewColor(216,216,216);
	BRect r(Bounds());
	r.InsetBy(10,10);
	r.bottom-=20;
	outerbox=new BBox(r,"outer-about-box",B_FOLLOW_ALL,B_WILL_DRAW|B_FRAME_EVENTS|B_NAVIGABLE_JUMP,B_FANCY_BORDER);
	AddChild(outerbox);
	r=Bounds();
	r.bottom-=10.0;
	r.top=r.bottom-15.0;
	r.left=(r.right/2.0)-20.0;
	r.right=(r.right/2.0)+20.0;
	OkB=new BButton(r,"Ok-button","Done",(new BMessage(B_OK)),B_WILL_DRAW|B_NAVIGABLE);
	AddChild(OkB);
	items=NULL;
	
}

aboutview::~aboutview() 
{
	if (items!=NULL) {
		delete items;
		
	}
	
}
void aboutview::MessageReceived(BMessage *msg) 
{
	switch(msg->what) {
		case B_OK: {
			BMessenger *msgr=new BMessenger(NULL,(BLooper*)Window(),NULL);
			msgr->SendMessage(B_QUIT_REQUESTED);
			delete msgr;
		}break;
		default:
			BView::MessageReceived(msg);
	}
	
}

void aboutview::AttachedToWindow() 
{
	OkB->SetTarget(this);
}
