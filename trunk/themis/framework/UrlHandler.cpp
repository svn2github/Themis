/*
 * UrlHandler.cpp
 */

#include <stdio.h>

#include "plugman.h"
#include "ThemisIcons.h"
#include "UrlHandler.h"
#include "../common/commondefs.h"
#include "../modules/Renderer/TRenderView.h"

extern plugman* PluginManager;

UrlHandler::UrlHandler()
	: MessageSystem("URL Handler")
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
	while( !fEntryList.empty() )
	{
		UrlEntry* ent = *( fEntryList.begin() );
		fEntryList.erase( fEntryList.begin() );
		delete ent;
	}
	
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

bool
UrlHandler::EntryValid(
	int32 id )
{
	if( GetEntry( id ) )
		return true;
	else
		return false;
}

UrlEntry*
UrlHandler::GetEntry(
	int32 id /*,
	vector< UrlEntry* >::iterator* itp = NULL */ )
{
	fLocker->Lock();
	
//	printf( "UrlHandler::GetEntry( %ld )\n", id );
	
	UrlEntry* entry = NULL;
	
	// browse through the entry list to find the url entry with the matching id
	
	vector< UrlEntry* >::iterator it;
	for( it = fEntryList.begin(); it != fEntryList.end(); it++ )
	{
		if( ( ( UrlEntry* )*it )->GetID() == id )
		{
//			if( itp )
//				itp = it;
			
			entry = *it;
		}
	}

	fLocker->Unlock();
	return entry;
}

BBitmap*
UrlHandler::GetFavIconFor(
	int32 id )
{
	fLocker->Lock();

//	printf( "UrlHandler::GetFavIconFor( %ld )\n", id );
	
	UrlEntry* entry = GetEntry( id );

	if( entry == NULL )
	{
		fLocker->Unlock();
		return NULL;
	}
	
	fLocker->Unlock();	
	return entry->GetFavIcon();	
}

int8
UrlHandler::GetLoadingProgressFor(
	int32 id )
{
	fLocker->Lock();

//	printf( "UrlHandler::GetLoadingProgressFor( %ld )\n", id );
	
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
UrlHandler::GetStatusTextFor(
	int32 id )
{
	fLocker->Lock();

//	printf( "UrlHandler::GetStatusTextFor( %ld )\n", id );
	
	BString title;
	
	UrlEntry* entry = GetEntry( id );
	
	if( entry == NULL )
	{
		fLocker->Unlock();
		return "";
	}

	fLocker->Unlock();	
	return entry->GetStatusText();
}

const char*
UrlHandler::GetTitleFor(
	int32 id )
{
	fLocker->Lock();

//	printf( "UrlHandler::GetTitleFor( %ld )\n", id );
	
	BString title;
	
	UrlEntry* entry = GetEntry( id );
	
	if( entry == NULL )
	{
		printf( "  entry NULL\n" );
		fLocker->Unlock();
		return "";
	}
	
//	printf( "  entry valid\n" );
	fLocker->Unlock();	
	return entry->GetTitle();
}

const char*
UrlHandler::GetUrlFor(
	int32 id )
{
	fLocker->Lock();

//	printf( "UrlHandler::GetUrlFor( %ld )\n", id );
	
	BString title;
	
	UrlEntry* entry = GetEntry( id );
	
	if( entry == NULL )
	{
		fLocker->Unlock();
		return "";
	}

	fLocker->Unlock();	
	return entry->GetUrl();
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
//					printf( "URLHANDLER: PlugInLoaded\n" );
					
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
				case UH_DOC_CLOSED :
				{
					printf( "URLHANDLER: UH_DOC_CLOSED\n" );
					
					int32 view_id = 0;
					msg->FindInt32( "view_id", &view_id );
					
					RemoveEntry( view_id );
					
					break;
				}
				case UH_LOADING_FINISHED :
				case ProtocolConnectionClosed :
				{
					//printf( "URLHANDLER: ProtocolConnectionClosed/UH_LOADING_FINISHED\n" );
					
					/*
					 * Set the UrlEntrys state to loading complete. (To be implemented.)
					 * Tell the document parser to start parsing the loaded data.
					 * Broadcast message is the following:
					 * 		- uin32		"what" = UH_PARSE_DOC_START
					 *		- int32		"command" = COMMAND_INFO
					 * 		- bool		"request_done" 	// Probably removable, as I can check here.
					 * 		- string	"mimetype"
					 * 		- string	"url"
					 * 		- int32		"view_id"
					 */
					 
					 /*
					  * With Marks idea of grouping modules/add-ons, I guess this broadcast should go to
					  * MS_TARGET_PARSER if i am not mistaken, right?
					  */
					
					//break;
				}
				case UH_LOADING_PROGRESS :
				case ReturnedData :	// we need to find better specified constants, or take the above one :)
				{
					printf( "URLHANDLER: ReturnedData/UH_LOADING_PROGRESS\n" );
					
					//msg->PrintToStream();
					
					int32 id = 0;
					int64 contentlength = 0;
					int64 bytes_received = 0;
					int64 size_delta = 0;
					bool request_done = false;
					bool secure = false;	
					
					// Due to every kind of data delivered with 'ReturnedData' we make a little sanity
					// check here to avoid problems.
					if( msg->HasInt32( "view_id" ) == false )
						break;
					
					msg->FindInt32( "view_id", &id );
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
							printf( "URLHANDLER: Document is still loading...\n" );
							
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
						{
							printf( "URLHANDLER: Document loaded completely!\n" );
							
							entry->SetLoadingProgress( 100 );
							
							/*
							 * As long, as we have no FavIcon loading ready, I do set
							 * the non-empty document icon here.
							 */
							BBitmap* bmp = new BBitmap(
								BRect( 0, 0, 15,15 ),
								B_RGB32 );
							memcpy( bmp->Bits(), icon_document_hex, 1024 );
							entry->SetFavIcon( bmp );
							delete bmp;
							
							/*
							 * Now set up the UH_PARSE_DOC_START message.
							 */
							printf( "URLHANDLER: sending UH_PARSE_DOC_START\n" );
							
							const char * url = NULL;
							msg->FindString( "url", &url );

							BMessage* parse = new BMessage( UH_PARSE_DOC_START );
							parse->AddInt32( "command", COMMAND_INFO );
							parse->AddInt32( "view_id", id );
							parse->AddString( "url", url );
							Broadcast( MS_TARGET_HTML_PARSER, parse );
							delete parse;
						}

						entry->Print();
						
						// Broadcast the loading progress notify to the windows.
						BMessage* notify = new BMessage( UH_WIN_LOADING_PROGRESS );
						notify->AddInt32( "command", COMMAND_INFO );
						notify->AddInt32( "view_id", id );
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
					fEntryList.push_back( entry );
					
					/*
					 * I was planning to move all urlparsing and handler decision making from the windows
					 * URL_OPEN/BUTTON_RELOAD switch in here.
					 */
					
					break;
				}
				case UH_PARSE_DOC_FINISHED :
				{
					printf( "URLHANDLER: UH_PARSE_DOC_FINISHED\n" );
					
					/*
					 * The document parser delivers us with the pointer to the DOM tree data AND with the
					 * view-ID of the parsed site, when it is finished with parsing the document data.
					 * When everything is fine, I will Broadcast a message to the renderer with
					 * 'UH_RENDER_START' including the DOM tree and the view id.
					 * Additionally the UrlEntrys state is set to html_parsed. (To be implemented.)
					 *
					 * Broadcast message to be sent is the following:
					 * 		- uint32	"what" = UH_RENDER_START
					 *		- int32		"command" = COMMAND_INFO
					 * 		- pointer	"dom_tree_pointer"
					 *		- int32		"view_id"
					 */
					
					if( msg->HasString( "type" ) )
					{
						BString str;
						msg->FindString( "type", &str );
						if( strncmp( "dom", str.String(), 3 ) == 0 )
						{
							void* dom_ptr = NULL;
							msg->FindPointer( "dom_tree_pointer", &dom_ptr );
							if( dom_ptr == NULL )
							{
								printf( "URLHANDLER: dom_tree_pointer invalid!!\n" );
								break;
							}
							
							int32 view_id = 0;
							msg->FindInt32( "view_id", &view_id );
							
							BMessage* render = new BMessage( UH_RENDER_START );
							render->AddInt32( "command", COMMAND_INFO );
							render->AddPointer( "dom_tree_pointer", dom_ptr );
							render->AddInt32( "view_id", view_id );
							
							printf( "URLHANDLER: sending UH_RENDER_START\n" );
							
							Broadcast( MS_TARGET_RENDERER, render );
							
							delete render;							
						}
					}
						
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

void
UrlHandler::RemoveEntry(
	int32 id )
{
	fLocker->Lock();

	printf( "UrlHandler::RemoveEntry( %ld )\n", id );
	
	vector< UrlEntry* >::iterator it;
	for( it = fEntryList.begin(); it != fEntryList.end(); it++ )
	{
		if( ( ( UrlEntry* )*it )->GetID() == id )
		{
			printf( "URLHANDLER: Removing following item:\n" );
			UrlEntry* entry = *( it );
			entry->Print();
			delete entry;
			fEntryList.erase( it );
			break;
		}
	}
	
	fLocker->Unlock();	
}
