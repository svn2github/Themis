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
	
	fLocker = new BLocker();
}

UrlHandler::~UrlHandler()
{
	printf( "UrlHandler::~UrlHandler()\n" );
	
	if( fCacheUserToken != 0 )
		fCachePlug->Unregister( fCacheUserToken );
	
	MsgSysUnregister( this );
	
	// delete the whole entry list
	fEntryList.clear();
	
	delete fLocker;
}

void
UrlHandler::BroadcastFinished()
{
//	printf( "UrlHandler::BroadcastFinished()\n" );
}

status_t
UrlHandler::BroadcastReply(
	BMessage* msg )
{
//	printf( "UrlHandler::BroadcastReply()\n" );
	
	return B_OK;
}


uint32
UrlHandler::BroadcastTarget()
{
//	printf( "UrlHandler::BroadCastTarget()\n" );
	
	return MS_TARGET_URLHANDLER;
}

UrlEntry*
UrlHandler::GetEntry(
	int32 id )
{
	fLocker->Lock();
	
	printf( "UrlHandler::GetEntry( %ld )\n", id );
	
	UrlEntry* entry = NULL;
	
	// browse through the entry list to find the url entry with the matching id
	
	vector< UrlEntry >::iterator it;
	for( it = fEntryList.begin(); it != fEntryList.end(); it++ )
	{
		if( it->GetID() == id )
		{
			entry = it;
		}
	}

	fLocker->Unlock();
	return entry;
}

int8
UrlHandler::GetLoadingProgressFor(
	int32 id )
{
	fLocker->Lock();

	printf( "UrlHandler::GetLoadingProgressFor( %ld )\n", id );
	
	UrlEntry* entry = GetEntry( id );

	if( entry == NULL )
	{
		fLocker->Unlock();
		return -1;
	}
	
	fLocker->Unlock();	
	return entry->GetLoadingProgress();
}

const char*
UrlHandler::GetTitleFor(
	int32 id )
{
	fLocker->Lock();

	printf( "UrlHandler::GetTitleFor( %ld )\n", id );
	
	BString title;
	
	UrlEntry* entry = GetEntry( id );
	
	if( entry == NULL )
	{
		fLocker->Unlock();
		return NULL;
	}

	fLocker->Unlock();	
	return entry->GetTitle();
}

status_t
UrlHandler::ReceiveBroadcast(
	BMessage* msg )
{
//	printf( "UrlHandler::ReceiveBroadCast()\n" );

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
					printf( "URLHANDLER: PlugInLoaded\n" );
					
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
				case UH_LOADING_FINISHED :
				case ProtocolConnectionClosed :
				{
					printf( "URLHANDLER: ProtocolConnectionClosed/UH_LOADING_FINISHED\n" );
					
					/*
					 * Set the UrlEntrys state to loading complete. (To be implemented.)
					 * Tell the HTML parser to start parsing the loaded data.
					 * Broadcast message is the following:
					 * 		- uin32		"what" = UH_PARSE_HTML_START
					 *		- int32		"command" = COMMAND_INFO
					 * 		- bool		"request_done" 	// Probably removable, as I can check here.
					 * 		- string	"mimetype"
					 * 		- string	"url"
					 * 		- int32		"view_id"
					 */
					
					break;
				}
				case UH_LOADING_PROGRESS :
				case ReturnedData :	// we need to find better specified constants, or take the above one :)
				{
					printf( "URLHANDLER: ReturnedData/UH_LOADING_PROGRESS\n" );
					
					msg->PrintToStream();
					
					int16 id = 0;
					int64 contentlength = 0;
					int64 bytes_received = 0;
					int64 size_delta = 0;
					bool request_done = false;
					bool secure = false;	
					
					// We need to change everything to int32 later!
					
					// Due to every kind of data delivered with 'ReturnedData' we make a little sanity
					// check here to avoid problems.
					if( msg->HasInt16( "view_uid" ) == false )
						break;
					
					msg->FindInt16( "view_uid", &id );
					msg->FindInt64( "bytes-received", &bytes_received);
					msg->FindInt64( "size-delta", &size_delta);
					msg->FindBool( "request_done", &request_done );
					msg->FindBool( "secure", &secure );					
					
					if( msg->HasInt64( "content-length" ) )
						msg->FindInt64( "content-length", &contentlength );
					
					// Put all the info in the UrlEntry.
					UrlEntry* entry = GetEntry( id );
					
					if( entry != NULL )
					{
						// When we do have some info about cookies, set it here.
						//SetCookiesDisabled( cookies_disabled );
						
						entry->SetSecureConnection( secure );
						
						if( request_done == false )
						{
							int8 loadprogress = -1;
						
							if( contentlength != 0 )
							{
								loadprogress = ( int8 )( ( bytes_received / contentlength ) * 100 );
							}
							else	// chunked transfer mode
							{
								loadprogress = 50;
							}
							entry->SetLoadingProgress( loadprogress );
						}
						else
							entry->SetLoadingProgress( 100 );

						entry->Print();
						
						// Broadcast the loading progress notify to the windows.
						BMessage* notify = new BMessage( UH_WIN_LOADING_PROGRESS );
						notify->AddInt32( "command", COMMAND_INFO );
						notify->AddInt32( "view_id", ( int32 )id );
						Broadcast( MS_TARGET_WINDOW, notify );
						delete notify;
					}
					else
					{
						printf( "URLHANDLER: UrlEntry not valid anymore!!!!!!!!\n" );
						
						/*
						 * Tell http/network layer to stop loading the page.
						 * Raymond, perhaps you can give me some hint on that. Thanks.
						 */
					}
					
					break;
				}
				case UH_LOAD_NEW_PAGE :
				{
					printf( "URLHANDLER: UH_LOAD_NEW_PAGE\n" );
					
					// create a new UrlEntry item and store it in the entry list
					
					int32 id;
					BString url;
					
					msg->FindInt32( "view_id", &id );
					msg->FindString( "target_url", &url );
					
					printf( "URLHANDLER: adding following item: ID[%ld] URL[%s]\n", id, url.String() ); 
					
					UrlEntry* entry = new UrlEntry( id, url.String() );
					
					fEntryList.push_back( *entry );
					
					/*
					 * I was planning to move all urlparsing and handler decision making from the windows
					 * URL_OPEN/BUTTON_RELOAD switch in here. Is this ok? But for that, I could use some
					 * little assistance, as I'd like to break some currently working things then, too. :))
					 */
					
					break;
				}
				case UH_PARSE_HTML_FINISHED :
				{
					printf( "URLHANDLER: UH_PARSE_HTML_FINISHED\n" );
					
					/*
					 * The HTML parser delivers us with the pointer to the DOM tree data AND with the
					 * view-ID of the parsed site, when it is finished with parsing the HTML data.
					 * Then I will fetch the RenderView pointer from the tab which is holding the
					 * RenderView with the appropriate view ID.
					 * When everything is fine, I will Broadcast a message to the renderer with
					 * 'UH_RENDER_START' including the DOM tree and RenderView pointer.
					 * Additionally the UrlEntrys state is set to html_parsed. (To be implemented.)
					 * Note: The RenderView object is initialized with its unique ID in the windows
					 * MessageReceived() URL_OPEN/BUTTON_RELOAD case and then attached to the appropriate
					 * tab.
					 *
					 * Broadcast message to be sent is the following:
					 * 		- uint32	"what" = UH_RENDER_START
					 *		- int32		"command" = COMMAND_INFO
					 * 		- pointer	"renderview_pointer"
					 * 		- pointer	"dom_tree_pointer"
					 */
						
					break;
				}
				case UH_RELOAD_PAGE :
				{
					printf( "URLHANDLER: UH_RELOAD_PAGE\n" );
					
					// TODO
					
					break;
				}
				case UH_RENDER_FINISHED :
				{
					printf( "URLHANDLER: UH_RENDER_FINISHED\n" );
					
					/*
					 * The renderer just informs us about that it has finished rendering the page.
					 * So we set the UrlEntrys state to rendered. (To be implemented.)
					 * Note: While the renderer is cycling through the DOM tree and drawing into
					 * the RenderView, it needs to make sure that it locks the window the RenderView
					 * is attached to. (BView::LockLooper() could come in handy there.)
					 *
					 * The Broadcast I expect here is the following:
					 * 		- uint32	"what" = UH_RENDER_FINISHED
					 * 		- int32		"command" = COMMAND_INFO
					 * 		- int32		"view_id"
					 */
					
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
