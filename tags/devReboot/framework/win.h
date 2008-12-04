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

Original Author & Project Manager: Z3R0 One (z3r0_one@users.sourceforge.net)
Project Start Date: October 18, 2000
*/
#ifndef _winclass
#define _winclass
#include <Menu.h>
#include <Window.h>
//#include "htmlparser.h"
#include "msgsystem.h"
#include "ThemisNavView.h"
#include "ThemisStatusView.h"
#include "ThemisTab.h"
#include "ThemisTabView.h"
#include "ThemisUrlPopUpWindow.h"

/*!
\brief The main browser window.

This is the main browser window.
*/

class ThemisTab;

class Win : public BWindow, public MessageSystem
{
	public:
									Win(
										BRect frame,
										const char *title,
										window_type type,
										uint32 flags,
										uint32 wspace=B_CURRENT_WORKSPACE );
									~Win();
		bool						QuitRequested();
		virtual void				FrameMoved( BPoint origin );
		virtual void				FrameResized( float width, float height );
		void						MessageReceived(BMessage *msg);
		//! Detects when the window is brought to the forefront, for utilization.
		void						WindowActivated(bool active);
		//! Detects when the workspace that the application is located in is changed.
		void 						WorkspacesChanged(uint32 oldws, uint32 newws);
		void						Zoom( BPoint origin, float width, float height );
		
		void						AddNewTab( bool hidden );
		uint32						BroadcastTarget();
		void						CreateTabView();
		void						CreateUrlPopUpWindow();
		
		//! Returns a pointer to a tab with the given _view_ ID. 
		ThemisTab*					FindTabFor(
										int32 site_id,
										int32* tabindex = NULL );		
		void						LoadInterfaceGraphics();
		Win*						NextWindow();
		status_t					ReceiveBroadcast(BMessage *message);
		void						ReInitTabHistory();
		void						SetNextWindow( Win* nextwin );
		void						SetQuitConfirmed( bool state );
		
		void						UrlTypedHandler( bool show_all );
						
		BMenuBar*					menubar;
		BMenu*						optionsmenu;
		BMenu*						filemenu;
		ThemisNavView*				navview;
		ThemisUrlPopUpWindow*		urlpopupwindow;
		ThemisTabView*				tabview;
		ThemisStatusView*			statusview;
		BBitmap*					bitmaps[10];

	private:
		bool						startup;
		uint32						protocol;
  		Win*						fNextWindow;
  		BRect						fOldFrame;
  		bool						fMaximized;
  		bool						fQuitConfirmed;
};

#endif
