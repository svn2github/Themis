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

extern "C" __declspec( dllexport ) status_t Initialize( void * info = NULL );
extern "C" __declspec( dllexport ) status_t Shutdown( bool now = false );
extern "C" __declspec( dllexport ) PlugClass * GetObject();

class HTMLParser	:	public BHandler, public PlugClass	{
	
	private:
		// Plugin variables
		CachePlug * cache;
		uint32 userToken;

		TDocumentPtr mDocument; // Stores the document to be build
		SGMLParser * parser;
	
	public:
		HTMLParser( BMessage * info = NULL );
		~HTMLParser();
		void MessageReceived( BMessage * message );
		bool IsHandler();
		BHandler * Handler();
		bool IsPersistent();
		uint32 PlugID();
		char * PlugName();
		float PlugVersion();
		void Heartbeat();
		status_t ReceiveBroadcast( BMessage * message );
		status_t BroadcastReply(BMessage *message);
		uint32 BroadcastTarget();
	
		int32 Type();
		
};

#endif
