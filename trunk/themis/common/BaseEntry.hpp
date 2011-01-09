/*
	Copyright (c) 2011 Mark Hellegers. All Rights Reserved.
	
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
	
	Original Author: 	Mark Hellegers (mark@firedisk.net)
	Project Start Date: October 18, 2000
	Class Start Date: January 08, 2011
*/

/*	BaseEntry
	The basic class for every entry in the system
	
	Mark Hellegers (mark@firedisk.net)
	08-01-2011

*/

#ifndef BASEENTRY_HPP
#define BASEENTRY_HPP

// Standard C++ headers
#include <map>
#include <string>
#include <vector>

// BeOS headers
#include <be/support/SupportDefs.h>

// Namespaces used
using std::map;
using std::string;
using std::vector;

class BaseEntry {
	
	private:
		int32 fId;
	
		map<string, string> mStrings;
		map<string, bool> mBooleans;
		map<string, int> mIntegers;
		map<string, void *> mPointers;

	protected:
		vector<BaseEntry *> fChildEntries;
	
	public:
		BaseEntry(int32 aId);
		virtual ~BaseEntry();
		int32 getId() const;
		void addEntry(BaseEntry * aEntry);
		BaseEntry * getEntry(int32 aId);
		void set(const string aName, const char * aValue);
		void set(const string aName, const string aValue);
		void set(const string aName, const bool aValue);
		void set(const string aName, const int aValue);
		void set(const string aName, void * aValue);
		string getString(const string aName);
		bool getBoolean(const string aName);
		int getInteger(const string aName);
		void * getPointer(const string aName);
	
};

#endif
