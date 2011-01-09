/*
 * SiteEntry.h
 */

#ifndef SITEENTRY_H
#define SITEENTRY_H

// Standard C++ headers
#include <vector.h>

// Themis headers
#include "BaseEntry.hpp"

// Namespaces used
using namespace std;

// Declarations used
class BBitmap;
class UrlEntry;

class SiteEntry : public BaseEntry {

	public:
		SiteEntry(int32 id,
				  const char * url);
		
		bool GetCookiesDisabled();
		BBitmap * GetFavIcon();
		int GetLoadingProgress();
		bool GetSecureConnection();
		const char * GetStatusText();
		const char * GetTitle();
		const char * GetUrl();
		void Print();
		void SetCookiesDisabled(bool value);
		void SetFavIcon(BBitmap * bmp);
		void SetLoadingProgress(int loadingprogress);
		void SetSecureConnection(bool value);		
		void SetStatusText(const char * text);
		void SetTitle(const char * title);
									
};

#endif
