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
#include "PositionException.hpp"

SGMLText	::	SGMLText( const string & aText )	{
	
	mText = aText;
	
	reset();
	
}

SGMLText	::	~SGMLText()	{
	
}

void SGMLText	::	reset( bool clearText )	{

	if ( clearText )	{
		mText.erase();
	}

	mPositions.clear();
	
	Position textInfo( 0, mText.size(), 1, 1 );
	mPositions.push_back( textInfo );
	
}

void SGMLText	::	loadText( const char * aDocument )	{
	
	mText.erase();

	// Load text
	if ( aDocument != NULL )	{
		ifstream file( aDocument );
	
		char ch;
		while ( file.get( ch ) )	{
			// Check for \r. Had something to do with dos line endings, I think.
			if ( ch == '\r' )	{
				mText += '\n';
			}
			else	{
				mText += ch;
			}
		};
	}

	reset();
	
}

char SGMLText	::	nextChar()	{

	Position & current = mPositions.back();
	
	char c = mText[ current.getIndex() ];
	
	try	{
		current.nextPosition( c );
	}
	catch( PositionException p )	{
		// End of piece of text reached
		//printf( "Text popped\n" );
		mPositions.pop_back();
		if ( mPositions.size() == 0 )	{
			// End of total text reached
			throw ReadException( current.getLineNr(), current.getCharNr(),
											"End of text reached", END_OF_FILE_REACHED, true );
		}
		return getChar();
	}

	return mText[ current.getIndex() ];

}

char SGMLText	::	getChar()	{

	// WARNING: Might be dodgy if a zero length entity was just pushed on the stack
	
	Position & current = mPositions.back();
	
	return mText[ current.getIndex() ];

}

void SGMLText	::	addEntity( const Position & aEntity )	{
	
	//printf( "Adding entity: index %i, lineNr %i, charNr %i\n", aEntity.getIndex(), aEntity.getLineNr(), aEntity.getCharNr() );
	
	mPositions.push_back( aEntity );

	//printf( "First character of entity: %c\n", getChar() );
	
}

State SGMLText	::	saveState() const	{
	
	return State( mPositions );
	
}

void SGMLText	::	restoreState( const State & aState )	{
	
	mPositions = aState.getPositions();
	
}

unsigned int SGMLText	::	getLineNr() const	{

	const Position & current = mPositions.back();
		
	return current.getLineNr();
	
}

unsigned int SGMLText	::	getCharNr() const	{
	
	const Position & current = mPositions.back();
		
	return current.getCharNr();
	
}

unsigned int SGMLText	::	getIndex() const	{
	
	const Position & current = mPositions.back();
		
	return current.getIndex();
	
}

unsigned int SGMLText	::	getSize() const	{
	
	return mText.size();
	
}
