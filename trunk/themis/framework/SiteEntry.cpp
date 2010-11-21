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

SiteEntry::SiteEntry(
	int32 id,
	const char* url )
{
	printf( "SiteEntry::SiteEntry()\n" );
	
	fID = id;
	fLoadingProgress = -1;
	
	fUrl = new BString( url );
	
	fStatusText = new BString( "Transfering data from " );
	fStatusText->Append( url );
	fStatusText->Append( " ..." );
	
	/*
	 * The page title is set to "loading..." now.
	 * When loading is finished, its set to the sites url.
	 * Then we got two options. Either I do grab the page title
	 * from the DOM tree, when the HTML parser is finished, or
	 * I wait for the renderer to finish, which then delivers me
	 * the page title.
	 */
	fTitle = new BString( "loading..." );
	
	fCookiesDisabled = false;
	fSecureConnection = false;
	
	fFavIcon = NULL;
}

SiteEntry::~SiteEntry()
{
	printf( "SiteEntry::~SiteEntry()\n" );
	
	if( fUrl != NULL )
		delete fUrl;
	if( fStatusText != NULL )
		delete fStatusText;
	if( fTitle != NULL )
		delete fTitle;
}

void
SiteEntry::AddEntry(
	int32 id,
	const char* url )
{
	printf( "SiteEntry::AddEntry()\n" );
	
	printf( "SiteEntry: adding following item: ID[%ld] URL[%s]\n", id, url );
	
	UrlEntry* entry = new UrlEntry(
		id,
		url );
	
	fEntryList.push_back( entry );
}

bool
SiteEntry::GetCookiesDisabled()
{
	return fCookiesDisabled;
}

BBitmap*
SiteEntry::GetFavIcon()
{
	return fFavIcon;
}

UrlEntry*
SiteEntry::GetEntry(
	int32 id )
{
//	fLocker->Lock();
	
	printf( "SiteEntry::GetEntry( %ld )\n", id );
	
	UrlEntry* entry = NULL;
	
	// browse through the entry list to find the UrlEntry with the matching id
	
	vector< UrlEntry* >::iterator it;
	for( it = fEntryList.begin(); it != fEntryList.end(); it++ )
	{
		if( ( ( UrlEntry* )*it )->GetID() == id )
		{
			entry = *it;
		}
	}

//	fLocker->Unlock();
	return entry;
}

int32
SiteEntry::GetID()
{
	return fID;
}

int8
SiteEntry::GetLoadingProgress()
{
	printf( "SiteEntry::GetLoadingProgress()\n" );
		
	/* cycle through the list of UrlEntries and calculate the loading progress */
	
	uint32 progress = 0;
	
	vector< UrlEntry* >::iterator it;
	for( it = fEntryList.begin(); it != fEntryList.end(); it++ )
	{
		progress += ( ( UrlEntry* )*it )->GetLoadingProgress();
	}
	
	progress = ( uint32 )( progress / fEntryList.size() );

	SetLoadingProgress( ( uint8 )progress );
	
	printf( "SITEENTRY: fLoadingProgress: %d\n", fLoadingProgress );
	
	return fLoadingProgress;
}

bool
SiteEntry::GetSecureConnection()
{
	return fSecureConnection;
}

const char*
SiteEntry::GetStatusText()
{
	return fStatusText ? fStatusText->String() : "";
}

const char*
SiteEntry::GetTitle()
{
	printf( "  GetTitle(): %s\n", fTitle->String() );
	return fTitle ? fTitle->String() : "";
}

const char*
SiteEntry::GetUrl()
{
	return fUrl ? fUrl->String() : "";
}

void
SiteEntry::Print()
{
	printf( "------------------------------------\n" );
	printf( "SiteEntry: ID[%ld] URL[%s] TITLE[%s]\n", fID, fUrl->String(), fTitle->String() );
	printf( "           LoadingProgess[%d] CookiesDisabled[%s], SecureConnection[%s]\n",
		fLoadingProgress,
		fCookiesDisabled ? "true" : "false",
		fSecureConnection ? "true" : "false" );
	
	printf( "  -- SiteEntrys UrlEntries --\n" );
	
	vector< UrlEntry* >::iterator it;
	for( it = fEntryList.begin(); it != fEntryList.end(); it++ )
	{
		( ( UrlEntry* )*it )->Print();
	}
	
	printf( "------------------------------------\n" );
}

void
SiteEntry::SetCookiesDisabled(
	bool value )
{
	fCookiesDisabled = value;
}

void
SiteEntry::SetFavIcon(
	BBitmap* bmp )
{
	if( bmp )
	{
		if( !fFavIcon )
			fFavIcon = new BBitmap(
				BRect( 0, 0, 15, 15 ), B_RGB32 );
		
		memcpy( fFavIcon->Bits(), bmp->Bits(), 1024 );
	}
}

void
SiteEntry::SetLoadingProgress(
	int8 loadingprogress )
{
	fLoadingProgress = loadingprogress;
	
	if( fLoadingProgress == 100 )
	{
		fStatusText->SetTo( "Done." );
		fTitle->SetTo( fUrl->String() );
	}
}

void
SiteEntry::SetSecureConnection(
	bool value )
{
	fSecureConnection = value;
}

void
SiteEntry::SetTitle(
	const char* title )
{
	fTitle->SetTo( title );
}
