/*	SGMLText implementation
	See SGMLText.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// SGMLParser headers
#include "SGMLText.hpp"
#include "ReadException.hpp"
#include "PositionException.hpp"

SGMLText	::	SGMLText( const string & aText )	{
	
	mText = aText;
	
	Position textInfo( 0, mText.size(), 1, 1 );
	mPositions.push_back( textInfo );
	
}

SGMLText	::	~SGMLText()	{
	
}

void SGMLText	::	addChar( char aChar )	{
	
	mText += aChar;

	Position & textInfo = mPositions[ 0 ];
	textInfo.increaseSize();

}

char SGMLText	::	nextChar()	{

	Position & current = mPositions.back();
	
	try	{
		current.nextPosition( mText );
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
