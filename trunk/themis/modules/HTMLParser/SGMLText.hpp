/*	SGMLText
	Stores a piece of raw text from an entity

	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	11-04-2003
*/

#ifndef SGMLTEXT_HPP
#define SGMLTEXT_HPP

// Standard C++ headers
#include <string>
#include <vector>

// SGMLParser headers
#include "Position.hpp"
#include "State.hpp"

using namespace std;

class SGMLText	{
	
	private:
		string mText;
		vector<Position> mPositions;
		
	public:
		SGMLText( const string & aText = "" );
		~SGMLText();
		void reset( bool clearText = false );
		void addChar( char aChar );
		char nextChar();
		char getChar();
		void addEntity( const Position & aEntity );
		State saveState() const;
		void restoreState( const State & aState );
		unsigned int getIndex() const;
		unsigned int getLineNr() const;
		unsigned int getCharNr() const;
		unsigned int getSize() const;

};

#endif
