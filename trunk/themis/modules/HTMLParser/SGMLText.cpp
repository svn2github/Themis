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
	Class Start Date: April 11, 2003
*/

/*	SGMLText implementation
	See SGMLText.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// Standard C++ headers
#include <fstream>

// SGMLParser headers
#include "SGMLText.hpp"
#include "ReadException.hpp"

// Namespaces used
using std::ifstream;

SGMLText	::	SGMLText( const string & aText )	{
	
	loadText(aText);

}

SGMLText	::	~SGMLText()	{
	
}

void SGMLText	::	reset( bool clearText )	{

	if ( clearText )	{
		mText.erase();
	}

	mState.reset();
	
	Position textInfo( 0, mText.size(), 1, 1 );
	mState.add( textInfo );
	
}

void SGMLText	::	loadText( const char * aDocument )	{
	
	const int bufferSize = 1024;
	char buffer[bufferSize];
	
	mText.erase();

	// Load text
	if (aDocument != NULL)	{
		ifstream file(aDocument);
	
		while (file.get(buffer, bufferSize, '\0'))	{
			int count = file.gcount();
			if (count < bufferSize - 1) {
				file.ignore();
			}
			mText.append(buffer, count);
		};
	}

	reset();
	
}

void SGMLText :: loadText(string aText) {

	mText = aText;
	
	reset();
	
}

char SGMLText	::	nextChar()	{

	Position & current = mState.top();
	
	char c = mText[current.getIndex()];
	
	bool nextFound = current.nextPosition(c);
	
	if (!nextFound) {
		// End of piece of text reached
		mState.pop();
		if (mState.size() == 0) {
			// End of total text reached
			// Return the special character: 0
			c = '\0';
		}
		else {
			c = getChar();
		}
	}
	else {
		c = mText[current.getIndex()];
	}

	// Filter out CR/LF sequences.
	if (c == '\r') {
		State backup = saveState();
		c = nextChar();
		if (c != '\n') {
			restoreState(backup);
			c = '\r';
		}
	}

	return c;

}

char SGMLText	::	getChar()	{

	// WARNING: Might be dodgy if a zero length entity was just pushed on the stack
	
	const Position & current = mState.top();
	
	return mText[ current.getIndex() ];

}

void SGMLText	::	addEntity( const Position & aEntity )	{
	
	mState.add( aEntity );

}

State SGMLText	::	saveState() const	{
	
	return mState;
	
}

void SGMLText	::	restoreState( const State & aState )	{
	
	mState = aState;
	
}

unsigned int SGMLText	::	getLineNr() const	{

	const Position & current = mState.top();
		
	return current.getLineNr();
	
}

unsigned int SGMLText	::	getCharNr() const	{
	
	const Position & current = mState.top();
		
	return current.getCharNr();
	
}

unsigned int SGMLText	::	getIndex() const	{
	
	const Position & current = mState.top();
		
	return current.getIndex();
	
}

unsigned int SGMLText	::	getSize() const	{
	
	return mText.size();
	
}
