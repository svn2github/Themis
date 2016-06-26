/*
	Copyright (c) 2009 Mark Hellegers. All Rights Reserved.
	
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
	Class Start Date: November 15, 2009
*/

/*	CSSParserPlugin addon
	Plugin for parsing css files.
	
	Mark Hellegers (mark@firedisk.net)
	15-11-2009
	
*/

#ifndef CSSPARSERPLUGIN_HPP
#define CSSPARSERPLUGIN_HPP

// BeOS headers
#include <SupportDefs.h>
#include <Handler.h>

// Themis headers
#include "plugclass.h"
#include "cacheplug.h"

// Standard C++ headers
#include <string>

// DOM headers
#include "DOMSupport.h"

// CSSParser headers
#include "CSSParser.hpp"

// Namespaces used
using namespace std;

extern "C" __declspec(dllexport) status_t Initialize(void * aInfo = NULL);
extern "C" __declspec(dllexport) status_t Shutdown(bool aNow = false);
extern "C" __declspec(dllexport) PlugClass * GetObject();

class CSSParserPlugin : public BHandler, public PlugClass {

	private:
		// Plugin variables
		CachePlug * mCache;
		uint32 mUserToken;

		// Storage for the parsed stylesheet documents.
		vector<CSSStyleSheetPtr> mDocuments;

		CSSParser * mParser;
		// List of mimetypes supported.
		vector<string> mMimeTypes;
		
		void LoadCSSFile(BMessage * aMessage);

		// Function to find out if a message was sent by the cache.
		bool MessageSentByCache(BMessage * aMessage);
		// Function to register the cache.
		void RegisterCache(BMessage * aMessage);
		// Function to unregister the cache.
		void UnregisterCache(BMessage * aMessage);
		// Function to check if we have registered with the cache.
		bool CacheRegistered() const;
		// Function to reply with the supported mimetypes.
		void ReplySupportedMimeTypes(BMessage * aMessage);
		// Function to check if the document is a supported one.
		bool IsDocumentSupported(BMessage * aMessage);
		// Function to notify plugins that parsing is finished.
		void NotifyParseFinished(CSSStyleSheetPtr aStyleSheet,
								 string aType,
								 BMessage * aOriginalMessage);

	public:
		CSSParserPlugin(BMessage * aInfo = NULL);
		~CSSParserPlugin();
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
		char * SettingsViewLabel();
		BView * SettingsView(BRect aFrame);

};

#endif
