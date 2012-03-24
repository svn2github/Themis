/*
	Copyright (c) 2010 Mark Hellegers. All Rights Reserved.
	
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
	Class Start Date: April 10, 2010
*/

/*	TextLine
	Stores information about where to print what part of text
	
	Mark Hellegers (mark@firedisk.net)
	10-04-2010
	
*/

#ifndef TEXTBOX_HPP
#define TEXTBOX_HPP

// BeOS headers
#include <Rect.h>

class TextBox {
	
	private:
		unsigned int mStart;
		unsigned int mEnd;
		BRect mRect;
		float mPixelWidth;
		bool mStartWithSpace;
		bool mEndWithSpace;
		bool mEmptyBox;

	public:
		TextBox(unsigned int aStart, unsigned int aEnd, BRect aRect);
		TextBox(unsigned int aStart,
				unsigned int aEnd,
				float aPixelWidth,
				bool aStartWithSpace = false,
				bool aEndWithSpace = false,
				bool aEmptyBox = false);
		~TextBox();
		void getRange(unsigned int & aStart, unsigned int & aEnd);
		BRect getRect() const;
		void setRect(BRect aRect);
		float getPixelWidth() const;
		bool startsWithSpace() const;
		bool endsWithSpace() const;
		bool emptyBox() const;
};

#endif
