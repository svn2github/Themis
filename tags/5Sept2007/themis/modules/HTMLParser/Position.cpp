/*
	Copyright (c) 2003 Mark Hellegers. All Rights Reserved.
	
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
	
	Original Author: 	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	Project Start Date: October 18, 2000
	Class Start Date: March 25, 2003
*/

/*	Position implementation
	See Position.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// SGMLParser headers
#include "Position.hpp"

Position	::	Position( unsigned int aStart, unsigned int aSize,
							 unsigned int aLineNr, unsigned int aCharNr )	{
	
	mStart = aStart;
	mEnd = aStart + aSize;
	mIndex = aStart;
	mLineNr = aLineNr;
	mCharNr = aCharNr;
	
}

Position	::	~Position()	{
	
}

unsigned int Position	::	getIndex() const	{
	
	return mIndex;
	
}

unsigned int Position	::	getLineNr()	const	{
	
	return mLineNr;
	
}

unsigned int Position	::	getCharNr() const	{
	
	return mCharNr;
	
}

unsigned int Position	::	getSize() const	{
	
	return mEnd - mStart;
	
}

bool Position	::	nextPosition( const char aChar )	{

	if ( mIndex + 1 >= mEnd )	{
		mIndex--;
		return false;
	}

	if ( aChar != '\n' )	{
		if ( aChar != '\t' )	{
			mCharNr++;
		}
		else	{
			// I'm counting 4 characters for a tab
			mCharNr += 4;
		}
	}
	else	{
		mLineNr++;
		mCharNr = 1;
	}
	mIndex++;
	
	return true;

}
