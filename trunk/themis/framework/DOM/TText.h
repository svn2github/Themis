/*
	Copyright (c) 2002 Mark Hellegers. All Rights Reserved.
	
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
	
	Original Author: 	Mark Hellegers (mark@firedisk.net)
	Project Start Date: October 18, 2000
	Class Start Date: April 01, 2002
*/

#ifndef TTEXT_H
#define TTEXT_H

// DOM headers
#include "DOMSupport.h"
#include "TCharacterData.h"

/// Text implementation of the DOM core

/*
	The Text class is used to store text in the DOM tree.
	It relies on the CharacterData class for part of its' functionality.
	For an extensive explanation, see the DOM core pages at http://www.w3.org/DOM
*/

class TText	:	public TCharacterData	{
	
	private:
		// Not implemented variables. Are constructed on the fly
		// TDOMString wholeText;
	
	public:
		/// Constructor of the TText class.
		/**
			The constructor of the TText class. It passes all its' arguments on to the
			TCharacterData class.
			
			@param	aOwnerDocument	The owner document of the text.
			@param	aText	The text to set.
		*/
		TText( const TDocumentPtr aOwnerDocument, const TDOMString aText );
		
		/// Destructor of the TText class.
		/**
			The destructor of the TText class does nothing. Everything is cleaned up
			automatically.
		*/
		~TText();
		
		/// A function to check if the text contains white space.
		/**
			This function checks if the text contains so called "ignorable" white space.
			It returns true if it does and false otherwise.
		*/
		bool isWhiteSpaceInElement() const;
		
		/// A function to get the whole text.
		/**
			This function returns the whole text of all text nodes adjacent to this one in
			document order.
		*/
		TDOMString getWholeText();
		
		/// A function to split the text.
		/**
			The text of this node is split at the first supplied argument. The text up to the
			offset is set in this node and the rest is set in a new text node and returned.
			
			@param	aOffset	The offset at which to split the text.

			@exception	INDEX_SIZE_ERR	Thrown if the specified offset is greater than
								the number of characters in this character data node.

			@todo	@li Have to check if this element is readonly.
		*/
		TTextPtr splitText( const unsigned long aOffset );
		
		/// A function to replace the whole text.
		/**
			This function replaces the whole text with the value of the supplied argument.
			The whole text is defined as in the getWholeText function.
			
			@param	aContent	The content to replace the whole text with.

			@todo	@li Have to check if this element is readonly.
						@li Have to check if the function is doing the right thing. The description of
						the DOM core sounds complicated and I haven't checked if my function
						gives the right result and does the necessary housekeeping.
		*/
		TTextPtr replaceWholeText( const TDOMString aContent );
		
};

#endif
