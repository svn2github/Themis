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
	Class Start Date: September 21, 2002
*/

#ifndef DOMVIEWER_HPP
#define DOMVIEWER_HPP

// Themis headers
#include "plugclass.h"

// Declarations of DOMViewer classes
class DOMView;

// Exports
extern "C" __declspec( dllexport ) status_t Initialize( void * aInfo = NULL );
extern "C" __declspec( dllexport ) status_t Shutdown( bool aNow = false );
extern "C" __declspec( dllexport ) PlugClass * GetObject();

class DOMViewer	:	public BHandler, public PlugClass	{
	
	private:
		DOMView * mView;
	
	public:
		DOMViewer( BMessage * aInfo = NULL );
		~DOMViewer();
		void MessageReceived( BMessage * aMessage );
		bool IsHandler();
		BHandler * Handler();
		bool IsPersistent();
		uint32 PlugID();
		char * PlugName();
		float PlugVersion();
		void Heartbeat();
		status_t ReceiveBroadcast( BMessage * aMessage );
		status_t BroadcastReply( BMessage * aMessage );
		uint32 BroadcastTarget();
		int32 Type();
		
};

#endif
