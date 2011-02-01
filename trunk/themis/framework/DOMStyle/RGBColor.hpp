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
	Class Start Date: March 07, 2004
*/

#ifndef RGBCOLOR_HPP
#define RGBCOLOR_HPP

// DOM Style headers
#include "CSSPrimitiveValue.hpp"

// Declarations
class CSSPrimitiveValue;

/// RGBColor implementation of the DOM CSS.

/**
	The RGBColor class stores an rgb color value.
	For an extensive explanation, see the DOM CSS pages at
	http://www.w3.org/TR/2000/REC-DOM-Level-2-Style-20001113/css.html
*/

class RGBColor {

	private:
		/// The red component of the color
		CSSPrimitiveValuePtr mRed;
		/// The green component of the color
		CSSPrimitiveValuePtr mGreen;
		/// The blue component of the color
		CSSPrimitiveValuePtr mBlue;
		// The name of the color if created with a name
		TDOMString mColorName;
	
	public:
		/// Constructor of the RGBColor class.
		/**
			The constructor of the RGBColor class. It stores
			the red, green and blue values of a color.

			@param aRed	The red component of the color.
			@param aGreen	The green component of the color.
			@param aBlue	The blue component of the color.	
		*/
		RGBColor(CSSPrimitiveValuePtr aRed,
				 CSSPrimitiveValuePtr aGreen,
				 CSSPrimitiveValuePtr aBlue);

		/// Constructor of the RGBColor class.
		/**
			The constructor of the RGBColor class. It stores
			the red, green and blue values of a color by converting
			the name of the color to its red, green and blue components.

			@param aColorName	The name of the color.
		*/
		RGBColor(TDOMString aColorName);

		/// Destructor of the RGBColor class.
		/**
			The destructor of the RGBColor class does nothing. Everything is cleaned up
			automatically.
		*/
		~RGBColor();
		
		/// A function to get the red value of the color.
		CSSPrimitiveValuePtr getRed() const;

		/// A function to get the green value of the color.
		CSSPrimitiveValuePtr getGreen() const;

		/// A function to get the blue value of the color.
		CSSPrimitiveValuePtr getBlue() const;
		
};

#endif
