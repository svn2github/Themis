/*
Copyright (c) 2001 Z3R0 One. All Rights Reserved.

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

#include "htmlparser.h"
#include "commondefs.h"
#include <stdio.h>
#include "nuresource.h"
extern plugman *PluginManager;

HTMLParser::HTMLParser()
	:BLooper("html parser",B_DISPLAY_PRIORITY){{
	Run();
}
HTMLParser::~HTMLParser(){
}
bool HTMLParser::QuitRequested(){
	printf("HTMLParser quitting\n");
	return true;
}
void HTMLParser::MessageReceived(BMessage *msg){
	switch(msg->what){
		case ProtocolResponse: {
		}break;
		default:{
			BLooper::MessageReceived(msg);
		}
	}
}
