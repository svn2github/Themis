/*	File protocol addon
	Processes file requests when an url is specified
	with the file:// prefix
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	23-08-2003
	
*/

#ifndef FILEPROTOCOL_HPP
#define FILEPROTOCOL_HPP

// BeOS headers
#include <Handler.h>

// Themis headers
#include "plugclass.h"

// Declarations of Themis classes
class CachePlug;

extern "C" __declspec( dllexport ) status_t Initialize( void * info = NULL );
extern "C" __declspec( dllexport ) status_t Shutdown( bool now = false );
extern "C" __declspec( dllexport ) PlugClass * GetObject();

class FileProtocol	:	public BHandler, public PlugClass	{

	private:
		CachePlug * cache;
		uint32 userToken;
	
	public:
		FileProtocol( BMessage * info = NULL );
		~FileProtocol();
		void MessageReceived( BMessage * message );
		bool IsHandler();
		BHandler * Handler();
		bool IsPersistent();
		uint32 PlugID();
		char * PlugName();
		float PlugVersion();
		void Heartbeat();
		status_t ReceiveBroadcast( BMessage * message );
		status_t BroadcastReply( BMessage * message );
		uint32 BroadcastTarget();
		int32 Type();

};

#endif

