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

/*	ReadException implementation
	See ReadException.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// ReadException headers
#include "ReadException.hpp"

ReadException	::	ReadException( unsigned int aLineNr, unsigned int aCharNr,
												   string aErrorMessage, ExceptionReason aReason,
												   bool aFatal )	{

	mLineNr = aLineNr;
	mCharNr = aCharNr;
	mErrorMessage = aErrorMessage;
	mReason = aReason;
	mFatal = aFatal;

}

ReadException	::	~ReadException()	{
	
}

unsigned int ReadException	::	getCharNr() const	{

	return mCharNr;
	
}

unsigned int ReadException	::	getLineNr() const	{
	
	return mLineNr;
	
}

string ReadException	::	getErrorMessage() const	{
	
	return mErrorMessage;
	
}

ExceptionReason ReadException	::	getReason() const	{
	
	return mReason;
	
}

void ReadException	::	setReason( ExceptionReason aReason )	{
	
	mReason = aReason;
	
}

bool ReadException	::	isFatal() const	{
	
	return mFatal;
	
}

bool ReadException	::	isEof() const	{
	
	if ( mReason == END_OF_FILE_REACHED )	{
		return true;
	}
	
	return false;
	
}

bool ReadException	::	isWrongTag() const	{
	
	if ( mReason == WRONG_TAG_FOUND )	{
		return true;
	}
	
	return false;
	
}

void ReadException	::	setWrongTag( string aWrongTag )	{
	
	mWrongTag = aWrongTag;
	
}

string ReadException	::	getWrongTag() const	{
	
	return mWrongTag;
	
}

bool ReadException	::	isEndTag() const	{

	if ( mReason == END_TAG_FOUND )	{
		return true;
	}
	
	return false;	
	
}

void ReadException	::	setFatal( bool aFatal )	{
	
	mFatal = aFatal;
	
}
