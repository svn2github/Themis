/*
	Copyright (c) 2010 Mark Hellegers. All Rights Reserved.
	
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
	
	Original Author: 	Mark Hellegers (mark@firedisk.net)
	Project Start Date: October 18, 2000
	Class Start Date: Februari 28, 2010
*/

/*	CSSRenderer
	Renders an HTML file through css
	
	Mark Hellegers (mark@firedisk.net)
	28-02-2010
	
*/

#ifndef CSSRENDERER_HPP
#define CSSRENDERER_HPP

// Themis headers
#include "plugclass.h"

extern "C" __declspec(dllexport) status_t Initialize(void * aInfo = NULL);
extern "C" __declspec(dllexport) status_t Shutdown(bool aNow = false);
extern "C" __declspec(dllexport) PlugClass * GetObject();

class CSSRenderer : public BHandler, public PlugClass {

	public:
		CSSRenderer(BMessage * aInfo = NULL);
		~CSSRenderer();
		void MessageReceived(BMessage * aMessage);
		bool IsHandler();
		BHandler * Handler();
		bool IsPersistent();
		uint32 PlugID();
		char * PlugName();
		float PlugVersion();
		void Heartbeat();
		status_t ReceiveBroadcast(BMessage * aMessage);
		status_t BroadcastReply(BMessage * aMessage);
		uint32 BroadcastTarget();
		int32 Type();

};

#endif
