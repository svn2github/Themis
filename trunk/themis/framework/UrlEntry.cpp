/*
 * UrlEntry.cpp
 */

// Standard C headers
#include <stdio.h>

// BeOS headers
#include <String.h>

// Themis headers
#include "UrlEntry.h"

UrlEntry::UrlEntry(
	int32 id,
	const char* url )
{
	
	fID = id;
	fLoadingProgress = -1;
	
	fUrl = new BString( url );
	
	/*
	 * The page title is set to "loading..." now.
	 * When loading is finished, its set to the sites url.
	 * Then we got two options. Either I do grab the page title
	 * from the DOM tree, when the HTML parser is finished, or
	 * I wait for the renderer to finish, which then delivers me
	 * the page title.
	 */
	fTitle = new BString( "loading..." );
	
	fSecureConnection = false;
	
}

UrlEntry::~UrlEntry()
{
	
	if( fUrl != NULL )
		delete fUrl;
	if( fTitle != NULL )
		delete fTitle;
}

int32
UrlEntry::GetID()
{
	return fID;
}

int8
UrlEntry::GetLoadingProgress()
{
	return fLoadingProgress;
}

bool
UrlEntry::GetSecureConnection()
{
	return fSecureConnection;
}

const char*
UrlEntry::GetTitle()
{
	return fTitle ? fTitle->String() : "";
}

const char*
UrlEntry::GetUrl()
{
	return fUrl ? fUrl->String() : "";
}

void
UrlEntry::Print()
{
	printf( "  URLENTRY: ID[%ld] URL[%s] TITLE[%s]\n", fID, fUrl->String(), fTitle->String() );
	printf( "            LoadingProgess[%d] SecureConnection[%s]\n",
		fLoadingProgress,
		fSecureConnection ? "true" : "false" );

}

void
UrlEntry::SetLoadingProgress(
	int8 loadingprogress )
{
	fLoadingProgress = loadingprogress;
	
	if( fLoadingProgress == 100 )
	{
		fTitle->SetTo( fUrl->String() );
	}
}

void
UrlEntry::SetSecureConnection(
	bool value )
{
	fSecureConnection = value;
}

void
UrlEntry::SetTitle(
	const char* title )
{
	fTitle->SetTo( title );
}
