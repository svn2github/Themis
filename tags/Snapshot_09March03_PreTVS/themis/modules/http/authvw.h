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
#ifndef _authvw
#define _authvw
#include "httplayer.h"
#include <Window.h>
#include <View.h>
#include <TextControl.h>
#include <TextView.h>
#include <Button.h>
#include <String.h>
class authview:public BView {
	private:
	public:
		BButton *ok,*cancel;
		BTextControl *user,*pass;
		BTextView *info;
		authview(BRect frame);
		~authview();
		void AttachedToWindow();
	
};

class authwin:public BWindow {
	private:
		http_request *request;
		authview *view;
		BString realm;
		bool update;
	public:
		authwin(const char *title,http_request *req,char *rlm,bool upd=false);
		~authwin();
		void MessageReceived(BMessage *msg);
		bool QuitRequested();
};
#endif
