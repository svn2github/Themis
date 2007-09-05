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
	Class Start Date: March 25, 2003
*/

#ifndef POSITION_HPP
#define POSITION_HPP

/// Stores the position in the document.

/**
	The Position class stores the position of the parser in a document.
	It takes care of all things related to the position, including making
	sure it stays inside the document.
	It is possible to restrict the positions that can be accessed to allow
	access to only a subpart of a document.
*/

class Position	{
	
	private:
		/// The start of the piece of text to keep track of.
		unsigned int mStart;
		/// The end of the piece of text to keep track of.
		unsigned int mEnd;
		/// The current position in the document.
		/**
			The current position in the document from the beginning of the
			document. It can only move between mStart and mEnd.
		*/
		unsigned int mIndex;
		/// The current line number of the document.
		unsigned int mLineNr;
		/// The current character number of the document.
		unsigned int mCharNr;
		
	public:
		/// Constructor of the Position class.
		/**
			The constructor of the Position class. It sets up the necessary
			range, size and start values.
			
			@param	aStart	The start of the piece of text to keep track of.
			@param	aSize		The size of the piece of text to keep track of.
			@param	aLineNr	The line number the piece of text starts at.
			@param	aCharNr	The character number the piece of text starts at.
		*/
		Position( unsigned int aStart = 0, unsigned int aSize = 0,
					 unsigned aLineNr = 1, unsigned int aCharNr = 1 );
		/// Destructor of the Position class.
		/**
			The destructor of the Position class does nothing.
			Everything is cleaned up automatically.
		*/
		~Position();
		/// A function to get the index of the current position.
		unsigned int getIndex() const;
		/// A function to get the line number of the current position.
		unsigned int getLineNr() const;
		/// A function to get the character number of the current position.
		unsigned int getCharNr() const;
		/// A function to get the size of the piece of text to keep track of.
		unsigned int getSize() const;
		/// A function to go to the next position in the document.
		/**
			This function goes to the next position in the document.
			If this would take it past the end of the piece of text it keeps
			track of, it throws an exception. It keeps the line number and
			character number up to date.
			
			@param	aChar	The character by which to determine
										what the next position is.
			
			@exception	PositionException	Thrown if going to the next index
															will cause it to go past the end
															of the piece of text to keep
															track of.
			@todo	@li	The tab size is hard set to 4. Dunno if that is good.
						@li	Have a look at the body of the function. Hmm...
		*/
		bool nextPosition( const char aChar );

};

#endif
