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
	Class Start Date: Februari 14, 2004
*/

#ifndef STYLESHEETLIST_HPP
#define STYLESHEETLIST_HPP

// Standard C++ headers
#include <vector>

// Boost headers
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

// DOM Style headers
#include "StyleSheet.hpp"

// Declarations used
class StyleSheetList;

// Typedefs used
typedef boost::weak_ptr<StyleSheetList> StyleSheetListWeak;
typedef boost::shared_ptr<StyleSheetList> StyleSheetListPtr;

/// StyleSheetList implementation of the DOM Style Sheets.

/**
	The StyleSheetList class stores a collection of stylesheets.
	For an extensive explanation, see the DOM Style Sheets pages at
	http://www.w3.org/TR/2000/REC-DOM-Level-2-Style-20001113/stylesheets.html
*/

class StyleSheetList	{
	
	public:
		/// Constructor of the StyleSheetList class.
		/**
			The constructor of the StyleSheetList class. It stores the pointer to
			the stylesheet list.
			
			@param	aStyleSheetList	The list of stylesheets to store.
		*/
		StyleSheetList( const vector<StyleSheetPtr> * aStyleSheetList );
		
		/// Destructor of the StyleSheetList class.
		/**
			The destructor of the StyleSheetList class does nothing. Everything is cleaned up
			automatically.
		*/
		~StyleSheetList();
		
		/// A function to get the number of stylesheets in the list.
		unsigned long getLength();
		
		/// A function to get an item.
		/**
			This function gets the item that is indexed at the value of the supplied argument.
			If the index is greater than the number of items in the map, a null item is returned.

			@param	aIndex	The index at which to get the item.
		*/
		StyleSheetPtr item( unsigned long aIndex );
		
};

#endif
