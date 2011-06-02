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

#ifndef _GLOBALHISTORY_H_
#define _GLOBALHISTORY_H_

#include <List.h>
#include <Message.h>

class GlobalHistoryItem;

class GlobalHistory {

	private:
		int8 fHistoryDepthInDays;
		int8 fFreeUrlCount;
		BList * fList;

		bool CheckDoubleDay(GlobalHistoryItem* checkitem);
		void CheckEntryExpiration();

	public:
		GlobalHistory(int8 depth,
					  int8 count);
		~GlobalHistory();
									
		void  AddEntry(const char* entry);
		void Clear();
		BList * GetFilteredList(BString & aText);
		void Init(BMessage* datamsg);
		void PrintHistory();
		void SetDepth(int8 newdepth);
		
};

#endif

#ifndef GLOBALHISTORYITEM
#define GLOBALHISTORYITEM

#include <ListItem.h>

class GlobalHistoryItem : public BStringItem {
	private:
		time_t fTime;

	public:
		GlobalHistoryItem(const char* text,
						  time_t time,
						  uint32 level = 0,
						  bool expaned = true);
		virtual ~GlobalHistoryItem();

		void Print();
		void SetFree();
		time_t Time();

};

#endif
