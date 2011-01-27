/*
 * SiteEntry.h
 */

#ifndef SITEENTRY_H
#define SITEENTRY_H

// Themis headers
#include "UrlEntry.h"

// Declarations used
class BBitmap;

class SiteEntry : public UrlEntry {

	public:
		SiteEntry(int32 id,
				  const char * url);
		
		bool GetCookiesDisabled();
		BBitmap * GetFavIcon();
		virtual int GetLoadingProgress();
		const char * GetStatusText();
		virtual void Print();
		void SetCookiesDisabled(bool value);
		void SetFavIcon(BBitmap * bmp);
		virtual void SetLoadingProgress(int loadingprogress);
		void SetStatusText(const char * text);
		void SetSize(int width, int height);
		void GetSize(int & width, int & height);
									
};

#endif
