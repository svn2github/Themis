/*
 * UrlEntry.h
 */

#ifndef URLENTRY_H
#define URLENTRY_H

#include <String.h>

#include <be_prim.h>	// missing int types :/

class UrlEntry
{
	public:
									UrlEntry(
										int32 id,
										const char* url );
									
									~UrlEntry();
		
		bool						GetCookiesDisabled();
		
		int32						GetID();

		int8						GetLoadingProgress();
		
		bool						GetSecureConnection();
		
		const char*					GetTitle();
		
		void						Print();
		
		void						SetCookiesDisabled(
										bool value );
		
		void						SetLoadingProgress(
										int8 loadingprogress );
		
		void						SetSecureConnection(
										bool value );		
				
		void						SetTitle(
										const char* title );
									
	private:
		int32						fID;
		
		int8						fLoadingProgress;
		
		BString*					fUrl;
		BString*					fTitle;
		
		bool						fCookiesDisabled;
		bool						fSecureConnection;
};

#endif
