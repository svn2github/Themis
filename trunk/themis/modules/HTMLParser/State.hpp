/*	State
	Stores the state of the document

	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	07-04-2003
	
*/

#ifndef STATE_HPP
#define STATE_HPP

// Standard C++ headers
#include <vector>

// SGMLParser headers
#include "Position.hpp"

// Namespaces used
using namespace std;

class State	{
	
	private:
		vector<Position> mPositions;
		
	public:
		State( const vector<Position> & aPositions );
		~State();
		vector<Position> getPositions() const;

};

#endif
