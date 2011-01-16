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
const string kStatusText = "StatusText";
const string kCookiesDisabled = "CookiesDisabled";
const string kFavIcon = "FavIcon";

SiteEntry :: SiteEntry(int32 id,
					   const char * url)
		  : UrlEntry(id, url) {

	string statusText = "Transfering data from ";
	statusText += url;
	statusText += " ...";
	set(kStatusText, statusText);
	
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

const char * SiteEntry :: GetStatusText() {

	return getString(kStatusText).c_str();

}

void SiteEntry :: Print() {

	printf("------------------------------------\n");
	printf("SiteEntry: ID[%ld] URL[%s] TITLE[%s]\n", getId(), GetUrl(), GetTitle());
	printf("           LoadingProgess[%d] CookiesDisabled[%s], SecureConnection[%s]\n",
		GetLoadingProgress(),
		getBoolean(kCookiesDisabled) ? "true" : "false",
		GetSecureConnection() ? "true" : "false");

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

	UrlEntry::SetLoadingProgress(loadingprogress);

	if (loadingprogress == 100) {
		set(kStatusText, "Done.");
	}
}

void SiteEntry :: SetStatusText(const char * text) {
	
	set(kStatusText, text);
	
}
