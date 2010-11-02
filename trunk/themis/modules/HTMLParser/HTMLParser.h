/*	HTMLParser addon
	Basically only testing at the moment
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	03-09-2002
	
*/

#ifndef HTMLPARSER_H
#define HTMLPARSER_H

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

// SGMLParser headers
#include "SGMLParser.hpp"

// Namespaces used
using namespace std;

extern "C" __declspec(dllexport) status_t Initialize(void * aInfo = NULL);
extern "C" __declspec(dllexport) status_t Shutdown(bool aNow = false);
extern "C" __declspec(dllexport) PlugClass * GetObject();

class HTMLParser : public BHandler, public PlugClass {

	private:
		// Plugin variables
		CachePlug * mCache;
		uint32 mUserToken;

		// Storage for the parsed document trees.
		vector<TDocumentPtr> mDocuments;

		SGMLParser * mParser;
		// Storing the path of the active DTD.
		string mActiveDTDPath;
		// List of mimetypes supported.
		vector<string> mMimeTypes;

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
		// Function to get the DTD path from the settings.
		string GetDTDPathFromSettings() const;
		// Function to check if the document is a supported one.
		bool IsDocumentSupported(BMessage * aMessage);
		// Function to notify plugins that parsing is finished.
		void NotifyParseFinished(void * aDocument,
								 string aType,
								 BMessage * aOriginalMessage);
		// Function to parse a document.
		void ParseDocument(string aURL,
						   BMessage * aOriginalMessage);

	public:
		HTMLParser(BMessage * aInfo = NULL);
		~HTMLParser();
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
