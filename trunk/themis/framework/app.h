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

#include "aboutwin.h"
#include "appdefines.h"
#include "GlobalHistory.h"
#include "msgsystem.h"
#include "msgdaemon.h"
#include "plugman.h"
#include "prefswin.h"
#ifdef NEWNET
#include "tcpmanager.h"
#endif
#include "UrlHandler.h"
#include "win.h"
#include "../modules/Renderer/TRenderView.h"

//! The application framework version number. 
#define THEMIS_FRAMEWORK_APP_VERSION 0.40

/*!
\brief The application object of the Themis application framework.

This is the heart of the entire application.
*/

extern BMessage* AppSettings;

class App : public BApplication, public MessageSystem
{
	public:
		//! Application object constructor.
									App(
										const char* appsig );
		//! Application object destructor.							
									~App();

		//! Handles the About request.
		void						AboutRequested();
		

		//! Handles arguments passed to the application.
		void						ArgvReceived(
										int32 argc,
										char** argv );
		
		//! Messagesystem method to answer a broadcast to its sender.
		status_t					BroadcastReply(
										BMessage* msg );

		//! Messagesystem method which returns an identifier.
		uint32						BroadcastTarget();
	
		//! Returns a pointer to the first window.
		Win*						FirstWindow();

		//! Returns a pointer to the GlobalHistory object.
		GlobalHistory*				GetGlobalHistory();

		//! Returns the count of _browser_ windows we currently have.
		int32						GetMainWindowCount();
		
		//! Returns a newly generated unique ID.
		int32						GetNewID();
		
		//! Returns a pointer to the TRenderView with the given ID.
		TRenderView*				GetRenderViewFor(
										int32 id );
		
		//! Loads the settings from disk.
		status_t					LoadSettings();
		
		//! General Be-style message handling.
		void						MessageReceived(
										BMessage* msg );

		//! Hook function vor B_PULSE messages.
		void						Pulse();
		
		//! Handles the quit request.
		bool						QuitRequested();
		
		//! Hook function which is triggered when the application is ready.
		void						ReadyToRun();

		//! Messagesystem method to handle broadcasts.
		status_t					ReceiveBroadcast(
										BMessage* msg );
		
		//! Hook to handle B_REFS_RECEIVED messages.
		void						RefsReceived(
										BMessage* refs );

		//! Saves the settings to disk.
		status_t					SaveSettings();

		//! Used to change the pointer to the first win, whenever it is closed.
		void						SetFirstWindow(
										Win* newfirst );
		
#ifdef NEWNET
		//! A pointer to the new network system.
		_Themis_Networking_::TCPManager*	TCPMan;
#endif

	private:
		//! Create the default settings.
		void						InitSettings(
										char* settings_path = NULL );

		//! The About window.
		aboutwin*					fAboutWin;

		//! The first window in a possible sequence of windows.
		Win*						fFirstWindow;
		
		//! The GlobalHistory object.
		GlobalHistory*				fGlobalHistory;
		
		//! A BLocker object for locking purposes.
		BLocker*					fLocker;
		
		//! The MessageDaemon object.
		MessageDaemon*				fMessageDaemon;

		//! The Preferences window.
		prefswin*					fPrefsWin;

		//! Checks to see if the QuitRequested function has been called.
		volatile int32				fQR_called;

		//! A counter for the generated unique IDs.
		int32						fIDCounter;

		//! The UrlHandler object.
		UrlHandler*					fUrlHandler;
};

#endif
