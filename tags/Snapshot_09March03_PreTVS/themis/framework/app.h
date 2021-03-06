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
#ifndef _appclass
#define _appclass
#include <AppKit.h>
#include "win.h"
#include "appdefines.h"
#include "plugman.h"
#include "aboutwin.h"

#define THEMIS_FRAMEWORK_APP_VERSION 0.40


class App:public BApplication {
	private:
		Win *win;
		void InitSettings(char *settings_path=NULL);
		volatile int32 qr_called;
	public:
		aboutwin *AWin;
		status_t LoadSettings();
		status_t SaveSettings();
		void AboutRequested();
		App(const char *appsig);
		~App();
		bool QuitRequested();
		void MessageReceived(BMessage *msg);
		void RefsReceived(BMessage *refs);
		void ReadyToRun();
		void ArgvReceived(int32 argc, char **argv);
};

#endif
