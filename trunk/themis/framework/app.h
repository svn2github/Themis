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
#include "GlobalHistory.h"
#include "prefswin.h"
#include "msgsystem.h"
#include "msgdaemon.h"
#include "UrlHandler.h"
#ifdef NEWNET
#include "tcpmanager.h"
#endif
//! The application framework version number. 
#define THEMIS_FRAMEWORK_APP_VERSION 0.40

/*!
\brief The application object of the Themis application framework.

This is the heart of the entire application.
*/

extern BMessage* AppSettings;

class App:public BApplication, public MessageSystem {
	private:
		//! The first Window in a possible sequence of Windows
		Win* fFirstWindow;
		//! Create the default settings.
		void InitSettings(char *settings_path=NULL);
		//! Checks to see if the QuitRequested function has been called.
		volatile int32 qr_called;
		MessageDaemon *MDaemon;
		
		GlobalHistory*				fGlobalHistory;
		
		int16						fUniqueIDCounter;
		
	public:
#ifdef NEWNET
		_Themis_Networking_::TCPManager *TCPMan;
#endif
		//! The About window.
		aboutwin *AWin;
		//! The Preferences window.
		prefswin *PWin;
		//! The UrlHandler object.
		UrlHandler*					fUrlHandler;
		//! Loads the settings from disk.
		status_t LoadSettings();
		//! Saves the settings to disk.
		status_t SaveSettings();
		void AboutRequested();
		App(const char *appsig);
		~App();
		bool QuitRequested();
		void MessageReceived(BMessage *msg);
		void RefsReceived(BMessage *refs);
		void ReadyToRun();
		void ArgvReceived(int32 argc, char **argv);
		void Pulse();
		GlobalHistory*				GetGlobalHistory();
		int32						GetMainWindowCount();
		int16						GetNewUniqueID();
		Win*						FirstWindow();
		void						SetFirstWindow( Win* newfirst );
		uint32 BroadcastTarget();
		status_t ReceiveBroadcast(BMessage *msg);
		status_t BroadcastReply(BMessage *msg);
};

#endif
