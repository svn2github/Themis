/*	MsgViewer implementation
	See MsgViewer.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// Standard C++ headers
#include <string>

// BeOS headers
#include <Message.h>

// MsgView headers
#include "MsgViewer.hpp"
#include "MsgView.hpp"

// Themis headers
#include "commondefs.h"
#include "plugman.h"

// Namespaces used
using namespace std;

MsgViewer * viewer;
BMessage ** appSettings_p;
BMessage * appSettings;

status_t Initialize( void * info )	{
	
	printf( "Calling Initialize of MsgViewer...\n" );
	
	viewer = NULL;
	if ( info != NULL )	{
		BMessage * message = (BMessage *) info;
		if ( message->HasPointer( "settings_message_ptr" ) )	{
			message->FindPointer( "settings_message_ptr", (void **) & appSettings_p );
			appSettings = *appSettings_p;
		}
		printf( "Constructing object\n" );
		viewer = new MsgViewer( message );
	}
	else	{
		printf( "Constructing object clean\n" );
		viewer = new MsgViewer();
	}
	
	return B_OK;
	
}

status_t Shutdown( bool now )	{
	
	printf( "Shutting down MsgViewer\n" );
	delete viewer;
	
	return B_OK;
	
}

PlugClass * GetObject()	{
	
	return viewer;
	
}

MsgViewer	::	MsgViewer( BMessage * info )
					:	BHandler( "MsgViewer" ), PlugClass( info, "MsgViewer" )	{

	view = new MsgView();
	view->Show();

}

MsgViewer	::	~MsgViewer()	{
	
	view->Quit();
	
}

void MsgViewer	::	MessageReceived( BMessage * message )	{
	
	message->PrintToStream();
	
}

bool MsgViewer	::	IsHandler()	{
	
	return true;
	
}

BHandler * MsgViewer	::	Handler()	{
	
	return this;
	
}

bool MsgViewer	::	IsPersistent()	{
	
	return true;
	
}

uint32 MsgViewer	::	PlugID()	{
	
	return 'msgv';
	
}

char * MsgViewer	::	PlugName()	{
	
	return "Message Viewer";
	
}

float MsgViewer	::	PlugVersion()	{
	
	return 0.1;
	
}

void MsgViewer	::	Heartbeat()	{
	
}

status_t MsgViewer	::	ReceiveBroadcast( BMessage * message )	{

	printf( "MsgViewer is receiving broadcast:\n" );
	
	int32 command = 0;
	message->FindInt32( "command", &command );
	
	switch ( command )	{
		case COMMAND_INFO:	{
			switch ( message->what )	{
				case ReturnedData:	{
					const char * type;
					message->FindString( "type", &type );
					string typeString = type;
					if ( typeString != "messages" )	{
						return PLUG_DOESNT_HANDLE;
					}
					PlugClass * plug = NULL;
					int32 id = 0;
					message->FindInt32( "pluginID", &id );
					string sender;
					if ( PlugMan )	{
						plug = PlugMan->FindPlugin( id );
					}
					if ( plug != NULL )	{
						sender = plug->PlugName();
					}
					int32 count = 0;
					type_code code = B_STRING_TYPE;
					message->GetInfo( "message", &code, &count );
					const char * messageString;
					for ( int32 i = 0; i < count; i++ )	{
						message->FindString( "message", i, &messageString );
						view->addMessage( sender.c_str(), messageString );
					}
					break;
				}
			}
			break;
		}
		default:	{
			return PLUG_DOESNT_HANDLE;
		}
	}

	return PLUG_HANDLE_GOOD;
	
}

status_t MsgViewer	::	BroadcastReply( BMessage * message )	{
	
	return B_OK;
	
}

uint32 MsgViewer	::	BroadcastTarget()	{

	return MS_TARGET_HANDLER;
	
}

int32 MsgViewer	::	Type()	{
	
	return TARGET_HANDLER;
	
}
