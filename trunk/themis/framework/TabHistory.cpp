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

#include <ListItem.h>

#include <stdio.h>

#include "TabHistory.h"

TabHistory::TabHistory(
	int8 depth )
{
	fMaxDepth = depth;
	fCurrentPosition = 0;
	fList = new BList( fMaxDepth );
}

TabHistory::~TabHistory()
{
	while( fList->CountItems() > 0 )
	{
		BStringItem* item = ( BStringItem* )fList->ItemAt( 0 );
		if( item != NULL )
		{
			fList->RemoveItem( item );
			delete item;
		}
	}
	delete fList;
}

void
TabHistory::AddEntry(
	const char* entry )
{
//	printf( "TabHistory::AddEntry()\n" );
//	printf( "  new entry: %s\n", entry );
	BStringItem* newitem = new BStringItem( entry );
	
	fList->AddItem( newitem, 0 );
	
	while( fList->CountItems() > fMaxDepth )
	{
		BStringItem* item = ( BStringItem* )fList->ItemAt( fList->CountItems() - 1 );
//		printf( "  Removing item: %s\n", item->Text() );
		fList->RemoveItem( item );
		delete item;
		item = NULL;
	}
	
	if( fCurrentPosition >= fMaxDepth )
		fCurrentPosition = fMaxDepth - 1;
	
//	printf( "  CountItems(): %ld \n", fList->CountItems() );
//	PrintHistory();
}

void
TabHistory::Clear()
{
	printf( "TabHistory::Clear()\n" );
	
	while( fList->CountItems() > 0 )
	{
		BStringItem* item = ( BStringItem* )fList->ItemAt( 0 );
		if( item != NULL )
		{
			fList->RemoveItem( item );
			delete item;
		}
	}
}

int8
TabHistory::GetCurrentPosition()
{
	return fCurrentPosition;
}

int32
TabHistory::GetEntryCount()
{
	return fList->CountItems();
}

int8
TabHistory::GetMaxDepth()
{
	return fMaxDepth;
}

const char*
TabHistory::GetNextEntry()
{
//	printf( "TabHistory::GetNextEntry()\n" );
	if( fCurrentPosition > 0 )
	{
//		printf( "fCurrentPosition > 0 [ %d ]\n", fCurrentPosition );
		fCurrentPosition -= 1;
		if( fCurrentPosition < fList->CountItems() && fCurrentPosition >= 0 )
			return ( (BStringItem* )fList->ItemAt( fCurrentPosition ) )->Text();
	}
	else
	{
//		printf( "fCurrentPosition == 0\n" );
		return NULL;
	}
}

const char*
TabHistory::GetPreviousEntry()
{
//	printf( "TabHistory::GetPreviousEntry()\n" );
	if( fCurrentPosition < fMaxDepth )
	{
//		printf( "fCurrentPosition < fMaxDepth [ %d ]\n", fCurrentPosition );
		fCurrentPosition += 1;
		if( fCurrentPosition < fList->CountItems() )
			return ( (BStringItem* )fList->ItemAt( fCurrentPosition ) )->Text();
	}
	else
	{
//		printf( "fCurrentPosition == fDepth\n" );
		return NULL;
	}
}

void
TabHistory::PrintHistory()
{
	int32 count = fList->CountItems();
	for( int32 i = 0; i < count; i++ )
	{
		BStringItem* item = ( BStringItem* )fList->ItemAt( i );
		if( item != NULL )
			printf( "item %3ld: %s\n", i, item->Text() );
		else
			printf( "item %3ld: NULL\n", i );
	}
}

void
TabHistory::SetDepth(
	int8 newdepth )
{
//	printf( "TabHistory::SetDepth()\n" );
	
	if( newdepth < fMaxDepth )
	{
		// remove entries if needed to suit fMaxDepth
		fMaxDepth = newdepth;
		while( fList->CountItems() > fMaxDepth )
		{
			BStringItem* item = ( BStringItem* )fList->ItemAt( fList->CountItems() - 1 );
//			printf( "  Removing item: %s\n", item->Text() );
			fList->RemoveItem( item );
			delete item;
			item = NULL;
		}
		
		if( fCurrentPosition >= fMaxDepth )
			fCurrentPosition = fMaxDepth - 1;
	}
	else
		fMaxDepth = newdepth;
}
