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

#ifndef MSGVIEWER_HPP
#define MSGVIEWER_HPP

// BeOS headers
#include <Handler.h>

// Themis headers
#include "plugclass.h"

// Declarations of MsgViewer classes
class MsgView;

// Exports
extern "C" __declspec( dllexport ) status_t Initialize( void * aInfo = NULL );
extern "C" __declspec( dllexport ) status_t Shutdown( bool aNow = false );
extern "C" __declspec( dllexport ) PlugClass * GetObject();

/// The message viewer plugin.
/**
	The message viewer addon receives debug messages
	and displays them in a window.
*/

class MsgViewer	:	public BHandler, public PlugClass	{
	
	private:
		/// The window to display the messages in.
		MsgView * mView;
	
	public:
		/// Constructor of the MsgViewer class.
		/**
			The constructor of the MsgViewer class. It creates the window
			and passes on the plugin list if available.
			
			@param	info	The settings for the plugin.
		*/
		MsgViewer( BMessage * aInfo = NULL );
		/// Destructor of the MsgViewer class.
		/**
			The destructor of the MsgViewer class.
			It deletes the message window.
		*/
		~MsgViewer();
		/// The message handler.
		/**
			The message handler.  All messages are passed on to the
			BHandler's function.
		*/
		void MessageReceived( BMessage * aMessage );
		/// A function to see if this class is a BHandler.
		bool IsHandler();
		/// A function to return the handler.
		BHandler * Handler();
		/// A function to see if this plugin is persistent.
		bool IsPersistent();
		/// A function to return the plug-id of this plugin.
		uint32 PlugID();
		/// A function to return the name of this plugin.
		char * PlugName();
		/// A function to return the version of this plugin.
		float PlugVersion();
		/// A function that is called on a regular interval.
		void Heartbeat();
		/// A function to receive a broadcast.
		/**
			This function receives a broadcast. It responds to
			COMMAND_INFO and DEBUG_INFO messages.
			
			@param	aMessage	The message to receive.
		*/
		status_t ReceiveBroadcast( BMessage * aMessage );
		status_t BroadcastReply( BMessage * message );
		/// A function to define the broadcast target that this plugin listens to.
		uint32 BroadcastTarget();
		/// A function to define the type of the plugin.
		int32 Type();

};

#endif
