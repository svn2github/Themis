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

HTMLParser * parser;
BMessage ** appSettings_p;
BMessage * appSettings;

status_t Initialize( void * aInfo )	{
	
	parser = NULL;
	if ( aInfo != NULL )	{
		BMessage * message = (BMessage *) aInfo;
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

status_t Shutdown( bool aNow )	{
	
	delete parser;
	
	return B_OK;
	
}

PlugClass * GetObject()	{
	
	return parser;
	
}

HTMLParser	::	HTMLParser( BMessage * aInfo )
					:	BHandler( "HTMLParser" ),
						PlugClass( aInfo , "HTMLParser" )	{
	
	mCache = (CachePlug *) PlugMan->FindPlugin( CachePlugin );
	mUserToken = 0;
	
	if ( mCache != NULL )	{
		mUserToken = mCache->Register( Type(), "HTML Parser" );
	}
	
	if ( appSettings != NULL )	{
		const char * path;
		appSettings->FindString( "DTDToUsePath", &path );
		mParser = new SGMLParser( path );
	}
	else	{
		mParser = NULL;
	}
	
}

HTMLParser	::	~HTMLParser()	{

	delete mParser;
	
}

void HTMLParser	::	MessageReceived( BMessage * aMessage )	{
	
	BHandler::MessageReceived( aMessage );
	
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
	
	return 0.6;
	
}

void HTMLParser	::	Heartbeat()	{
	
}

status_t HTMLParser	::	ReceiveBroadcast( BMessage * aMessage )	{
	
	int32 command = 0;
	aMessage->FindInt32( "command", &command );
	
	switch ( command )	{
		case COMMAND_INFO_REQUEST:	{
			switch( aMessage->what )	{
				case GetSupportedMIMEType:	{
					if ( aMessage->HasBool( "supportedmimetypes" ) )	{
						BMessage types( SupportedMIMEType );
						types.AddString( "mimetype", "text/html" );
						types.AddInt32( "command", COMMAND_INFO );
						PlugClass * plug = NULL;
						if ( aMessage->HasPointer( "ReplyToPointer" ) )	{
							aMessage->FindPointer( "ReplyToPointer", (void**) &plug );
							if ( plug != NULL )	{
								plug->BroadcastReply( &types );
							}
						}
					}
					break;
				}
			}
		}	
		case COMMAND_INFO:	{
			switch ( aMessage->what )	{
				case PlugInLoaded:	{
					PlugClass * plug = NULL;
					aMessage->FindPointer(  "plugin", (void **) &plug );
					if ( plug != NULL )	{
							if (  plug->PlugID() == 'cash' )	{
								mCache = (CachePlug *) plug;
								mUserToken = mCache->Register( Type(), "HTML Parser" );
							}
					}
					break;
				}
				case PlugInUnLoaded:	{
					uint32 type = 0;
					type = aMessage->FindInt32( "type" );
					if ( ( type & TARGET_CACHE ) != 0 )	{
						mCache = NULL;
					}
					break;
				}
				case DTD_CHANGED_PARSER:	{
					Debug( "Request to change parser", PlugID() );
					if ( appSettings != NULL )	{
						const char * path;
						appSettings->FindString( "DTDToUsePath", &path );
						string dtdLoad = "Loading new DTD: ";
						dtdLoad += path;
						Debug( dtdLoad.c_str(), PlugID() );
						if ( mParser == NULL )	{
							mParser = new SGMLParser( path );
							mParser->parseDTD();
						}
						else	{
							mParser->parseDTD( path );
						}
						Debug( "New DTD loaded", PlugID() );
					}
					break;
				}
				case ProtocolConnectionClosed:	{
					bool requestDone = false;
					aMessage->FindBool( "request_done", &requestDone );
					
					if ( !requestDone )	{
						// I'll wait
						break;
					}
					
					/* temporarily added by emwe */
					int32 view_id = 0;
					aMessage->FindInt32( "view_id", &view_id );

					const char * mimetype = NULL;
					aMessage->FindString( "mimetype", &mimetype );
					
					if ( mimetype != NULL ) {
						if ( strncasecmp( mimetype, "image", 5 ) == 0 ) {
							break;
						}
					}

					const char * url = NULL;
					aMessage->FindString( "url", &url );
					if ( url == NULL )	{
						// What the heck
						break;
					}
			
					int32 fileToken = mCache->FindObject( mUserToken, url );
					ssize_t fileSize = mCache->GetObjectSize( mUserToken, fileToken );
					
					if ( fileSize == 0 )	{
						Debug( "Requested file is 0 bytes long. Something is wrong here",
								   PlugID() );
						break;
					}
					string content;
					char * buffer = new char[ 2000 ];
					ssize_t bytesRead = 0;
					int totalBytes = 0;
					bytesRead = mCache->Read( mUserToken, fileToken, buffer, 2000 );
					while (  bytesRead > 0 )	{
						totalBytes += bytesRead;
						if ( totalBytes > fileSize )	{
							break;
						}
						content += buffer;
						memset( buffer, 0, 1000 );
						bytesRead = mCache->Read( mUserToken, fileToken, buffer, 2000 );
					}
					delete[] buffer;

					// Parse it
					SGMLTextPtr	docText = SGMLTextPtr( new SGMLText( content ) );
					if ( mParser != NULL )	{
						mDocument = mParser->parse( docText );
						// A bit messy. Variable url is still valid. Check back to start of case.
						string urlString( url );
						mDocument->setDocumentURI( urlString );
						
						BMessage * done = new BMessage( UH_PARSE_DOC_FINISHED );
						done->AddInt32( "command", COMMAND_INFO );
						done->AddString( "type", "dom" );
						done->AddPointer( "dom_tree_pointer", &mDocument );
						/* added by emwe */
						done->AddInt32( "view_id", view_id );
						/**/
						Broadcast( MS_TARGET_ALL, done );
						
						Debug( "File parsed", PlugID() );

						delete done;
						done = NULL;
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

status_t HTMLParser		::	BroadcastReply( BMessage * aMessage )	{

	return B_OK;

}

uint32 HTMLParser	::	BroadcastTarget()	{

	return MS_TARGET_HTML_PARSER;
	
}

int32 HTMLParser	::	Type()	{
	
	return TARGET_PARSER;
	
}
