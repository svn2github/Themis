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
#ifndef _appclass
#define _appclass

// BeOS headers
#include <Application.h>

// Themis headers
#include "tcpmanager.h"

// Declarations used
class BMessage;
class BLocker;
class Win;
class PrefsWin;
class SiteHandler;
class aboutwin;
class GlobalHistory;
class MessageDaemon;

/*!
\brief The application object of the Themis application framework.

This is the heart of the entire application.
*/

extern BMessage* AppSettings;

class App : public BApplication {

	private:
		//! The About window.
		aboutwin * fAboutWin;
		//! The first window in a possible sequence of windows.
		Win * fFirstWindow;
		//! The GlobalHistory object.
		GlobalHistory * fGlobalHistory;
		//! The MessageDaemon object.
		MessageDaemon * fMessageDaemon;
		//! The Preferences window.
		PrefsWin * fPrefsWin;
		//! Checks to see if the QuitRequested function has been called.
		int32 fQR_called;
		//! A counter for the generated unique IDs.
		int32 fIDCounter;
		//! The SiteHandler object.
		SiteHandler * fSiteHandler;
		//! A pointer to the new network system.
		_Themis_Networking_::TCPManager * TCPMan;
		//! Checks settings for missing items, and adds defaults if required.
		void CheckSettings(char * settings_path = NULL);
		//! Opens the preferences window
		void OpenPrefsWindow();
		//! Returns a pointer to the first window.
		Win * FirstWindow();
		//! Used to change the pointer to the first win, whenever it is closed.
		void SetFirstWindow(Win * newfirst);

	public:
		//! Application object constructor.
		App(const char * appsig);
		//! Application object destructor.							
		~App();

		//! Handles the About request.
		void AboutRequested();
		//! Handles arguments passed to the application.
		void ArgvReceived(int32 argc,
						  char ** argv);
		//! Returns a pointer to the GlobalHistory object.
		GlobalHistory * GetGlobalHistory();
		//! Returns the count of _browser_ windows we currently have.
		int32 GetMainWindowCount();
		//! Returns a newly generated unique ID.
		int32 GetNewID();
		//! Returns a pointer to the UrlHandler object.
		SiteHandler * GetSiteHandler();
		//! Loads the settings from disk.
		status_t LoadSettings();
		//! General Be-style message handling.
		void MessageReceived(BMessage * msg);
		//! Handles the quit request.
		bool QuitRequested();
		//! Hook function which is triggered when the application is ready.
		void ReadyToRun();
		//! Hook to handle B_REFS_RECEIVED messages.
		void RefsReceived(BMessage * refs);
		//! Saves the settings to disk.
		status_t SaveSettings();

};

#endif
