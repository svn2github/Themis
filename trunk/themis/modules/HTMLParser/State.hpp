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
	Class Start Date: April 04, 2003
*/

#ifndef STATE_HPP
#define STATE_HPP

// Standard C++ headers
#include <vector>

// SGMLParser headers
#include "Position.hpp"

// Namespaces used
using std::vector;

/// Stores the state of a document.

/**
	This class stores the state of a document. A state is a set of positions
	which are collected by a parser as it jumps to different parts
	of the document. It is used to keep track of where in the document
	the parser is and which way it has to go back.
	
	@todo @li Have to figure out how to return references in functions.
			   That should make life a bit easier.
*/

class State	{
	
	private:
		/// List of positions stored so far.
		/**
			This variable stores the positions of the current state.
			It only needs stack behaviour, but the stack class is too limited
			to use here. There is no clear function, which is necessary.
		*/
		vector<Position> mPositions;
		
	public:
		/// Constructor of the State class.
		/**
			The constructor of the State class.
			It does nothing.
			
		*/
		State();
		/// Destructor of the State class.
		/**
			The destructor of the State class does nothing.
			Everything is cleaned up automatically.
		*/
		~State();
		/// Add a position to the state.
		/**
			Adds a new position to the state. It is added at the back,
			so top and pop calls target this new state.
			
			@param	aPosition	The position to add.
		*/		
		void add( const Position & aPosition );
		/// Get the top of the State, when you won't change anything.
		const Position & top() const;
		/// Get the top of the State, when you want to change something.
		Position & top();
		/// Remove the last position.
		void pop();
		/// Resets the State by removing all the positions.
		void reset();
		/// Gives the number of positions in the State.
		unsigned int size() const;

};

#endif
