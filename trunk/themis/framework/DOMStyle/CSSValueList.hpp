/*
	Copyright (c) 2004 Mark Hellegers. All Rights Reserved.
	
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
	Class Start Date: March 06, 2004
*/

#ifndef CSSVALUELIST_HPP
#define CSSVALUELIST_HPP

// Boost headers
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

// DOM Style headers
#include "CSSValue.hpp"

/// CSSValueList implementation of the DOM CSS.

/**
	The CSSValueList class stores a collection of css values.
	For an extensive explanation, see the DOM CSS pages at
	http://www.w3.org/TR/2000/REC-DOM-Level-2-Style-20001113/css.html
*/

class CSSValueList	:	public CSSValue	{
	
	public:
		/// Constructor of the CSSValueList class.
		/**
			The constructor of the CSSValueList class.
			It stores a list of css values and their type. Only the values can be
			changed after construction.
			
			@param	aCssText			The css text of the values to store.
			@param	aCssValueType	The type of the values to store.
		*/
		CSSValue( const TDOMString aCssText, unsigned short aCssValueType );
		
		/// Destructor of the CSSValuList class.
		/**
			The destructor of the CSSValueList class does nothing. Everything is cleaned up
			automatically.
		*/
		~CSSValueList();
		
		/// A function to get the number of css values in the list.
		unsigned long getLength();
		
		/// A function to get an item.
		/**
			This function gets the item that is indexed at the value of the supplied argument.
			If the index is greater than the number of items in the map, a null item is returned.

			@param	aIndex	The index at which to get the item.
		*/
		CSSValuePtr item( unsigned long aIndex );
		
};

#endif
