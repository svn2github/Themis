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
