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
#include "authvw.h"
#include <stdio.h>
#include <string.h>
#include <String.h>
extern httplayer *meHTTP;

authview::authview(BRect frame)
	:BView(frame,"authview",B_FOLLOW_ALL,B_WILL_DRAW|B_NAVIGABLE_JUMP){
		SetViewColor(216,216,216);
		BRect r(Bounds());
		r.top=10;
		r.bottom=r.top+35;
		r.left=10;
		r.right-=10;
		info=new BTextView(r,"infostring",r,B_FOLLOW_LEFT|B_FOLLOW_TOP,B_WILL_DRAW);
		info->MakeEditable(false);
		info->SetWordWrap(true);
		r.top=r.bottom+15;
		r.bottom=r.top+15;
		user=new BTextControl(r,"authuser","User name:","",NULL,B_FOLLOW_LEFT|B_FOLLOW_TOP,B_WILL_DRAW|B_NAVIGABLE);
		r.top=r.bottom+15;
		r.bottom=r.top+15;
		user->SetDivider(85);
		
		pass=new BTextControl(r,"authpass","Password:","",NULL,B_FOLLOW_LEFT|B_FOLLOW_TOP,B_WILL_DRAW|B_NAVIGABLE);
		(pass->TextView())->HideTyping(true);
		pass->SetDivider(80);
		
		r=Bounds();
		r.left=r.right-(0.25*r.right);
		r.right-=10;
		r.bottom-=10;
		r.top=r.bottom-20;
		ok=new BButton(r,"okbutton","Ok",new BMessage(B_OK));
		float width=r.right-r.left;
		r.right=r.left-10;
		r.left=r.right-width;
		cancel=new BButton(r,"cancelbutton","Cancel",new BMessage(B_CANCEL));
		AddChild(info);
		AddChild(user);
		AddChild(pass);
		AddChild(cancel);
		AddChild(ok);
}
authview::~authview() {
}
void authview::AttachedToWindow() {
	cancel->SetTarget(Window());
	ok->SetTarget(Window());
	user->MakeFocus(true);
	
}

authwin::authwin(const char *title,http_request *req,char *rlm)
	:BWindow(BRect(200,200,500,400),title,B_FLOATING_WINDOW,B_NOT_CLOSABLE|B_NOT_ZOOMABLE|B_NO_WORKSPACE_ACTIVATION|B_ASYNCHRONOUS_CONTROLS,B_CURRENT_WORKSPACE) {
	view=new authview(Bounds());
	realm=rlm;
	request=req;
	AddChild(view);
	SetDefaultButton(view->ok);
	BString info;
	info << "You have been requested to provide a user name\n and password for \""<<realm<<"\".";
	view->info->SetText(info.String());
	Show();
		
}

authwin::~authwin() {
}

void authwin::MessageReceived(BMessage *msg) {
	switch(msg->what) {
		case B_OK: {
			meHTTP->AddAuthRealm(request,(char*)realm.String(),(char*)view->user->Text(),(char*)view->pass->Text());
			Quit();
//			PostMessage(B_QUIT_REQUESTED);
		}break;
		case B_CANCEL: {
			Quit();
//			PostMessage(B_QUIT_REQUESTED);
		}break;
		default:
			BWindow::MessageReceived(msg);
	}
	
}
bool authwin::QuitRequested(){
	return true;
}
