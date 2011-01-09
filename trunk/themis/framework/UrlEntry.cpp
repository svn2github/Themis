/*
 * UrlEntry.cpp
 */

// Standard C headers
#include <stdio.h>

// Standard C++ headers
#include <string>

// BeOS headers
#include <String.h>

// Themis headers
#include "UrlEntry.h"

// Namespaces used
using std::string;

// Constants declared
const string kUrl = "Url";
const string kTitle = "Title";
const string kSecureConnection = "SecureConnection";
const string kLoadingProgress = "LoadingProgress";

UrlEntry :: UrlEntry(int32 id, const char * url) : BaseEntry(id) {
	
	set(kLoadingProgress, -1);
	set(kUrl, url);
	
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
	
}

int UrlEntry :: GetLoadingProgress() {

	return getInteger(kLoadingProgress);

}

bool UrlEntry :: GetSecureConnection() {

	return getBoolean(kSecureConnection);

}

const char * UrlEntry :: GetTitle() {

	return getString(kTitle).c_str();

}

const char * UrlEntry :: GetUrl() {

	return getString(kUrl).c_str();

}

void UrlEntry :: Print() {

	printf( "  URLENTRY: ID[%ld] URL[%s] TITLE[%s]\n", getId(), getString(kUrl).c_str(), getString(kTitle).c_str());
	printf( "            LoadingProgess[%d] SecureConnection[%s]\n",
		getInteger(kLoadingProgress),
		getBoolean(kSecureConnection) ? "true" : "false" );

}

void UrlEntry :: SetLoadingProgress(int loadingprogress) {

	set(kLoadingProgress, loadingprogress);
	
	if (loadingprogress == 100) {
		set(kTitle, getString(kUrl));
	}

}

void UrlEntry :: SetSecureConnection(bool value) {

	set(kSecureConnection, value);

}

void UrlEntry :: SetTitle(const char* title) {

	set(kTitle, title);

}
