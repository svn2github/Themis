/*
	Copyright (c) 2012 Mark Hellegers. All Rights Reserved.
	
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
	Class Start Date: August 19, 2012
*/

#ifndef ABOUTITEM_HPP
#define ABOUTITEM_HPP

// BeOS headers
#include <be/interface/ListItem.h>
#include <be/support/String.h>

enum AboutType {
	APPLICATION_TYPE = 1,
	LIBRARY_TYPE,
	PLUGIN_TYPE
};

class AboutItem : public BListItem {

	private:
		BString mAboutText;
		AboutType mAboutType;
	
	public:
		AboutItem(const char * aAboutText, AboutType aAboutType);

		virtual void DrawItem(BView * aOwner,
							  BRect aFrame,
							  bool aComplete = false);
		const char * Text() const;
		AboutType Type() const;


};

#endif
