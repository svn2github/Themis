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
#include "httpv4main.h"
#include "httpv4.h"
HTTPv4 *HTTP=NULL;
BMessage *HTTPSettings=NULL;
status_t Initialize(void *info)
 {
	HTTPSettings=new BMessage();
  if (info!=NULL) {
	  
	BMessage *imsg=(BMessage *)info;
	HTTP=(new HTTPv4(imsg,"HTTP Protocol MS"));
  }else 
  	HTTP=(new HTTPv4(NULL,"HTTP Protocol MS"));
  return B_OK;
 }
status_t Shutdown(bool now)
 {
	  if (HTTP!=NULL)
	   {
	    HTTP->Stop();
  		delete HTTP;
  	   }
	delete HTTPSettings;
  return B_OK;
 }
protocol_plugin* GetObject(void)
 {
  if (HTTP==NULL)
   (new HTTPv4(NULL,"HTTP Protocol"));
  return HTTP;
 }
