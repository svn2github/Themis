/*
 * SiteEntry.h
 */

#ifndef SITEENTRY_H
#define SITEENTRY_H

// Standard C++ headers
#include <vector.h>

// Namespaces used
using namespace std;

// Declarations used
class BString;
class BBitmap;
class UrlEntry;

class SiteEntry {

	private:
		vector<UrlEntry*> fEntryList;
		int32 fID;
		int8 fLoadingProgress;
		BString * fUrl;
		BString * fTitle;
		BString * fStatusText;
		bool fCookiesDisabled;
		bool fSecureConnection;
		BBitmap * fFavIcon;

	public:
		SiteEntry(int32 id,
				  const char* url);
	    ~SiteEntry();
		
		void AddEntry(int32 id,
					  const char* url);
		bool GetCookiesDisabled();
		BBitmap * GetFavIcon();
		UrlEntry * GetEntry(int32 id);
		int32 GetID();
		int8 GetLoadingProgress();
		bool GetSecureConnection();
		const char * GetStatusText();
		const char * GetTitle();
		const char * GetUrl();
		void Print();
		void SetCookiesDisabled(bool value);
		void SetFavIcon(BBitmap* bmp);
		void SetLoadingProgress(int8 loadingprogress);
		void SetSecureConnection(bool value);		
		void SetStatusText(const char* text);
		void SetTitle(const char* title);
									
};

#endif
