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
	Class Start Date: March 07, 2004
*/

#ifndef COUNTER_HPP
#define COUNTER_HPP

// DOM Style headers
#include "DOMStyleSupport.hpp"

// Boost headers
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

// Declarations
class Counter;

// Typedefs
typedef boost::shared_ptr<Counter> CounterPtr;

/// Counter implementation of the DOM CSS.

/**
	The Counter class stores a counter value.
	For an extensive explanation, see the DOM CSS pages at
	http://www.w3.org/TR/2000/REC-DOM-Level-2-Style-20001113/css.html
*/

class Counter	{

	private:
		/// The identifier of the counter.
		TDOMString mIdentifier;
		
		/// The list style of the counter.
		TDOMString mListStyle;
		
		/// The separator of the counter.
		TDOMString mSeparator;

	public:
		/// Constructor of the Counter class.
		/**
			The constructor of the Counter class. It stores
			the identifier, listStyle and separator values of a counter.

			@param aIdentifier	The identifier of the counter.
			@param aListStyle		The right of the rect.
			@param aSeparator	The bottom of the rect.
		*/
		Counter( const TDOMString aIdentifier, const TDOMString aListStyle,
					  const TDOMString aSeparator );

		/// Destructor of the Counter class.
		/**
			The destructor of the Counter class does nothing. Everything is cleaned up
			automatically.
		*/
		~Counter();
		
		/// A function to get the identifier of the counter.
		TDOMString getIdentifier() const;

		/// A function to get the list style of the counter.
		TDOMString getListStyle() const;

		/// A function to get the separator of the counter.
		TDOMString getSeparator() const;

};

#endif
