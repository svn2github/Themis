/*	ReadException implementation
	See ReadException.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// ReadException headers
#include "ReadException.hpp"

ReadException	::	ReadException( unsigned int aLineNr, unsigned int aCharNr,
												   string aErrorMessage, bool aFatal, bool aEof )	{
		
	mLineNr = aLineNr;
	mCharNr = aCharNr;
	mErrorMessage = aErrorMessage;
	mFatal = aFatal;
	mEof = aEof;
		
}

ReadException	::	~ReadException()	{
	
}

unsigned int ReadException	::	getCharNr()	{

	return mCharNr;
	
}

unsigned int ReadException	::	getLineNr()	{
	
	return mLineNr;
	
}

string ReadException	::	getErrorMessage()	{
	
	return mErrorMessage;
	
}

bool ReadException	::	isFatal()	{
	
	return mFatal;
	
}

bool ReadException	::	isEof()	{
	
	return mEof;
	
}

void ReadException	::	setFatal( bool aFatal )	{
	
	mFatal = aFatal;
	
}
