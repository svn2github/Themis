/*
	Copyright (c) 2004 Mark Hellegers. All Rights Reserved.
	
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
	Class Implementation Start Date: March 14, 2004
*/

/*
	MediaList implementation.
	See MediaList.hpp for more information.
*/

// Standard C headers
#include <stdio.h>

// DOM headers
#include "TDOMException.h"

// DOM Style headers
#include "MediaList.hpp"

MediaList :: MediaList(const TDOMString aMediaText) {

	printf("Creating MediaList\n");

	setMediaText(aMediaText);

}

MediaList :: ~MediaList() {
	
}

TDOMString MediaList :: getMediaText() const {

	TDOMString result = "";
	unsigned int length = mMediaList.size();
	for (unsigned int i = 0; i < length; i++) {
		result += mMediaList[i] + '+';
	}

	if (result.length() > 0) {
		// There is at least one medium and the last character is a ','
		// and needs to be removed.
		result.erase(result.length() - 1);
	}

	return result;
}

void MediaList :: setMediaText(const TDOMString aMediaText) {

	// Clear the existing values first.
	mMediaList.clear();
	
	// Parse the string and add the items to the list.
	unsigned int length = aMediaText.length();
	TDOMString medium = "";
	char c = '\0';
	for (unsigned int i = 0; i < length; i++) {
		c = aMediaText[i];
		if (!(isspace(c) || iscntrl(c))) {
			if (c == ',' && medium.length() > 0) {
				// End of a medium and it contains text.
				mMediaList.push_back(medium);
				medium = "";
			}
			else {
				// Not at the end of the medium text yet.
				medium += c;
			}
		}
	}
	
	// See if there is still a medium in the buffer and if so add it.
	if (medium.length() > 0) {
		mMediaList.push_back(medium);
	}

}

unsigned long MediaList :: getLength() const {

	return mMediaList.size();
}

TDOMString MediaList :: item(unsigned long aIndex) {

	if ( aIndex < mMediaList.size() )	{
		return mMediaList[ aIndex ];
	}
	
	return "";
	
}

void MediaList :: deleteMedium(const TDOMString aMedium) {

	vector<TDOMString>::iterator iter = find(mMediaList.begin(), mMediaList.end(), aMedium);
	if (iter != mMediaList.end()) {
		// Remove node
		mMediaList.erase(iter);
	}
	else {
		// Not found. Notify the user.
		throw TDOMException(NOT_FOUND_ERR);
	}

}

void MediaList :: appendMedium(const TDOMString aMedium) {

	// Need to remove it first, but don't want to incur an exception if it
	// wasn't in the list before, which is most likely.
	vector<TDOMString>::iterator iter = find(mMediaList.begin(), mMediaList.end(), aMedium);
	if (iter != mMediaList.end()) {
		deleteMedium(aMedium);
	}
	
	mMediaList.push_back(aMedium);
	
}
