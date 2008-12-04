/*
 * SiteHandler.h
 */

#ifndef SITEHANDLER_H
#define SITEHANDLER_H

#include <Locker.h>

#include <vector.h>

#include "msgsystem.h"
#include "../common/cacheplug.h"
#include "SiteEntry.h"

using namespace std;

class SiteHandler : public MessageSystem 
{
	public:
									SiteHandler();
									
									~SiteHandler();
		
		void						BroadcastFinished();
									
		uint32						BroadcastTarget();
		
		status_t					BroadcastReply(
										BMessage* msg );

		bool						EntryValid(
										int32 id );		
		
		BBitmap*					GetFavIconFor(
										int32 id );
		
		int8						GetLoadingProgressFor(
										int32 id );
		
		const char*					GetStatusTextFor(
										int32 id );
		
		const char*					GetTitleFor(
										int32 id );
		
		const char*					GetUrlFor(
										int32 id );
		
		status_t					ReceiveBroadcast(
										BMessage* msg );
		
		void						RemoveEntry(
										int32 id );
														
	private:
		SiteEntry*					GetEntry(
										int32 id );
		
		CachePlug*					fCachePlug;
		uint32						fCacheUserToken;
		
		BLocker*					fLocker;
		
		vector< SiteEntry* >		fEntryList;
};

#endif
