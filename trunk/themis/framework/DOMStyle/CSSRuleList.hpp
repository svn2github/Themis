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
	
	Original Author: 	Mark Hellegers (mark@firedisk.net)
	Project Start Date: October 18, 2000
	Class Start Date: Februari 14, 2004
*/

#ifndef CSSRULELIST_HPP
#define CSSRULELIST_HPP

// Standard C++ headers
#include <vector>

// Boost headers
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

// DOM Style headers
#include "CSSRule.hpp"

/// CSSRuleList implementation of the DOM CSS.

/**
	The CSSRuleList class stores a collection of css rules.
	For an extensive explanation, see the DOM CSS pages at
	http://www.w3.org/TR/2000/REC-DOM-Level-2-Style-20001113/css.html
*/

class CSSRuleList	{
	
	private:
		/// List of the css rules.
		const vector<CSSRulePtr> * mCSSRuleList;
	
	public:
		/// Constructor of the CSSRuleList class.
		/**
			The constructor of the CSSRuleList class. It stores the pointer to
			the stylesheet list.
			
			@param	aCSSRuleList	The list of css rules to store.
		*/
		CSSRuleList( const vector<CSSRulePtr> * aCSSRuleList );
		
		/// Destructor of the CSSRuleList class.
		/**
			The destructor of the CSSRuleList class does nothing. Everything is cleaned up
			automatically.
		*/
		~CSSRuleList();
		
		/// A function to get the number of css rules in the list.
		unsigned long getLength() const;
		
		/// A function to get an item.
		/**
			This function gets the item that is indexed at the value of the supplied argument.
			If the index is greater than the number of items in the map, a null item is returned.

			@param	aIndex	The index at which to get the item.
		*/
		CSSRulePtr item( unsigned long aIndex );
		
};

#endif
