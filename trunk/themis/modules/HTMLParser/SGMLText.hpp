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

/*	SGMLText
	Stores a piece of raw text from an entity

	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	11-04-2003
*/

#ifndef SGMLTEXT_HPP
#define SGMLTEXT_HPP

// Standard C++ headers
#include <string>

// SGMLParser headers
#include "Position.hpp"
#include "State.hpp"

// Namespaces used
using std::string;

/// Class to store an SGML text.

/**
	This class stores an SGML text.
	It does this by providing functions and
	variables to make parsing it easier.
*/

class SGMLText	{
	
	private:
		/// The full text of the SGML document.
		string mText;
		/// The positions it has currently encountered.
		/**
			Might be better to store it in just one place: The state.
		*/
		State mState;
		
	public:
		/// Constructor of the SGMLText class.
		/**
			The constructor of the SGMLText class. It stores the text
			and resets all variables to their default values.
			
			@param	aText	The text of the SGML document.
		*/
		SGMLText( const string & aText = "" );
		/// Destructor of the SGMLText class.
		/**
			The destructor of the SGMLText class does nothing.
			Everything is cleaned up automatically.
		*/
		~SGMLText();
		/// A funtion to reset the variables to the default state.
		/**
			This function resets the variables to their default state.
			It is possible to leave the text from being cleared.
			
			@param	clearText	Determines if the text will be cleared or not.
		*/
		void reset( bool clearText = false );
		/// A function to load a document.
		/**
			This function loads a document replacing the old
			one and resetting all variables.
			
			@param	aDocument	The document to load.
		*/
		void loadText( const char * aDocument );
		/// A function to go to the next character.
		const char nextChar();
		/// A function to get the current character.
		const char getChar();
		/// A function to add an entity.
		/**
			This function adds an entity position to the text.
			
			@param	aEntity	The entity position to add.
			
			@todo	Function seems to have an odd name.
		*/
		void addEntity( const Position & aEntity );
		/// A function to save the current state of the text.
		State saveState() const;
		/// A function to restore the state.
		/**
			This function restores the state of the text to the state
			given. This is ususally done if parsing went in the wrong
			direction and it has to start again from the last good position.
			
			@param	aState	The state to restore to.
		*/
		void restoreState( const State & aState );
		/// A function to get the index.
		unsigned int getIndex() const;
		/// A function to get the line number.
		unsigned int getLineNr() const;
		/// A function to get the character number.
		unsigned int getCharNr() const;
		/// A function to get the size of the text.
		unsigned int getSize() const;

};

#endif
