/*	State implementation
	See State.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// SGMLParser headers
#include "State.hpp"

State	::	State( const vector<Position> & aPositions )	{
	
	mPositions = aPositions;
	
}

State	::	~State()	{
	
}

vector<Position> State	::	getPositions()	const	{
	
	return mPositions;
	
}
