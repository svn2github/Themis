/*	HTMLParser addon
	Basically only testing at the moment
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	03-09-2002
	
*/

#ifndef HTMLPARSER_H
#define HTMLPARSER_H

#include <SupportDefs.h>
#include <Handler.h>
#include "plugclass.h"
#include "cacheplug.h"

#include <string>

#include "DOMSupport.h"

using namespace std;

extern "C" __declspec( dllexport ) status_t Initialize( void * info = NULL );
extern "C" __declspec( dllexport ) status_t Shutdown( bool now = false );
extern "C" __declspec( dllexport ) PlugClass * GetObject();

class HTMLParser	:	public BHandler, public PlugClass	{
	
	private:
		// Plugin variables
		CachePlug * cache;
		uint32 userToken;

		TDocumentShared mDocument; // Stores the document to be build
	
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
		int32 Type();
		
};

#endif
