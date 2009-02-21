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

#ifndef RECT_HPP
#define RECT_HPP

// DOM Style headers
#include "DOMStyleSupport.hpp"

// Declarations
class CSSPrimitiveValue;

// Typedefs
typedef boost::shared_ptr<CSSPrimitiveValue> CSSPrimitiveValuePtr;

/// Rect implementation of the DOM CSS.

/**
	The Rect class stores a rect value.
	For an extensive explanation, see the DOM CSS pages at
	http://www.w3.org/TR/2000/REC-DOM-Level-2-Style-20001113/css.html
*/

class Rect	{

	private:
		/// The top of the rect.
		CSSPrimitiveValuePtr mTop;
		/// The right of the rect.
		CSSPrimitiveValuePtr mRight;
		/// The bottom of the rect.
		CSSPrimitiveValuePtr mBottom;
		/// The left of the rect.
		CSSPrimitiveValuePtr mLeft;

	public:
		/// Constructor of the Rect class.
		/**
			The constructor of the Rect class. It stores
			the top, right, bottom and left values of a rect.

			@param aTop		The top of the rect.
			@param aRight		The right of the rect.
			@param aBottom	The bottom of the rect.
			@param aLeft		The left of the rect.
		*/
		Rect( CSSPrimitiveValuePtr aTop, CSSPrimitiveValuePtr aRight,
				CSSPrimitiveValuePtr aBottom, CSSPrimitiveValuePtr aLeft );

		/// Destructor of the Rect class.
		/**
			The destructor of the Rect class does nothing. Everything is cleaned up
			automatically.
		*/
		~Rect();
		
		/// A function to get the top value of the rect.
		CSSPrimitiveValuePtr getTop() const;

		/// A function to get the right value of the rect.
		CSSPrimitiveValuePtr getRight() const;

		/// A function to get the bottom value of the rect.
		CSSPrimitiveValuePtr getBottom() const;

		/// A function to get the left value of the rect.
		CSSPrimitiveValuePtr getLeft() const;
		
};

#endif
