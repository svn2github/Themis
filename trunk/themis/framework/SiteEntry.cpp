/*
 * SiteEntry.cpp
 */

// BeOS headers
#include <Bitmap.h>
#include <String.h>

// Standard C headers
#include <stdio.h>

// Themis headers
#include "SiteEntry.h"
#include "UrlEntry.h"

// Constants declared
const string kUrl = "Url";
const string kTitle = "Title";
const string kStatusText = "StatusText";
const string kSecureConnection = "SecureConnection";
const string kCookiesDisabled = "CookiesDisabled";
const string kLoadingProgress = "LoadingProgress";
const string kFavIcon = "FavIcon";

SiteEntry :: SiteEntry(int32 id,
					   const char * url) : BaseEntry(id) {

	set(kLoadingProgress, -1);
	set(kUrl, url);

	string statusText = "Transfering data from ";
	statusText += url;
	statusText += " ...";
	set(kStatusText, statusText);
	
	/*
	 * The page title is set to "loading..." now.
	 * When loading is finished, its set to the sites url.
	 * Then we got two options. Either I do grab the page title
	 * from the DOM tree, when the HTML parser is finished, or
	 * I wait for the renderer to finish, which then delivers me
	 * the page title.
	 */
	set(kTitle, "loading...");
	
	set(kSecureConnection, false);
	set(kCookiesDisabled, false);
	
}

bool SiteEntry :: GetCookiesDisabled() {

	return getBoolean(kCookiesDisabled);

}

BBitmap * SiteEntry :: GetFavIcon() {

	return (BBitmap *) getPointer(kFavIcon);

}

int SiteEntry :: GetLoadingProgress() {

	/* cycle through the list of UrlEntries and calculate the loading progress */
	int progress = 0;
	unsigned int nrOfEntries = fChildEntries.size();
	for (unsigned int i = 0; i < nrOfEntries; i++) {
		UrlEntry * entry = (UrlEntry *) fChildEntries[i];
		progress += entry->GetLoadingProgress();
	}

	progress = (int)(progress / nrOfEntries);
	SetLoadingProgress(progress);

	return progress;

}

bool SiteEntry :: GetSecureConnection() {

	return getBoolean(kSecureConnection);

}

const char * SiteEntry :: GetStatusText() {

	return getString(kStatusText).c_str();

}

const char  * SiteEntry :: GetTitle() {

	return getString(kTitle).c_str();

}

const char * SiteEntry :: GetUrl() {

	return getString(kUrl).c_str();

}

void SiteEntry :: Print() {

	printf("------------------------------------\n");
	printf("SiteEntry: ID[%ld] URL[%s] TITLE[%s]\n", getId(), getString(kUrl).c_str(), getString(kTitle).c_str());
	printf("           LoadingProgess[%d] CookiesDisabled[%s], SecureConnection[%s]\n",
		getInteger(kLoadingProgress),
		getBoolean(kCookiesDisabled) ? "true" : "false",
		getBoolean(kSecureConnection) ? "true" : "false");

	printf("  -- SiteEntry UrlEntries --\n");

	vector<BaseEntry *>::iterator it;
	for (it = fChildEntries.begin(); it != fChildEntries.end(); it++) {
		((UrlEntry *)*it)->Print();
	}

	printf( "------------------------------------\n" );

}

void SiteEntry :: SetCookiesDisabled(bool value) {

	set(kCookiesDisabled, value);

}

void SiteEntry :: SetFavIcon(BBitmap * bmp) {

	if (bmp) {
		BBitmap * favIcon = GetFavIcon();
		if (!favIcon) {
			favIcon = new BBitmap(BRect(0, 0, 15, 15), B_RGB32);
			set(kFavIcon, favIcon);
		}
		memcpy(favIcon->Bits(), bmp->Bits(), 1024);
	}
}

void SiteEntry :: SetLoadingProgress(int loadingprogress) {

	set(kLoadingProgress, loadingprogress);

	if (loadingprogress == 100) {
		set(kStatusText, "Done.");
		set(kTitle, getString(kUrl));
	}
}

void SiteEntry :: SetSecureConnection(bool value) {

	set(kSecureConnection, value);

}

void SiteEntry :: SetTitle(const char * title) {

	set(kTitle, title);

}
