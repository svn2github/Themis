/*
Copyright (c) 2004 Raymond "Z3R0 One" Rodgers. All Rights Reserved. 

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

Original Author & Project Manager: Raymond "Z3R0 One" Rodgers (z3r0_one@users.sourceforge.net)
Project Start Date: October 18, 2000
*/

#include "optionshandler.h"
#include "httpv4.h"

#include <Message.h>
#include <stdio.h>
extern HTTPv4 *HTTP;

http_opt_handler::http_opt_handler()
	:BHandler("http_options_handler"){
	parentmenu=NULL;
	browserident=new BMenu("Browser Identification");
	BMenuItem *mi;
	browserident->AddItem((mi=(new BMenuItem("Themis",(new BMessage((uint32)BROWSER_IDENT_THEMIS)),'t', B_SHIFT_KEY))));
	browserident->AddItem((new BMenuItem("Netscape",(new BMessage((uint32)BROWSER_IDENT_NETSCAPE)),'n', B_SHIFT_KEY)));
	browserident->AddItem((new BMenuItem("Internet Explorer",(new BMessage((uint32)BROWSER_IDENT_IE)),'e', B_SHIFT_KEY)));
	browserident->SetRadioMode(true);
	mi->SetMarked(true);
		
}

http_opt_handler::~http_opt_handler() {
	delete browserident;
}

void http_opt_handler::MessageReceived(BMessage *msg) {
	msg->PrintToStream();
	char *bstr[3]={{"Themis"},{"IE"},{"Netscape"}};
	switch(msg->what) {
		case BROWSER_IDENT_THEMIS: {
			HTTP->use_useragent=BROWSER_IDENT_THEMIS;
		}break;
		case BROWSER_IDENT_NETSCAPE: {
			HTTP->use_useragent=BROWSER_IDENT_NETSCAPE;
		}break;
		case BROWSER_IDENT_IE:{
			HTTP->use_useragent=BROWSER_IDENT_IE;
		}break;
		default:
			BHandler::MessageReceived(msg);
	}
//	printf("Browser string is set to %s\n",bstr[meHTTP->use_useragent]);
}
void http_opt_handler::AddMenu(BMenu *parent) {
//	printf("hoh::AddMenu\n");
	if (parentmenu!=NULL) {
		if (parentmenu!=parent) {
			RemoveMenu();
		}
	}
	parentmenu=parent;
	parentmenu->AddItem(browserident);
//	printf("hoh: add menu complete\n");
	parentmenu->InvalidateLayout();
}

void http_opt_handler::RemoveMenu() {
	parentmenu->RemoveItem(browserident);
	parentmenu=NULL;
	
}
