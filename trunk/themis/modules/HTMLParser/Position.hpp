/*	Position
	Stores the position in the document

	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	25-03-2003
	
*/

#ifndef POSITION_HPP
#define POSITION_HPP

// Standard C++ headers
#include <string>

// Namespaces used
using namespace std;

class Position	{
	
	private:
		unsigned int mStart;
		unsigned int mEnd;
		unsigned int mIndex;
		unsigned int mLineNr;
		unsigned int mCharNr;
		
	public:
		Position( unsigned int aStart = 0, unsigned int aSize = 0,
					 unsigned aLineNr = 1, unsigned int aCharNr = 1 );
		~Position();
		unsigned int getIndex() const;
		unsigned int getLineNr() const;
		unsigned int getCharNr() const;
		unsigned int getSize() const;
		unsigned int nextPosition( const string & aText );
		void increaseSize();

};

#endif
