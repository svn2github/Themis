/*
 * UrlHandler.h
 */

#ifndef URLHANDLER_H
#define URLHANDLER_H

#include <Locker.h>

#include <vector.h>

#include "msgsystem.h"
#include "../common/cacheplug.h"
#include "UrlEntry.h"

using namespace std;

class UrlHandler : public MessageSystem 
{
	public:
									UrlHandler();
									
									~UrlHandler();
		
		void						BroadcastFinished();
									
		uint32						BroadcastTarget();
		
		status_t					BroadcastReply(
										BMessage* msg );

		int8						GetLoadingProgressFor(
										int32 id );
		
		const char*					GetTitleFor(
										int32 id );
		
		status_t					ReceiveBroadcast(
										BMessage* msg );
														
	private:
		UrlEntry*					GetEntry(
										int32 id );
		
		CachePlug*					fCachePlug;
		uint32						fCacheUserToken;
		
		BLocker*					fLocker;
		
		vector< UrlEntry >			fEntryList;
	
};

#endif
