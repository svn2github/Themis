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
		if (((SiteEntry *)*it)->getId() == id) {
			entry = *it;
		}
	}

	fLocker->Unlock();
	return entry;

}

SiteEntry * SiteHandler :: AddEntry(int32 aSiteID, const char * aUrl) {

	SiteEntry * entry = new SiteEntry(aSiteID, aUrl);
	fEntryList.push_back(entry);
	
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
					int32 siteID;
					BString url;
					msg->FindInt32("site_id", &siteID);
					msg->FindString("url", &url);
					
					printf( "SiteHandler: adding following item: ID[%ld] URL[%s]\n", siteID, url.String() );
					
					SiteEntry * entry = AddEntry(siteID, url.String());
					/* Get an unique ID from the app */
					int32 urlID = ((App *)be_app)->GetNewID();
					UrlEntry * urlEntry = new UrlEntry(urlID, url.String());
					entry->addEntry(urlEntry);
				
					/*
					 * Inform the protocol(s) to retrieve the site.
					 * Remember, this is the workaround way, with an url_id > 0.
					 */
					
					BMessage* retrieve = new BMessage(SH_RETRIEVE_START);
					retrieve->AddInt32("command", COMMAND_RETRIEVE);
					retrieve->AddInt32("site_id", siteID);
					retrieve->AddInt32("url_id", urlID);
					retrieve->AddString("url", url.String());
					if (msg->what ==  SH_RELOAD_PAGE)
						retrieve->AddBool("reload", true);
						
					Broadcast(MS_TARGET_PROTOCOL, retrieve);
					delete retrieve;
					
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
		if (((SiteEntry *)*it)->getId() == id) {
			SiteEntry * entry = *(it);
			delete entry;
			fEntryList.erase(it);
			break;
		}
	}

	fLocker->Unlock();

}
