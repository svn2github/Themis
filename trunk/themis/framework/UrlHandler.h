/*
 * UrlHandler.h
 */

#ifndef URLHANDLER_H
#define URLHANDLER_H

#include "msgsystem.h"
#include "../common/cacheplug.h"

class UrlHandler : public MessageSystem 
{
	public:
									UrlHandler();
									
									~UrlHandler();
		
		void						BroadcastFinished();
									
		uint32						BroadCastTarget();
		
		status_t					BroadcastReply(
										BMessage* msg );
		
		status_t					ReceiveBroadCast(
										BMessage* msg );
														
	private:
		CachePlug*					fCachePlug;
		uint32						fCacheUserToken;
	
};

#endif
