/*	ReadException implementation
	See ReadException.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// ReadException headers
#include "ReadException.hpp"

ReadException	::	ReadException( unsigned int aLineNr, unsigned int aCharNr,
												   string aErrorMessage, bool aFatal, bool aEof,
												   bool aWrongTag )	{
		
	mLineNr = aLineNr;
	mCharNr = aCharNr;
	mErrorMessage = aErrorMessage;
	mFatal = aFatal;
	mEof = aEof;
	mWrongTag = aWrongTag;
		
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

bool ReadException	::	isFatal() const	{
	
	return mFatal;
	
}

bool ReadException	::	isEof() const	{
	
	return mEof;
	
}

bool ReadException	::	isWrongTag() const	{
	
	return mWrongTag;
	
}

void ReadException	::	setFatal( bool aFatal )	{
	
	mFatal = aFatal;
	
}
