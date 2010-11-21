/*
 * SiteHandler.cpp
 */

// BeOS headers
#include <Locker.h>
#include <Bitmap.h>

// Standard C headers
#include <stdio.h>

// Themis headers
#include "app.h"
#include "plugman.h"
#include "ThemisIcons.h"
#include "../common/commondefs.h"
#include "../common/cacheplug.h"
#include "UrlEntry.h"
#include "SiteEntry.h"
#include "SiteHandler.h"

extern plugman* PluginManager;

SiteHandler::SiteHandler()
	: MessageSystem("URL Handler")
{
	// register myself with the message system
	MsgSysRegister( this );
	
	fCacheUserToken = 0;
	
	fLocker = new BLocker();
}

SiteHandler::~SiteHandler()
{
	if( fCacheUserToken != 0 )
		fCachePlug->Unregister( fCacheUserToken );
	
	MsgSysUnregister( this );
	
	// delete the whole entry list
	while( !fEntryList.empty() )
	{
		SiteEntry* ent = *( fEntryList.begin() );
		fEntryList.erase( fEntryList.begin() );
		delete ent;
	}
	
	delete fLocker;
}

void
SiteHandler::BroadcastFinished()
{
//	printf( "SiteHandler::BroadcastFinished()\n" );
}

status_t
SiteHandler::BroadcastReply(
	BMessage* msg )
{
//	printf( "SiteHandler::BroadcastReply()\n" );
	
	return B_OK;
}


uint32
SiteHandler::BroadcastTarget()
{
//	printf( "SiteHandler::BroadCastTarget()\n" );
	
	return MS_TARGET_SITEHANDLER;
}

bool
SiteHandler::EntryValid(
	int32 id )
{
	if( GetEntry( id ) )
		return true;
	else
		return false;
}

SiteEntry*
SiteHandler::GetEntry(
	int32 id )
{
	fLocker->Lock();
	
	printf( "SiteHandler::GetEntry( %ld )\n", id );
	
	SiteEntry* entry = NULL;
	
	// browse through the entry list to find the SiteEntry with the matching id
	
	vector< SiteEntry* >::iterator it;
	for( it = fEntryList.begin(); it != fEntryList.end(); it++ )
	{
		if( ( ( SiteEntry* )*it )->GetID() == id )
		{
			entry = *it;
		}
	}

	fLocker->Unlock();
	return entry;
}

BBitmap*
SiteHandler::GetFavIconFor(
	int32 id )
{
	fLocker->Lock();

	printf( "SiteHandler::GetFavIconFor( %ld )\n", id );
	
	SiteEntry* entry = GetEntry( id );

	if( entry == NULL )
	{
		fLocker->Unlock();
		return NULL;
	}
	
	fLocker->Unlock();	
	return entry->GetFavIcon();	
}

int8
SiteHandler::GetLoadingProgressFor(
	int32 id )
{
	fLocker->Lock();

	printf( "SiteHandler::GetLoadingProgressFor( %ld )\n", id );
	
	SiteEntry* entry = GetEntry( id );

	if( entry == NULL )
	{
		fLocker->Unlock();
		return -1;
	}
	
	fLocker->Unlock();	
	return entry->GetLoadingProgress();
}

const char*
SiteHandler::GetStatusTextFor(
	int32 id )
{
	fLocker->Lock();

	printf( "SiteHandler::GetStatusTextFor( %ld )\n", id );
	
	BString title;
	
	SiteEntry* entry = GetEntry( id );
	
	if( entry == NULL )
	{
		fLocker->Unlock();
		return "";
	}

	fLocker->Unlock();	
	return entry->GetStatusText();
}

const char*
SiteHandler::GetTitleFor(
	int32 id )
{
	fLocker->Lock();

	printf( "SiteHandler::GetTitleFor( %ld )\n", id );
	
	BString title;
	
	SiteEntry* entry = GetEntry( id );
	
	if( entry == NULL )
	{
		printf( "  entry NULL\n" );
		fLocker->Unlock();
		return "";
	}
	
	fLocker->Unlock();	
	return entry->GetTitle();
}

const char*
SiteHandler::GetUrlFor(
	int32 id )
{
	fLocker->Lock();

	printf( "SiteHandler::GetUrlFor( %ld )\n", id );
	
	BString title;
	
	SiteEntry* entry = GetEntry( id );
	
	if( entry == NULL )
	{
		fLocker->Unlock();
		return "";
	}

	fLocker->Unlock();	
	return entry->GetUrl();
}

status_t
SiteHandler::ReceiveBroadcast(
	BMessage* msg )
{
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
//					printf( "SiteHandler: PlugInLoaded\n" );
					
					int32 plugid = 0;
					msg->FindInt32( "plugid", &plugid );
					
					if( plugid == CachePlugin )
					{
						// register with the cache system
						fCachePlug = ( CachePlug* )PluginManager->FindPlugin( CachePlugin );
						if( fCachePlug != NULL )
						{
							fCacheUserToken = fCachePlug->Register( MS_TARGET_SITEHANDLER );
							printf( "SiteHandler: Registered with CachePlug.\n" );
						}
						else
						{
							printf( "SiteHandler: Couldn't register with CachePlug!\n" );
						}
					}
					break;					
				}
				case SH_LOAD_NEW_PAGE :
				case SH_RELOAD_PAGE :
				{
					printf( "SITEHANDLER: SH_LOAD_NEW_PAGE/SH_RELOAD_PAGE\n" );
					
					/* create an SiteEntry, and add it to fEntryList */
					
					int32 site_id;
					BString url;
					
					msg->FindInt32( "site_id", &site_id );
					msg->FindString( "url", &url );
					
					printf( "SiteHandler: adding following item: ID[%ld] URL[%s]\n", site_id, url.String() );
					
					SiteEntry* siteentry = new SiteEntry( site_id, url.String() );
					fEntryList.push_back( siteentry );
					
					/*
					 * GENERAL PROCESSING IDEA:
					 * 
					 * The main idea now would be to tell the network, to retrieve the site. When loading is
					 * finished, the parser would be given the url for parsing. The parser then tells us if
					 * the document is a single-framed or multi-framed document. According to this info,
					 * we create the appropriate count of UrlEntry items in the parent SiteEntry.
					 * Each UrlEntry for the site is then to be loaded from the network system, and loading
					 * states are reported back accordingly.
					 * An idea would be, that as soon, an url of the site is retrieved, the parser gets it for
					 * parsing, and then returns the pointer to the dom tree, which is stored in the appropriate
					 * UrlEntry for later processing by the Renderer.
					 * If all URLs for the site are retrieved, we hand over all the dom tree pointers and stuff
					 * to the Renderer with the SH_RENDER_START broadcast.
					 * Though it is not said, that we may not even tell the Renderer to render parts ( URLs ) of
					 * the site in between.
					 *
					 * Note: The first retrieve will contain an site_id > 0 and an url_id = 0. This makes it clear
					 * for the SiteHandler that the response from the network is for the intial URL.
					 */
					 
					/*
					 * WORKAROUND IDEA:
					 *
					 * As we currently can't ask the parser to tell us about the single- and/or multi-framed-ness
					 * of a site, we just create an UrlEntry and store it in the SiteEntry.
					 * Then we tell the network to retrieve exactly that site_id/url_id combination.
					 * This means site_id > 0 and url_id > 0.
					 */
					
					/* Go the workaround way */
					
					/* Get an unique ID from the app */
					int32 url_id = ( ( App* )be_app )->GetNewID();
					
					siteentry->AddEntry( url_id, url.String() );

					/*
					 * TODO
					 * 
					 * I was planning to move all urlparsing and handler decision making from the windows
					 * URL_OPEN/BUTTON_RELOAD switch in here.
					 */

					
					/*
					 * Inform the protocol(s) to retrieve the site.
					 * Remember, this is the workaround way, with an url_id > 0.
					 */
					
					BMessage* retrieve = new BMessage( SH_RETRIEVE_START );
					retrieve->AddInt32( "command", COMMAND_RETRIEVE );
					retrieve->AddInt32( "site_id", site_id );
					retrieve->AddInt32( "url_id", url_id );
					retrieve->AddString( "url", url.String() );
					if ( msg->what ==  SH_RELOAD_PAGE)
						retrieve->AddBool( "reload", true );
						
					Broadcast( MS_TARGET_PROTOCOL, retrieve );
					
					delete retrieve;
					
					printf( "SITEHANDLER: Broadcast to protocol(s) sent!\n" );
					
					break;
				}
				case SH_LOADING_PROGRESS :
				{
					printf( "SITEHANDLER: SH_LOADING_PROGRESS\n" );
					
					/*
					 * TODO
					 * 
					 * We later need to distinquish between inital network site retrieval response, and the
					 * subsequent url retrieval messages.
					 */
					
					/* Find the SiteEntry with the appropriate UrlEntry */
					
					int32 site_id;
					int32 url_id;
					
					msg->FindInt32( "site_id", &site_id );
					msg->FindInt32( "url_id", &url_id );
					
					SiteEntry* site_entry = GetEntry( site_id );
					UrlEntry* url_entry = NULL;

					if ( site_entry != NULL )
						url_entry = site_entry->GetEntry( url_id );
					
					if( !site_entry || !url_entry )
					{
						printf( "SITEHANDLER: SiteEntry [%ld] and/or UrlEntry [%ld] not found! Aborting!\n", site_id, url_id );
						break;
					}
					
					int64 content_length = 0;
					int64 bytes_received = 0;
					int64 size_delta = 0;
					bool request_done = false;
					bool secure = false;
					
					msg->FindInt64( "bytes-received", &bytes_received);
					msg->FindInt64( "size-delta", &size_delta);
					msg->FindBool( "request_done", &request_done );
					msg->FindBool( "secure", &secure );					
					
					if( msg->HasInt64( "content-length" ) )
						msg->FindInt64( "content-length", &content_length );

					// When we do have some info about cookies, set it here.
					// url_entry->SetCookiesDisabled( cookies_disabled );
						
					site_entry->SetSecureConnection( secure );
						
					if( request_done == false )
					{
						printf( "SiteHandler: Document is still loading...\n" );
							
						int8 loadprogress = -1;
					
						if( content_length != 0 )
						{
							loadprogress = ( int8 )( ( bytes_received / content_length ) * 100 );
						}
						else	// chunked transfer mode
						{
							loadprogress = 50;
						}
							url_entry->SetLoadingProgress( loadprogress );
					}
					else
					{
						printf( "SiteHandler: Document loaded completely!\n" );
							
						url_entry->SetLoadingProgress( 100 );
							
						/*
						 * As long, as we have no FavIcon loading ready, I do set
						 * the non-empty document icon here.
						 */
						BBitmap* bmp = new BBitmap(
							BRect( 0, 0, 15,15 ),
							B_RGB32 );
						memcpy( bmp->Bits(), icon_document_hex, 1024 );
						site_entry->SetFavIcon( bmp );
						delete bmp;
					}

					site_entry->Print();
					
					/*
					 * Now check if the site is loaded completely. If so, broadcast SH_PARSE_DOC_START.
					 */
					
					if( site_entry->GetLoadingProgress() == 100 )
					{
						/* Now set up the UH_PARSE_DOC_START message. */
						printf( "SiteHandler: sending SH_PARSE_DOC_START\n" );
						
						/*
						 * As the file protocol returns a file system url, we grab the url from the
						 * loading progress message, and attach it to the broadcast.
						 */
						const char* url = NULL;
						msg->FindString( "url", &url );
						
						BMessage* parse = new BMessage( SH_PARSE_DOC_START );
						parse->AddInt32( "command", COMMAND_INFO );
						parse->AddInt32( "site_id", site_id );
						parse->AddInt32( "url_id", url_id );
						parse->AddString( "url", url );
						Broadcast( MS_TARGET_HTML_PARSER, parse );
						delete parse;
					}
						
					/* Broadcast the loading progress notify to the windows. */
					BMessage* notify = new BMessage( SH_WIN_LOADING_PROGRESS );
					notify->AddInt32( "command", COMMAND_INFO );
					notify->AddInt32( "site_id", site_id );
					Broadcast( MS_TARGET_WINDOW, notify );
					delete notify;
					
					break;
				}
				case SH_PARSE_DOC_FINISHED :
				{
					printf( "SiteHandler: SH_PARSE_DOC_FINISHED\n" );
					
					/* The following is still the workaround way, as we have no multi-framed rendering stuff. */
					
					/*
					 * The document parser delivers us with the pointer to the DOM tree data AND with the
					 * site/url_id combination of the parsed url, when it is finished with parsing the document data.
					 * When everything is fine, I will Broadcast a message to the renderer with
					 * 'SH_RENDER_START' including the DOM tree and the view id.
					 * Additionally the UrlEntrys state is set to html_parsed. (To be implemented.)
					 *
					 * Broadcast message to be sent is the following:
					 * 		- uint32	"what" = UH_RENDER_START
					 *		- int32		"command" = COMMAND_INFO
					 * 		- pointer	"dom_tree_pointer"
					 *		- int32		"site_id"
					 * 		- int32		"url_id"
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
								printf( "SiteHandler: dom_tree_pointer invalid!!\n" );
								break;
							}
							
							int32 site_id = 0;
							int32 url_id = 0;
							msg->FindInt32( "site_id", &site_id );
							msg->FindInt32( "url_id", &url_id );
							
							BMessage* render = new BMessage( SH_RENDER_START );
							render->AddInt32( "command", COMMAND_INFO );
							render->AddPointer( "dom_tree_pointer", dom_ptr );
							render->AddInt32( "site_id", site_id );
							render->AddInt32( "url_id", url_id );
							
							printf( "SiteHandler: sending SH_RENDER_START\n" );
							
							Broadcast( MS_TARGET_RENDERER, render );
							
							delete render;							
						}
					}
						
					break;
				}
				case SH_RENDER_FINISHED :
				{
					printf( "SiteHandler: SH_RENDER_FINISHED\n" );
					
					/*
					 * do nothing for now ;)
					 * 
					 */
					
					break;
				}
//				case UH_DOC_CLOSED :
//				{
//					printf( "SiteHandler: UH_DOC_CLOSED\n" );
//					
//					int32 view_id = 0;
//					msg->FindInt32( "view_id", &view_id );
//					
//					RemoveEntry( view_id );
//					
//					break;
//				}
//				case UH_LOADING_FINISHED :
//				case ProtocolConnectionClosed :
//				{
//					//printf( "SiteHandler: ProtocolConnectionClosed/UH_LOADING_FINISHED\n" );
//					
//					/*
//					 * Set the UrlEntrys state to loading complete. (To be implemented.)
//					 * Tell the document parser to start parsing the loaded data.
//					 * Broadcast message is the following:
//					 * 		- uin32		"what" = UH_PARSE_DOC_START
//					 *		- int32		"command" = COMMAND_INFO
//					 * 		- bool		"request_done" 	// Probably removable, as I can check here.
//					 * 		- string	"mimetype"
//					 * 		- string	"url"
//					 * 		- int32		"view_id"
//					 */
//					 
//					 /*
//					  * With Marks idea of grouping modules/add-ons, I guess this broadcast should go to
//					  * MS_TARGET_PARSER if i am not mistaken, right?
//					  */
//					
//					//break;
//				}
//				case UH_LOADING_PROGRESS :
//				case ReturnedData :	// we need to find better specified constants, or take the above one :)
//				{
//					printf( "SiteHandler: ReturnedData/UH_LOADING_PROGRESS\n" );
//					
//					//msg->PrintToStream();
//					
//					int32 id = 0;
//					int64 contentlength = 0;
//					int64 bytes_received = 0;
//					int64 size_delta = 0;
//					bool request_done = false;
//					bool secure = false;	
//					
//					// Due to every kind of data delivered with 'ReturnedData' we make a little sanity
//					// check here to avoid problems.
//					if( msg->HasInt32( "view_id" ) == false )
//						break;
//					
//					msg->FindInt32( "view_id", &id );
//					msg->FindInt64( "bytes-received", &bytes_received);
//					msg->FindInt64( "size-delta", &size_delta);
//					msg->FindBool( "request_done", &request_done );
//					msg->FindBool( "secure", &secure );					
//					
//					if( msg->HasInt64( "content-length" ) )
//						msg->FindInt64( "content-length", &contentlength );
//					
//					// Put all the info in the UrlEntry.
//					UrlEntry* entry = GetEntry( id );
//					
//					if( entry != NULL )
//					{
//						// When we do have some info about cookies, set it here.
//						//SetCookiesDisabled( cookies_disabled );
//						
//						entry->SetSecureConnection( secure );
//						
//						if( request_done == false )
//						{
//							printf( "SiteHandler: Document is still loading...\n" );
//							
//							int8 loadprogress = -1;
//						
//							if( contentlength != 0 )
//							{
//								loadprogress = ( int8 )( ( bytes_received / contentlength ) * 100 );
//							}
//							else	// chunked transfer mode
//							{
//								loadprogress = 50;
//							}
//							entry->SetLoadingProgress( loadprogress );
//						}
//						else
//						{
//							printf( "SiteHandler: Document loaded completely!\n" );
//							
//							entry->SetLoadingProgress( 100 );
//							
//							/*
//							 * As long, as we have no FavIcon loading ready, I do set
//							 * the non-empty document icon here.
//							 */
//							BBitmap* bmp = new BBitmap(
//								BRect( 0, 0, 15,15 ),
//								B_RGB32 );
//							memcpy( bmp->Bits(), icon_document_hex, 1024 );
//							entry->SetFavIcon( bmp );
//							delete bmp;
//							
//							/*
//							 * Now set up the UH_PARSE_DOC_START message.
//							 */
//							printf( "SiteHandler: sending UH_PARSE_DOC_START\n" );
//							
//							const char * url = NULL;
//							msg->FindString( "url", &url );
//
//							BMessage* parse = new BMessage( UH_PARSE_DOC_START );
//							parse->AddInt32( "command", COMMAND_INFO );
//							parse->AddInt32( "view_id", id );
//							parse->AddString( "url", url );
//							Broadcast( MS_TARGET_HTML_PARSER, parse );
//							delete parse;
//						}
//
//						entry->Print();
//						
//						// Broadcast the loading progress notify to the windows.
//						BMessage* notify = new BMessage( UH_WIN_LOADING_PROGRESS );
//						notify->AddInt32( "command", COMMAND_INFO );
//						notify->AddInt32( "view_id", id );
//						Broadcast( MS_TARGET_WINDOW, notify );
//						delete notify;
//					}
//					else
//					{
//						printf( "SiteHandler: UrlEntry not valid anymore!!!!!!!!\n" );
//						
//						/*
//						 * Tell http/network layer to stop loading the page.
//						 * Raymond, perhaps you can give me some hint on that. Thanks.
//						 */
//					}
//					
//					break;
//				}
//				case UH_LOAD_NEW_PAGE :
//				{
//					printf( "SiteHandler: UH_LOAD_NEW_PAGE\n" );
//					
//					// create a new UrlEntry item and store it in the entry list
//					
//					int32 id;
//					BString url;
//					
//					msg->FindInt32( "view_id", &id );
//					msg->FindString( "target_url", &url );
//					
//					printf( "SiteHandler: adding following item: ID[%ld] URL[%s]\n", id, url.String() ); 
//					
//					UrlEntry* entry = new UrlEntry( id, url.String() );
//					fEntryList.push_back( entry );
//					
//					/*
//					 * I was planning to move all urlparsing and handler decision making from the windows
//					 * URL_OPEN/BUTTON_RELOAD switch in here.
//					 */
//					
//					break;
//				}
//				case UH_PARSE_DOC_FINISHED :
//				{
//					printf( "SiteHandler: UH_PARSE_DOC_FINISHED\n" );
//					
//					/*
//					 * The document parser delivers us with the pointer to the DOM tree data AND with the
//					 * view-ID of the parsed site, when it is finished with parsing the document data.
//					 * When everything is fine, I will Broadcast a message to the renderer with
//					 * 'UH_RENDER_START' including the DOM tree and the view id.
//					 * Additionally the UrlEntrys state is set to html_parsed. (To be implemented.)
//					 *
//					 * Broadcast message to be sent is the following:
//					 * 		- uint32	"what" = UH_RENDER_START
//					 *		- int32		"command" = COMMAND_INFO
//					 * 		- pointer	"dom_tree_pointer"
//					 *		- int32		"view_id"
//					 */
//					
//					if( msg->HasString( "type" ) )
//					{
//						BString str;
//						msg->FindString( "type", &str );
//						if( strncmp( "dom", str.String(), 3 ) == 0 )
//						{
//							void* dom_ptr = NULL;
//							msg->FindPointer( "dom_tree_pointer", &dom_ptr );
//							if( dom_ptr == NULL )
//							{
//								printf( "SiteHandler: dom_tree_pointer invalid!!\n" );
//								break;
//							}
//							
//							int32 view_id = 0;
//							msg->FindInt32( "view_id", &view_id );
//							
//							BMessage* render = new BMessage( UH_RENDER_START );
//							render->AddInt32( "command", COMMAND_INFO );
//							render->AddPointer( "dom_tree_pointer", dom_ptr );
//							render->AddInt32( "view_id", view_id );
//							
//							printf( "SiteHandler: sending UH_RENDER_START\n" );
//							
//							Broadcast( MS_TARGET_RENDERER, render );
//							
//							delete render;							
//						}
//					}
//						
//					break;
//				}
//				case UH_RELOAD_PAGE :
//				{
//					printf( "SiteHandler: UH_RELOAD_PAGE\n" );
//					
//					// TODO
//					
//					break;
//				}
//				case UH_RENDER_FINISHED :
//				{
//					printf( "SiteHandler: UH_RENDER_FINISHED\n" );
//					
//					/*
//					 * The renderer just informs us about that it has finished rendering the page.
//					 * So we set the UrlEntrys state to rendered. (To be implemented.)
//					 *
//					 * The Broadcast I expect here is the following:
//					 * 		- uint32	"what" = UH_RENDER_FINISHED
//					 * 		- int32		"command" = COMMAND_INFO
//					 * 		- int32		"view_id"
//					 */
//					
//					break;
//				}
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
SiteHandler::RemoveEntry(
	int32 id )
{
	fLocker->Lock();

	printf( "SiteHandler::RemoveEntry( %ld )\n", id );
	
	vector< SiteEntry* >::iterator it;
	for( it = fEntryList.begin(); it != fEntryList.end(); it++ )
	{
		if( ( ( SiteEntry* )*it )->GetID() == id )
		{
			printf( "SiteHandler: Removing following item:\n" );
			SiteEntry* entry = *( it );
			entry->Print();
			delete entry;
			fEntryList.erase( it );
			break;
		}
	}
	
	fLocker->Unlock();	
}
