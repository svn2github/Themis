/*
 * UrlHandler.cpp
 */

#include <stdio.h>

#include "plugman.h"
#include "UrlHandler.h"
#include "../common/commondefs.h"

extern plugman* PluginManager;

UrlHandler::UrlHandler()
	: MessageSystem()
{
	printf( "UrlHandler::UrlHandler()\n" );

	// register myself with the message system
	MsgSysRegister( this );
	
	fCacheUserToken = 0;
}

UrlHandler::~UrlHandler()
{
	printf( "UrlHandler::~UrlHandler()\n" );
	
	if( fCacheUserToken != 0 )
		fCachePlug->Unregister( fCacheUserToken );
	
	MsgSysUnregister( this );
}

void
UrlHandler::BroadcastFinished()
{
	printf( "UrlHandler::BroadcastFinished()\n" );
}

status_t
UrlHandler::BroadcastReply(
	BMessage* msg )
{
	printf( "UrlHandler::BroadcastReply()\n" );
	
	return B_OK;
}


uint32
UrlHandler::BroadCastTarget()
{
	printf( "UrlHandler::BroadCastTarget()\n" );
	
	return MS_TARGET_URLHANDLER;
}

status_t
UrlHandler::ReceiveBroadCast(
	BMessage* msg )
{
	printf( "UrlHandler::ReceiveBroadCast()\n" );
	
	int32 command = 0;
	msg->FindInt32( "command", &command );
	
	switch( command )
	{
		case COMMAND_INFO :
		{
			switch( msg->what )
			{
				case PlugInLoaded :
				{
					printf( "  PlugInLoaded\n" );
					
					int32 plugid = 0;
					msg->FindInt32( "plugid", &plugid );
					
					if( plugid == CachePlugin )
					{
						// register with the cache system
						fCachePlug = ( CachePlug* )PluginManager->FindPlugin( CachePlugin );
						if( fCachePlug != NULL )
						{
							fCacheUserToken = fCachePlug->Register( MS_TARGET_URLHANDLER );
							printf( "URLHANDLER: Registered with CachePlug.\n" );
						}
						else
						{
							printf( "URLHANDLER: Couldn't register with CachePlug!\n" );
						}
					}
					break;					
				}
			}
			break;
		}
		case COMMAND_RETRIEVE :
		{
			break;
		}
	}
	
	return B_OK;
}
