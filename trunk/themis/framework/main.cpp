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
/*!
\file
\brief The start of the application.

This is the where Themis begins execution.
*/

/*!
\mainpage The Themis Project

\section intro Introduction
*
* What is Themis? The Themis project is an effort by a handfull of developers to bring a modern
* web browser to the BeOS platform.
*/

#define DEBUG 1

// BeOS headers
#include <SupportKit.h>

// Standard C headers
#include <stdio.h>

// Themis headers
#include "app.h"
#include "protocol_plugin.h"
#include "appdefines.h"

int main() {
	SET_DEBUG_ENABLED(TRUE);
	(new App(AppSig));
	be_app->Run();
	delete be_app;
	fflush(stdout);
	/*
	char output[401];
	const char *input="http://235396898359/obscure.htm";
	ProtocolPlugClass::UnObfuscateURL(input,output,400);
	printf("input: %s\noutput: %s\n",input,output);
	input="http://3468664375/obscure.htm";
	ProtocolPlugClass::UnObfuscateURL(input,output,400);
	printf("input: %s\noutput: %s\n",input,output);
	input="http://7763631671/obscure.htm";
	ProtocolPlugClass::UnObfuscateURL(input,output,400);
	printf("input: %s\noutput: %s\n",input,output);
	input="http://3468664375@3468664375/o%62s%63ur%65%2e%68t%6D";
	ProtocolPlugClass::UnObfuscateURL(input,output,400);
	printf("input: %s\noutput: %s\n",input,output);
	*/
	return 0;
}
