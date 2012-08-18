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
#include "../common/BaseEntry.hpp"
#include "UrlEntry.h"
#include "SiteEntry.h"
#include "SiteHandler.h"

extern plugman* PluginManager;

SiteHandler :: SiteHandler()
			: MessageSystem("URL Handler") {

	// register myself with the message system
	MsgSysRegister( this );
	fLocker = new BLocker();

}

SiteHandler :: ~SiteHandler() {

	MsgSysUnregister(this);

	// delete the whole entry list
	while (!fEntryList.empty()) {
		SiteEntry* ent = *(fEntryList.begin());
		fEntryList.erase(fEntryList.begin());
		delete ent;
	}

	delete fLocker;
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

void SiteHandler :: AddEntry(SiteEntry * aEntry) {
	
	fLocker->Lock();

	fEntryList.push_back(aEntry);

	fLocker->Unlock();
}

void SiteHandler :: AddEntry(BaseEntry * aEntry,
							 int32 aSiteId,
							 int32 aParentId) {

	SiteEntry * site = GetEntry(aSiteId);

	if (site != NULL) {
		fLocker->Lock();
		
		BaseEntry * parent = site->getEntry(aParentId);
		parent->addEntry(aEntry);
		
		fLocker->Unlock();
	}

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
