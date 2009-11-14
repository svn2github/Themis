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
	Class Start Date: June 29, 2002
*/

#ifndef TCOMMENT_H
#define TCOMMENT_H

// DOM headers
#include "DOMSupport.h"
#include "TCharacterData.h"

/// Comment implementation of the DOM core

/*
	The Comment class is used to store comments in the DOM tree.
	It completely relies on the CharacterData class for everything.
	For an extensive explanation, see the DOM core pages at http://www.w3.org/DOM
*/

class TComment	:	public TCharacterData	{
	
	public:
		/// Constructor of the TComment class.
		/**
			The constructor of the TComment class. It passes all arguments to the
			TCharacterData class and does nothing in itself. It only supplies the
			node type.
			
			@param	aOwnerDocument	The owner document of this comment.
			@param	aData	The character data of the comment.
		*/
		TComment( const TDocumentWeak aOwnerDocument, const TDOMString aData )	:
				TCharacterData( COMMENT_NODE, aOwnerDocument, aData )	{};

		/// Destructor of the TComment class.
		/**
			The destructor of the TComment class does nothing. Everything is cleaned up
			automatically.
		*/
		~TComment()	{};
		
};

#endif
