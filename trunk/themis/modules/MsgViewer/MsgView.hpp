/*
	Copyright (c) 2001 Mark Hellegers. All Rights Reserved.
	
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
	
	Original Author: 	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	Project Start Date: October 18, 2000
	Class Start Date: August 23, 2003
*/

#ifndef MSGVIEW_HPP
#define MSGVIEW_HPP

// Standard C++ headers
#include <string>
#include <map>
#include <vector>

// BeOS headers
#include <Window.h>

// Declarations of BeOS classes
class BTextView;
class BPopUpMenu;
class BMessage;

// Namespaces used
using namespace std;

// Constants used
const uint32 CHANGE_MESSAGE_VIEW = 'cmsv';

/// MsgView shows the debug messages from parts of Themis

/**
	The MsgView class shows a window in which the output of
	parts of Themis is shown. Messages can be shown in their
	own view or in a general view depending on how the message
	was sent.
	
*/

class MsgView	:	public BWindow	{

	private:
		/// The text view to show the messages in.
		BTextView * mMessageView;
		/// The popup menu to let the user choose which messages to show.
		BPopUpMenu * mSenderMenu;
		/// The map which stores all the messages.
		map<string, vector<string> >  messageMap;
	
	public:
		/// Constructor of the MsgView class.
		/**
			The constructor of the MsgView class. It sets up the window,
			creates the popup menu and the storage for the messages.
			
			@param	aPluginList	The list of plugins to show messages for.
											If this is null, the addPlugin function can
											be used to add to the list.
		*/
		MsgView( BMessage * aPluginList = NULL );
		
		/// Destructor of the MsgView class.
		/**
			The destructor of the MsgView class. It does nothing.
			Everything is cleaned up automatically.
		*/
		~MsgView();
		/// The message handler.
		/**
			This function handles all the incoming messages.
			It only handles the custom CHANGED_MESSAG_VIEW message.
			Everything else is passed on to BWindow's function.
			
			@param	aMessage	The message to handle.
		*/
		void MessageReceived( BMessage * aMessage );
		/// A function to shut down the window.
		bool QuitRequested();
		/// A function to add a message to the window.
		/**
			This function adds a message to the window. The message can
			come from a particular sender, but if no sender is indicated
			the message will be added to the "General Messages" view.
			
			@param	aMessage	The message to add.
			@param	aSender		The sender it is coming from.
		*/
		void addMessage( string aMessage, string aSender = "" );
		/// A function to add a plugin to the list.
		/**
			This function adds a plugin to the list of plugins that have their own
			message view. All subsequent messages that arrive under this plugin
			name will be added to a separate view.
			
			@param	aPlugin	The name of the plugin to add.
		*/
		void addPlugin( string aPlugin );
		
};

#endif
