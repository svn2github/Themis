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
	
	Original Author: 	Mark Hellegers (mark@firedisk.net)
	Project Start Date: October 18, 2000
	Class Start Date: August 23, 2003
*/
/*
	MsgViewer implementation
	See MsgViewer.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// Standard C++ headers
#include <string>

// BeOS headers
#include <Message.h>
#include <Messenger.h>

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

status_t Initialize( void * aInfo )	{
	
	viewer = NULL;
	if ( aInfo != NULL )	{
		BMessage * message = (BMessage *) aInfo;
		if ( message->HasPointer( "settings_message_ptr" ) )	{
			message->FindPointer( "settings_message_ptr", (void **) & appSettings_p );
			appSettings = *appSettings_p;
		}
		viewer = new MsgViewer( message );
	}
	else	{
		viewer = new MsgViewer();
	}
	
	return B_OK;
	
}

status_t Shutdown( bool aNow )	{
	
	delete viewer;
	
	return B_OK;
	
}

PlugClass * GetObject()	{
	
	return viewer;
	
}

MsgViewer	::	MsgViewer( BMessage * aInfo )
					:	BHandler( "MsgViewer" ), PlugClass( aInfo, "MsgViewer" )	{

	mView = new MsgView();
	int32 pluginCount = CountMembers();
	for ( int32 i = 0; i < pluginCount; i++ )	{
		MessageSystem * msgSystem = GetMember( i );
		string name = msgSystem->MsgSysObjectName();
		mView->addPlugin( name );
	}
	mView->Show();

}

MsgViewer	::	~MsgViewer()	{
	
	BMessenger messenger( mView );
	messenger.SendMessage( B_QUIT_REQUESTED );

}

void MsgViewer	::	MessageReceived( BMessage * aMessage )	{
	
	BHandler::MessageReceived( aMessage );
	
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
	
	return 0.4;
	
}

void MsgViewer	::	Heartbeat()	{
	
}

status_t MsgViewer	::	ReceiveBroadcast( BMessage * aMessage )	{

	int32 command = 0;
	aMessage->FindInt32( "command", &command );
	
	switch ( command )	{
		case COMMAND_INFO:	{
			switch ( aMessage->what )	{
				case PlugInLoaded:	{
					PlugClass * plug = NULL;
					aMessage->FindPointer(  "plugin", (void **) &plug );
					if ( plug != NULL )	{
						mView->addPlugin( plug->PlugName() );
					}
					break;
				}
				case DEBUG_INFO_MSG:	{
					const char * type;
					PlugClass * plug = NULL;
					int32 id = 0;
					aMessage->FindInt32( "pluginID", &id );
					string sender;
					if ( PlugMan )	{
						plug = PlugMan->FindPlugin( id );
					}
					if ( plug != NULL )	{
						sender = plug->MsgSysObjectName();
					}
					else	{
						sender = "General messages";
					}
					int32 count = 0;
					type_code code = B_STRING_TYPE;
					aMessage->GetInfo( "message", &code, &count );
					const char * message;
					for ( int32 i = 0; i < count; i++ )	{
						aMessage->FindString( "message", i, &message );
						string messageString = message;
						mView->addMessage( messageString, sender );
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

status_t MsgViewer	::	BroadcastReply( BMessage * aMessage )	{
	
	return B_OK;
	
}

uint32 MsgViewer	::	BroadcastTarget()	{

	return MS_TARGET_DEBUG;
	
}

int32 MsgViewer	::	Type()	{
	
	return TARGET_HANDLER;
	
}
