/*	Position implementation
	See Position.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// SGMLParser headers
#include "Position.hpp"
#include "PositionException.hpp"

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

unsigned int Position	::	nextPosition( const string & aText )	{

	if ( mIndex + 1 >= mEnd )	{
		mIndex--;
		throw PositionException();
	}

	if ( aText[ mIndex ] != '\n' )	{
		if ( aText[ mIndex ] != '\t' )	{
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
	
	return mIndex;

}

void Position	::	increaseSize()	{
	
	mEnd++;
	
}
