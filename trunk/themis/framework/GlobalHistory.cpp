/*
Copyright (c) 2003 Z3R0 One. All Rights Reserved.

Permission is hereby granted, free of charge, to any person 
obtaining a copy of this software and associated documentation 
files (the "Software"), to deal in the Software without 
restriction, including without limitation the rights to use, 
copy, modify, merge, publish, distribute, sublicense, and/or 
sell copies of the Software, and to permit persons to whom 
the Software is furnished to do so, subject to the following 
conditions: 

   The above copyright notice and this permission notice 
   shall be included in all copies or substantial portions 
   of the Software. 

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY 
KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS 
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR 
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Original Author & Project Manager: Z3R0 One (z3r0_one@bbnk.dhs.org)
Project Start Date: October 18, 2000
*/

#include <stdio.h>
#include <time.h>

#include "GlobalHistory.h"
#include "app.h"

GlobalHistory::GlobalHistory(
	int8 depth )
{
	fMaxDepthInDays = depth;
	fList = new BList( 0 );
}

GlobalHistory::~GlobalHistory()
{
	printf( "GlobalHistory::~GlobalHistory()\n" );
	printf( "  Putting GlobalHistoryData in AppSettings\n" );
	
	BMessage* collector = new BMessage;
		
	while( fList->CountItems() > 0 )
	{
		GlobalHistoryItem* item = ( GlobalHistoryItem* )fList->ItemAt( fList->CountItems() - 1 );
		if( item != NULL )
		{
			/* Pack up the item data in a BMessage, which will then be stored in the
			 * collector message.
			 */
			
			printf( "  Adding following item to collector message:\n" );
			item->Print();
			
			BMessage* collitem = new BMessage;
			collitem->AddString( "url", item->Text() );
			collitem->AddInt32( "time", item->Time() );
			
			collector->AddMessage( "GlobalHistoryItemMessage", collitem );
			delete collitem;
							
			fList->RemoveItem( item );
			delete item;
		}
	}
	delete fList;
	
//	collector->PrintToStream();
	
	if( AppSettings->HasMessage( "GlobalHistoryData" ) )
		AppSettings->ReplaceMessage(
			"GlobalHistoryData",
			collector );
	else
		AppSettings->AddMessage(
			"GlobalHistoryData",
			collector );
	
//	AppSettings->PrintToStream();
	
	delete collector;
}

void
GlobalHistory::AddEntry(
	const char* entry )
{
	printf( "GlobalHistoryItem::AddEntry()\n" );
	printf( "  New entry: %s\n", entry );
	GlobalHistoryItem* newitem = new GlobalHistoryItem( entry, time( NULL ) );
	
	/* We don't want to add an item twice for a day.
	 * ( This has nothing to do about similar entries on different days. )
	 */
	
	if( CheckDoubleDay( newitem ) == false )
	{
		fList->AddItem( newitem, 0 );
		CheckEntryExpiration();
	}
	else
		delete newitem;

	PrintHistory();
}

bool
GlobalHistory::CheckDoubleDay(
	GlobalHistoryItem* checkitem )
{
	printf( "GlobalHistory::CheckDoubleDay()\n" );

	const time_t checktime_t = checkitem->Time();	
	struct tm checkitem_tm;
	
	if( localtime_r( &checktime_t, &checkitem_tm ) == NULL )
	{
		printf( "  checkitem_tm == NULL. aborting.\n" );
		return true;
	}
	
	int32 count = fList->CountItems();
	
	for( int32 i = 0; i < count; i++ )
	{
		GlobalHistoryItem* item = ( GlobalHistoryItem* )fList->ItemAt( i );
		if( item != NULL )
		{
			const time_t itemtime_t = item->Time();
			struct tm item_tm;
						
			if( localtime_r( &itemtime_t, &item_tm ) != NULL )
			{
				/* note: If we find an item from the day before, we can stop the check.
				 * ( List is kept chronologically. Newest first. )
				 */
				
				if( item_tm.tm_year == checkitem_tm.tm_year )
				{
					if( item_tm.tm_yday == checkitem_tm.tm_yday )
					{
						if( strcmp( item->Text(), checkitem->Text() ) == 0 )
						{
							printf( "  URL already stored today. skipping.\n" );
							return true;
						}
					}
					else
					{
						printf( "  Found older (or newer) item [day]. break.\n" );
						break;
					}
				}
				else
				{
					printf( "  Found older (or newer) item [year]. break.\n" );
					break;
				}
			}
		}
	}
	
	printf( "  URL not yet stored today.\n" );
		
	return false;
}

void
GlobalHistory::CheckEntryExpiration()
{
	printf( "GlobalHistory::CheckEntryExpiration()\n" );
	
	int32 count = fList->CountItems();
	const time_t now = time( NULL );
	struct tm today_tm;
	localtime_r( &now, &today_tm );
			
	int8 depth = 0;
	AppSettings->FindInt8( "GlobalHistoryDepthInDays", &depth );
	
	printf( "  GlobalHistoryDepthInDays: %d\n", depth );
	printf( "  today_tm.tm_yday: %d\n", today_tm.tm_yday );
	
	/* Check the history list for expired entries from last to first item.
	 * If we find one, which is younger then limit, we can stop checking.
	 * ( Oldest item is stored at the end of the list. )
	 */
		
	for( int32 i = count - 1; i >= 0; i-- )
	{
		GlobalHistoryItem* item = ( GlobalHistoryItem* )fList->ItemAt( i );
		
		const time_t itemtime_t = item->Time();
		struct tm item_tm;
		localtime_r( &itemtime_t, &item_tm );
		
		int16 limit = 0;
		
		if( item_tm.tm_yday >= depth - 1 )
		{
			printf( "  Item within this year.\n" );
			limit = ( int16 )( today_tm.tm_yday - depth );
		}
		else
		{
			printf( "  Item from last year.\n" );
			limit = ( int16 )( 365 - ( depth - ( item_tm.tm_yday + 1 ) ) );
		}
		
		printf( "  limit: %d ( Entries <= this one are expired. )\n", limit ); 
		
		if( item_tm.tm_yday <= limit )
		{
			printf( "  The foloowing item is expired. (Will be removed now.):\n" );
			item->Print();
			fList->RemoveItem( item );
			delete item;
		}
		else
		{
			printf( "  Found in time item. break.\n" );
			break;
		}
	}
}

void
GlobalHistory::Clear()
{
	printf( "GlobalHistory::Clear()\n" );
	while( fList->CountItems() > 0 )
	{
		GlobalHistoryItem* item = ( GlobalHistoryItem* )fList->ItemAt( 0 );
		if( item != NULL )
		{
			fList->RemoveItem( item );
			delete item;
		}
	}	
}

BList*
GlobalHistory::GetStrippedList()
{
	printf( "GlobalHistory::GetStrippedList()\n" );

	BList* slist = new BList( 0 );
	
	int32 count = fList->CountItems();
	
	/* start adding from last to first, so that we just have to BList::AddItem()
	 * without index in Win::UrlTypedHandler(). This results in newer urls ( if
	 * they match ) to be at top of the list.
	 */
	
	for( int32 i = count - 1; i >= 0; i-- )
	{
//		printf( "  ==================\n" );
		
		GlobalHistoryItem* item = ( GlobalHistoryItem* )fList->ItemAt( i );
		if( item != NULL )
		{
//			printf( "  Checking for: %s\n", item->Text() );
			
			bool filed = false;
			int32 count2 = slist->CountItems();
			
			for( int32 j = 0; j	< count2; j++ )
			{
				GlobalHistoryItem* compareitem = ( GlobalHistoryItem* )slist->ItemAt( j );
				if( compareitem != NULL )
				{
					if( strcmp( item->Text(), compareitem->Text() ) == 0 )
					{
//						printf( "  Entry already filed in stripped list. break.\n" );
						filed = true;
						break;
					}
				}
			}
			if( filed == false )
			{
//				printf( "  Entry not filed. Adding.\n" );
				GlobalHistoryItem* newitem = new GlobalHistoryItem(
					item->Text(),
					item->Time() );
				slist->AddItem( newitem );
			}
		}
	}
	
	int32 scount = slist->CountItems();
	
	printf( "  ============================\n" );
	printf( "  Stripped List: ( %ld items )\n", scount );
	for( int32 k = 0; k < scount; k++ )
	{
		GlobalHistoryItem* item = ( GlobalHistoryItem* )slist->ItemAt( k );
		if( item != NULL )
			item->Print();
	}
	printf( "  ============================\n" );
	
	return slist;
}

void
GlobalHistory::Init(
	BMessage* datamsg )
{
	printf( "GlobalHistory::Init()\n" );
	
//	datamsg->PrintToStream();
	
	if( datamsg == NULL )
		return;
	
	type_code tc = B_ANY_TYPE;
	int32 count = -1;
	
	if( datamsg->GetInfo( "GlobalHistoryItemMessage", &tc, &count ) == B_OK )
	{
		printf( "  Found %ld GlobalHistoryItemMessages.\n", count );
		
		for( int32 i = 0; i < count; i++ )
		{
			BMessage* ghimsg = new BMessage;
			datamsg->FindMessage( "GlobalHistoryItemMessage", i, ghimsg );
			if( ghimsg != NULL )
			{
				GlobalHistoryItem* newitem = new GlobalHistoryItem(
					ghimsg->FindString( "url" ),
					ghimsg->FindInt32( "time" ) );
				printf( "  Adding the following item:\n", i );
				newitem->Print();
				fList->AddItem( newitem, 0 );
			}
		}
		
		CheckEntryExpiration();
	}
	else
	{
		printf( "  No GlobalHistoryItemMessages found.\n" );
	}
}

void
GlobalHistory::PrintHistory()
{
	printf( "GlobalHistory::PrintHistory()\n" );
	
	int32 count = fList->CountItems();
	
	printf( "  =============================\n" );
	printf( "  Global History: ( %ld items )\n", count );
	for( int32 i = 0; i < count; i++ )
	{
		GlobalHistoryItem* item = ( GlobalHistoryItem* )fList->ItemAt( i );
		if( item != NULL )
			item->Print();
	}
	printf( "  =============================\n" );
}

void
GlobalHistory::SetDepth(
	int8 newdepth )
{
//	printf( "GlobalHistory::SetDepth()\n" );
	
	if( newdepth < fMaxDepthInDays )
	{
		// remove entries if needed to suit fMaxDepth
		fMaxDepthInDays = newdepth;
		CheckEntryExpiration();
	}
	else
		fMaxDepthInDays = newdepth;
}


/* GLOBALHISTORYITEM */
 
GlobalHistoryItem::GlobalHistoryItem(
	const char* text,
	time_t time,
	uint32 level = 0,
	bool expanded = true )
	: BStringItem( text, level, expanded )
{
	fTime = time;
}

GlobalHistoryItem::~GlobalHistoryItem()
{
}

void
GlobalHistoryItem::Print()
{
//	printf( "GlobalHistoryItem::Print()\n" );
	printf( "  %s @ %s", Text(), asctime( localtime( &fTime ) ) );
}

//void
//GlobalHistoryItem::SetTime(
//	time_t newtime )
//{
//	printf( "GlobalHistoryItem::SetTime()\n" );
//	
//	fTime = newtime;
//}

time_t
GlobalHistoryItem::Time()
{
	return fTime;
}
