/* HTMLParser implementation
	See HTMLParser.h for some more information
*/

// Standard C headers
#include <stdio.h>

// Standard C++ headers
#include <fstream>
#include <string>
#include <algorithm>
#include <ctype.h>

// Be headers
#include <Message.h>
#include <DataIO.h>

// HTMLParser headers
#include "HTMLParser.h"
#include "commondefs.h"
#include "plugman.h"

// DOM headers
#include "TDocument.h"

// SGMLParser headers
#include "SGMLText.hpp"
#include "SGMLParser.hpp"

HTMLParser * parser;
BMessage ** appSettings_p;
BMessage * appSettings;

status_t Initialize( void * info )	{
	
	printf( "Calling Initialize...\n" );
	
	parser = NULL;
	if ( info != NULL )	{
		BMessage * message = (BMessage *) info;
		if ( message->HasPointer( "settings_message_ptr" ) )	{
			message->FindPointer( "settings_message_ptr", (void **) & appSettings_p );
			appSettings = *appSettings_p;
		}
		parser = new HTMLParser( message );
	}
	else	{
		parser = new HTMLParser();
	}
	
	return B_OK;
	
}

status_t Shutdown( bool now )	{
	
	delete parser;
	
	return B_OK;
	
}

PlugClass * GetObject()	{
	
	return parser;
	
}

HTMLParser	::	HTMLParser( BMessage * info )	:	BHandler( "HTMLParser" ), PlugClass( info )	{
	
	cache = (CachePlug *) PlugMan->FindPlugin( CachePlugin );
	userToken = 0;
	
	if ( cache != NULL )	{
		userToken = cache->Register( Type(), "HTML Parser" );
	}
	
}

HTMLParser	::	~HTMLParser()	{
	
}

void HTMLParser	::	MessageReceived( BMessage * message )	{
	
	message->PrintToStream();
	
}

bool HTMLParser	::	IsHandler()	{
	
	return true;
	
}

BHandler * HTMLParser	::	Handler()	{
	
	return this;
	
}

bool HTMLParser	::	IsPersistent()	{
	
	return true;
	
}

uint32 HTMLParser	::	PlugID()	{
	
	return 'html';
	
}

char * HTMLParser	::	PlugName()	{
	
	return "HTML Parser";
	
}

float HTMLParser	::	PlugVersion()	{
	
	return 0.0;
	
}

void HTMLParser	::	Heartbeat()	{
	
}

status_t HTMLParser	::	ReceiveBroadcast( BMessage * message )	{
	
	printf( "HTMLParser is receiving broadcast:\n" );
	
	int32 command = 0;
	message->FindInt32( "command", &command );
	
	switch ( command )	{
		case COMMAND_INFO_REQUEST:	{
			printf( "COMMAND_INFO_REQUEST called\n" );
			int32 replyto = 0;
			message->FindInt32( "ReplyTo", &replyto );
			switch( message->what )	{
				case GetSupportedMIMEType:	{
					if ( message->HasBool( "supportedmimetypes" ) )	{
						BMessage types( SupportedMIMEType );
						types.AddString( "mimetype", "text/html" );
						types.AddInt32( "command", COMMAND_INFO );
						PlugClass * plug = NULL;
						if ( message->HasPointer( "ReplyToPointer" ) )	{
							message->FindPointer( "ReplyToPointer", (void**) &plug );
							if ( plug != NULL )	{
								printf( "Replying to broadcast\n" );
								plug->BroadcastReply( &types );
							}
						}
						else	{	
							BMessage container;
							container.AddMessage( "message", &types );
							//PlugMan->Broadcast( PlugID(), replyto, &container );
						}
					}
					break;
				}
			}
		}	
		case COMMAND_INFO:	{
			switch ( message->what )	{
				case PlugInLoaded:	{
					PlugClass * plug = NULL;
					message->FindPointer(  "plugin", (void **) &plug );
					if ( plug != NULL )	{
							printf( "A plugin has been loaded\n" );
							if ( ( plug->Type() & TARGET_CACHE ) != 0 )	{
								cache = (CachePlug *) plug;
								userToken = cache->Register( Type(), "HTML Parser" );
							}
					}
					break;
				}
				case PlugInUnLoaded:	{
					uint32 type = 0;
					type = message->FindInt32( "type" );
					if ( ( type & TARGET_CACHE ) != 0 )	{
						printf( "Cache unloaded\n" );
						cache = NULL;
					}
					break;
				}
				case ProtocolConnectionClosed:	{
					printf( "Got data\n" );
					
					bool requestDone = false;
					message->FindBool( "request_done", &requestDone );
					
					if ( !requestDone )	{
						// I'll wait
						break;
					}
					
					const char * url = NULL;
					message->FindString( "url", &url );
					
					if ( url != NULL )	{
						printf( "Getting data of url: %s\n", url );
					}
					else	{
						// What the heck
						break;
					}
			
					int32 fileToken = cache->FindObject( userToken, url );
					ssize_t fileSize = cache->GetObjectSize( userToken, fileToken );
					
					printf( "File size: %i\n", (int) fileSize );
					
					string content;
					char * buffer = new char[ 2000 ];
					ssize_t bytesRead = 0;
					int totalBytes = 0;
					bytesRead = cache->Read( userToken, fileToken, buffer, 2000 );
					while (  bytesRead > 0 )	{
						totalBytes += bytesRead;
						if ( totalBytes > fileSize )	{
							printf( "Ahem. Whose bug is this ?\n" );
							printf( "Complete text gotten:\n%s", content.c_str() );
							break;
						}
						printf( "Got part of data: %i\n", totalBytes  );
						content += buffer;
						memset( buffer, 0, 1000 );
						bytesRead = cache->Read( userToken, fileToken, buffer, 2000 );
					}
					delete[] buffer;

					printf( "Put all data in content\n" );
			
					
					// Parse it
					SGMLTextPtr	docText = SGMLTextPtr( new SGMLText( content ) );
					SGMLParser parser( "/boot/home/config/settings/Themis/dtd/HTML.3.2.Final.DTD", docText );
					
					mDocument = parser.parse();
				
					printf( "Data parsed\n" );
			
					//showDocument();

					if ( PlugMan )	{
						BMessage * done = new BMessage( ReturnedData );
						done->AddInt32( "command", COMMAND_INFO );
						done->AddString( "type", "dom" );
						done->AddPointer( "data_pointer", &mDocument );
						
						BMessage container;
						container.AddMessage( "message", done );
						delete done;
						done = NULL;
						
						PlugMan->Broadcast( TARGET_PARSER, ALL_TARGETS, &container );
					}
					break;
				}
			}
			break;
		}
		default:	{
			message->PrintToStream();
			return PLUG_DOESNT_HANDLE;
		}
	}
	
	return PLUG_HANDLE_GOOD;
	
}

status_t HTMLParser		::	BroadcastReply(BMessage *message) {
	return B_OK;
}

uint32 HTMLParser	::	BroadcastTarget() 
{
	return MS_TARGET_HTML_PARSER;
	
}

int32 HTMLParser	::	Type()	{
	
	return TARGET_PARSER;
	
}
