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

#ifndef CSSPRIMITIVEVALUE_HPP
#define CSSPRIMITIVEVALUE_HPP

/// CSSPrimitiveValue implementation of the DOM CSS.

/**
	The CSSPrimitiveValue class stores a single css value.
	For an extensive explanation, see the DOM CSS pages at
	http://www.w3.org/TR/2000/REC-DOM-Level-2-Style-20001113/css.html
*/

class CSSPrimitiveValue	{
	
	public:
		enum	{
			/// The value is an unknown CSS2 value.
			CSS_UNKNOWN = 0,
			/// The value is a simple number.
			CSS_NUMBER,
			/// The value is a percentage.
			CSS_PERCENTAGE,
			/// The value is a length (ems).
			CSS_EMS,
			/// The value is a length (exs).
			CSS_EXS,
			/// The value is a length (px).
			CSS_PX,
			/// The value is a length (cm).
			CSS_CM,
			/// The value is a length (mm).
			CSS_MM,
			/// The value is a length (in).
			CSS_IN,
			/// The value is a length (pt).
			CSS_PT,
			/// The value is a length (pc).
			CSS_PC,
			/// The value is an angle (deg).
			CSS_DEG,
			/// The value is an angle (rad).
			CSS_RAD,
			/// The value is an angle (grad).
			CSS_GRAD,
			/// The value is a time (ms).
			CSS_MS,
			/// The value is a time (s).
			CSS_S,
			/// The value is a frequency (Hz).
			CSS_HZ,
			/// The value is a frequency (kHz).
			CSS_KHZ,
			/// The value is a number with an unknown dimension.
			CSS_DIMENSION,
			/// The value is a string.
			CSS_STRING,
			/// The value is an URI.
			CSS_URI,
			/// The value is an identifier.
			CSS_IDENT,
			/// The value is an attribute function.
			CSS_ATTR,
			/// The value is a counter or a counters function.
			CSS_COUNTER,
			/// The value is a rect function.
			CSS_RECT,
			/// The value is an RGB color.
			CSS_RGBCOLOR
		};
		
		/// Constructor of the CSSPrimitive class.
		/**
			The constructor of the CSSPrimitive class. It stores
			a css primitive value and its type. Only the value can be
			changed after construction.
			
			@param	aCssText				The css text of the value to store.
			@param	aCssPrimitiveType	The type of the value to store.
		*/
		CSSPrimitiveValue( const TDOMString aCssText,
									unsigned short aCssPrimitiveType );

		/// Destructor of the CSSPrimitiveValue class.
		/**
			The destructor of the CSSPrimitiveValue class does nothing. Everything is cleaned up
			automatically.
		*/
		~CSSPrimitiveValue();

		/// A function to get the type of the value.
		void getPrimitiveType();

		/// A function to set a float value.
		/**
			This function sets the float value of the specified type to
			the specified value.
			
			@param	aUnitType		The type of the float value.
			@param	aFloatValue	The value to set.

			@exception	INVALID_ACCESS_ERR Thrown if the supplied type is not a
								valid float type.
			@exception	NO_MODIFICATION_ALLOWED_ERR
								Thrown if the value is readonly.
		*/
		void setFloatValue( unsigned short aUnitType, float aFloatValue );

		/// A function to get a float value.
		/**
			This function gets the float value of the specified type.
			
			@param	aUnitType		The type of the float value.
			
			@exception INVALID_ACCESS_ERR Thrown if the css value doesn't contain
							  a float value or the value can't be converted to
							  the supplied type.
		*/
		float getFloatValue( unsigned short aUnitType );
		
		/// A function to set a string value.
		/**
			This function sets the string value of the specified type to
			the specified value.
			
			@param	aUnitType		The type of the float value.
			@param	aStringValue	The value to set.

			@exception	INVALID_ACCESS_ERR Thrown if the supplied type is not a
								valid float type.
			@exception	NO_MODIFICATION_ALLOWED_ERR
								Thrown if the value is readonly.
		*/
		void setStringValue( unsigned short aStringType, TDOMString aStringValue );

		/// A function to get a string value.
		/**
			This function gets a string value.
			All string types are accessible with this function.
			
			@exception INVALID_ACCESS_ERR Thrown if the css value doesn't contain
							  a string value.
		*/
		TDOMString getStringValue();
		
		/// A function to get a Counter value.
		/**
			This function gets a Counter value.
			Modifying the result, modifies the Counter value in this primitive value.
			
			@exception INVALID_ACCESS_ERR Thrown if the css value doesn't contain
							  a Counter value.
		*/
		CounterPtr getCounterValue();

		/// A function to get a Rect value.
		/**
			This function gets a Rect value.
			Modifying the result, modifies the Rect value in this primitive value.
			
			@exception INVALID_ACCESS_ERR Thrown if the css value doesn't contain
							  a Rect value.
		*/
		RectPtr getRectValue();

		/// A function to get a RGBColor value.
		/**
			This function gets a RGBColor value.
			Modifying the result, modifies the RGBColor value in this primitive value.
			
			@exception INVALID_ACCESS_ERR Thrown if the css value doesn't contain
							  a RGBColor value.
		*/
		RGBColorPtr getRGBColorValue();
};

#endif

