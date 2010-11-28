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

SiteHandler :: SiteHandler()
			: MessageSystem("URL Handler") {

	// register myself with the message system
	MsgSysRegister( this );
	fCacheUserToken = 0;
	fLocker = new BLocker();

}

SiteHandler :: ~SiteHandler() {

	if (fCacheUserToken != 0)
		fCachePlug->Unregister(fCacheUserToken);

	MsgSysUnregister(this);

	// delete the whole entry list
	while (!fEntryList.empty()) {
		SiteEntry* ent = *(fEntryList.begin());
		fEntryList.erase(fEntryList.begin());
		delete ent;
	}

	delete fLocker;
}

void SiteHandler :: BroadcastFinished() {

}

status_t SiteHandler :: BroadcastReply(BMessage * msg) {

	return B_OK;

}


uint32 SiteHandler :: BroadcastTarget() {

	return MS_TARGET_SITEHANDLER;

}

bool SiteHandler :: EntryValid(int32 id) {

	if (GetEntry(id))
		return true;
	else
		return false;

}

SiteEntry * SiteHandler :: GetEntry(int32 id) {

	fLocker->Lock();
	SiteEntry* entry = NULL;
	
	// browse through the entry list to find the SiteEntry with the matching id
	
	vector< SiteEntry* >::iterator it;
	for (it = fEntryList.begin(); it != fEntryList.end(); it++) {
		if (((SiteEntry *)*it)->GetID() == id) {
			entry = *it;
		}
	}

	fLocker->Unlock();
	return entry;

}

BBitmap * SiteHandler :: GetFavIconFor(int32 id) {

	fLocker->Lock();
	SiteEntry* entry = GetEntry(id);

	if (entry == NULL) {
		fLocker->Unlock();
		return NULL;
	}

	fLocker->Unlock();	
	return entry->GetFavIcon();	

}

int8 SiteHandler :: GetLoadingProgressFor(int32 id) {

	fLocker->Lock();
	SiteEntry* entry = GetEntry(id);

	if (entry == NULL) {
		fLocker->Unlock();
		return -1;
	}

	fLocker->Unlock();	
	return entry->GetLoadingProgress();

}

const char * SiteHandler :: GetStatusTextFor(int32 id) {

	fLocker->Lock();
	BString title;
	SiteEntry * entry = GetEntry(id);
	
	if (entry == NULL) {
		fLocker->Unlock();
		return "";
	}

	fLocker->Unlock();	
	return entry->GetStatusText();

}

const char * SiteHandler :: GetTitleFor(int32 id) {

	fLocker->Lock();
	BString title;
	SiteEntry * entry = GetEntry(id);
	
	if (entry == NULL) {
		printf( "  entry NULL\n" );
		fLocker->Unlock();
		return "";
	}
	
	fLocker->Unlock();	
	return entry->GetTitle();

}

const char * SiteHandler :: GetUrlFor(int32 id) {

	fLocker->Lock();
	BString title;
	SiteEntry * entry = GetEntry(id);
	
	if (entry == NULL) {
		fLocker->Unlock();
		return "";
	}

	fLocker->Unlock();	
	return entry->GetUrl();

}

status_t SiteHandler :: ReceiveBroadcast(BMessage* msg) {

	int32 command = 0;
	msg->FindInt32("command", &command);
	
	switch(command) {
		case COMMAND_INFO: {
			switch(msg->what) {
				case PlugInLoaded: {
					int32 plugid = 0;
					msg->FindInt32("plugid", &plugid);
					
					if (plugid == CachePlugin) {
						// register with the cache system
						fCachePlug = (CachePlug *)PluginManager->FindPlugin(CachePlugin);
						if (fCachePlug != NULL) {
							fCacheUserToken = fCachePlug->Register(MS_TARGET_SITEHANDLER);
						}
					}
					break;					
				}
				case SH_LOAD_NEW_PAGE:
				case SH_RELOAD_PAGE: {
					/* create a SiteEntry, and add it to fEntryList */
					int32 site_id;
					BString url;
					msg->FindInt32("site_id", &site_id);
					msg->FindString("url", &url);
					
					printf( "SiteHandler: adding following item: ID[%ld] URL[%s]\n", site_id, url.String() );
					
					SiteEntry * siteentry = new SiteEntry(site_id, url.String());
					fEntryList.push_back(siteentry);
					
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
					int32 url_id = ((App *)be_app)->GetNewID();
					siteentry->AddEntry(url_id, url.String());

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
					
					BMessage* retrieve = new BMessage(SH_RETRIEVE_START);
					retrieve->AddInt32("command", COMMAND_RETRIEVE);
					retrieve->AddInt32("site_id", site_id);
					retrieve->AddInt32("url_id", url_id);
					retrieve->AddString("url", url.String());
					if (msg->what ==  SH_RELOAD_PAGE)
						retrieve->AddBool("reload", true);
						
					Broadcast(MS_TARGET_PROTOCOL, retrieve);
					delete retrieve;
					
					break;
				}
				case SH_PARSE_DOC_FINISHED :
				{
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
					
					if (msg->HasString( "type")) {
						BString str;
						msg->FindString("type", &str);
						if( strncmp("dom", str.String(), 3) == 0) {
							void * dom_ptr = NULL;
							msg->FindPointer("dom_tree_pointer", &dom_ptr);
							if (dom_ptr == NULL) {
								printf("SiteHandler: dom_tree_pointer invalid!!\n");
								break;
							}

							int32 site_id = 0;
							int32 url_id = 0;
							msg->FindInt32("site_id", &site_id);
							msg->FindInt32("url_id", &url_id);
							
							BMessage * render = new BMessage(SH_RENDER_START);
							render->AddInt32("command", COMMAND_INFO);
							render->AddPointer("dom_tree_pointer", dom_ptr);
							render->AddInt32("site_id", site_id);
							render->AddInt32("url_id", url_id);
							Broadcast(MS_TARGET_RENDERER, render);
							
							delete render;							
						}
					}

					break;
				}
			}
			break;
		}
		case COMMAND_RETRIEVE: {
			break;
		}
	}

	return B_OK;

}

void SiteHandler :: RemoveEntry(int32 id) {

	fLocker->Lock();

	vector<SiteEntry *>::iterator it;
	for (it = fEntryList.begin(); it != fEntryList.end(); it++) {
		if (((SiteEntry *)*it)->GetID() == id) {
			SiteEntry * entry = *(it);
			delete entry;
			fEntryList.erase(it);
			break;
		}
	}

	fLocker->Unlock();

}
