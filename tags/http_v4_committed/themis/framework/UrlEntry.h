/*
 * UrlEntry.h
 */

#ifndef URLENTRY_H
#define URLENTRY_H

#include <Bitmap.h>
#include <String.h>

class UrlEntry
{
	public:
									UrlEntry(
										int32 id,
										const char* url );
									
									~UrlEntry();
		
//		bool						GetCookiesDisabled();
		
//		BBitmap*					GetFavIcon();
		
		int32						GetID();

		int8						GetLoadingProgress();
		
		bool						GetSecureConnection();
		
//		const char*					GetStatusText();
		
		const char*					GetTitle();
		
		const char*					GetUrl();
		
		void						Print();
		
//		void						SetCookiesDisabled(
//										bool value );
		
//		void						SetFavIcon(
//										BBitmap* bmp );
		
		void						SetLoadingProgress(
										int8 loadingprogress );
		
		void						SetSecureConnection(
										bool value );		
		
//		void						SetStatusText(
//										const char* text );
				
		void						SetTitle(
										const char* title );
									
	private:
		int32						fID;
		
		int8						fLoadingProgress;
		
		BString*					fUrl;
		BString*					fTitle;
//		BString*					fStatusText;
		
//		bool						fCookiesDisabled;
		bool						fSecureConnection;
		
//		BBitmap*					fFavIcon;
};

#endif
