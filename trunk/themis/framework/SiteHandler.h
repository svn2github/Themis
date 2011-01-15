/*
 * SiteHandler.h
 */

#ifndef SITEHANDLER_H
#define SITEHANDLER_H

// Standard C++ headers
#include <vector.h>

// Themis headers
#include "msgsystem.h"

// Namespaces used
using namespace std;

// Declarations used
class BLocker;
class BBitmap;
class BMessage;
class SiteEntry;
class BaseEntry;
class CachePlug;

class SiteHandler : public MessageSystem {

	private:
		CachePlug *	fCachePlug;
		uint32 fCacheUserToken;
		BLocker * fLocker;
		vector<SiteEntry*> fEntryList;

	public:
		SiteHandler();
		~SiteHandler();

		SiteEntry * GetEntry(int32 id);
		void AddEntry(SiteEntry * aEntry);
		void AddEntry(BaseEntry * aEntry, int32 aSiteId, int32 aParentId);
		void BroadcastFinished();
		uint32 BroadcastTarget();
		status_t BroadcastReply(BMessage * msg);
		bool EntryValid(int32 id);
		BBitmap * GetFavIconFor(int32 id);
		int8 GetLoadingProgressFor(int32 id);
		const char * GetStatusTextFor(int32 id);
		const char * GetTitleFor(int32 id);
		const char * GetUrlFor(int32 id);
		status_t ReceiveBroadcast(BMessage* msg);
		void RemoveEntry(int32 id);
														
};

#endif
