/*	MsgViewer addon
	Displays messages sent to it by other parts of Themis
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	23-08-2003
	
*/

#ifndef MSGVIEWER_HPP
#define MSGVIEWER_HPP

// BeOS headers
#include <Handler.h>

// Themis headers
#include "plugclass.h"

// Declarations of MsgViewer classes
class MsgView;

extern "C" __declspec( dllexport ) status_t Initialize( void * info = NULL );
extern "C" __declspec( dllexport ) status_t Shutdown( bool now = false );
extern "C" __declspec( dllexport ) PlugClass * GetObject();

class MsgViewer	:	public BHandler, public PlugClass	{
	
	private:
		MsgView * view;
	
	public:
		MsgViewer( BMessage * info = NULL );
		~MsgViewer();
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
