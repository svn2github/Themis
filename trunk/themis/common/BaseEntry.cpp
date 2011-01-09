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

/*	BaseEntry implementation
	See BaseEntry.hpp for more information

*/

// Themis headers
#include "BaseEntry.hpp"

BaseEntry :: BaseEntry(int32 aId) {

	fId = aId;

}

BaseEntry :: ~BaseEntry() {

	unsigned int size = fChildEntries.size();
	for (unsigned int i = 0; i < size; i++) {
		delete fChildEntries[i];
	}

}

int32 BaseEntry :: getId() const {
	
	return fId;

}

void BaseEntry :: addEntry(BaseEntry * aEntry) {
	
	fChildEntries.push_back(aEntry);
	
}

BaseEntry * BaseEntry :: getEntry(int32 aId) {

	BaseEntry * result = NULL;
	
	// browse through the entry list to find the UrlEntry with the matching id
	vector<BaseEntry *>::iterator it = fChildEntries.begin();
	while (it != fChildEntries.end() && result == NULL) {
		if (((BaseEntry *)*it)->getId() == aId) {
			result = *it;
		}
		else {
			it++;
		}
	}

	return result;
}

void BaseEntry :: set(const string aName, const char * aValue) {
	
	if (mStrings.count(aName) == 0) {
		mStrings.insert(
			map<string, string>::value_type(aName, aValue));
	}
	else {
		mStrings[aName] = aValue;
	};
}

void BaseEntry :: set(const string aName, const string aValue) {
	
	if (mStrings.count(aName) == 0) {
		mStrings.insert(
			map<string, string>::value_type(aName, aValue));
	}
	else {
		mStrings[aName] = aValue;
	};
}

void BaseEntry :: set(const string aName, const bool aValue) {
	
	if (mBooleans.count(aName) == 0) {
		mBooleans.insert(
			map<string, bool>::value_type(aName, aValue));
	}
	else {
		mBooleans[aName] = aValue;
	};
}

void BaseEntry :: set(const string aName, const int aValue) {
	
	if (mIntegers.count(aName) == 0) {
		mIntegers.insert(
			map<string, int>::value_type(aName, aValue));
	}
	else {
		mIntegers[aName] = aValue;
	};
}

void BaseEntry :: set(const string aName, void * aValue) {
	
	if (mPointers.count(aName) == 0) {
		mPointers.insert(
			map<string, void *>::value_type(aName, aValue));
	}
	else {
		mPointers[aName] = aValue;
	};
}

string BaseEntry :: getString(const string aName) {
	
	return mStrings[aName];
}

bool BaseEntry :: getBoolean(const string aName) {
	
	return mBooleans[aName];
}

int BaseEntry :: getInteger(const string aName) {
	
	return mIntegers[aName];
}

void * BaseEntry :: getPointer(const string aName) {
	
	return mPointers[aName];
}
