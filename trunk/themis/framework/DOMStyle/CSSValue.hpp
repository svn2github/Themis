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

#ifndef CSSVALUE_HPP
#define CSSVALUE_HPP

/// CSSValue implementation of the DOM CSS.

/**
	The CSSValue class stores a simple or complex css value.
	For an extensive explanation, see the DOM CSS pages at
	http://www.w3.org/TR/2000/REC-DOM-Level-2-Style-20001113/css.html
*/

class CSSValue	{
	
	public:
		enum	{
			/// The value is inherited and cssText contains "inherit"
			CSS_INHERIT = 0,
			/// The value is a primitive value.
			CSS_PRIMITIVE,
			/// The value is a css value list.
			CSS_VALUE_LIST,
			/// The value is a custom value.
			CSS_CUSTOM
		};
		
		/// Constructor of the CSSValue class.
		/**
			The constructor of the CSSValue class. It stores
			a css value and its type. Only the value can be
			changed after construction.
			
			@param	aCssText			The css text of the value to store.
			@param	aCssValueType	The type of the value to store.
		*/
		CSSValue( const TDOMString aCssText, unsigned short aCssValueType );

		/// Destructor of the CSSValue class.
		/**
			The destructor of the CSSValue class does nothing. Everything is cleaned up
			automatically.
		*/
		~CSSValue();

		/// A function to get the text of the value.
		/**
			This function gets the current text of the value.
		*/
		TDOMString getCssText() const;
		
		/// A function to set the text of the value.
		/**
			This function sets the current text of the value to
			the value of the supplied parameter.

			@param aCssText	The new text of the rule.
			
			@exception	SYNTAX_ERR	Thrown if the value is unparsable.
			@exception	INVALID_MODIFICATION_ERR
								Thrown if the parameter represents a different type of value
								than the current one.
			@exception	NO_MODIFICATION_ALLOWED_ERR
								Thrown if the value is readonly.
		*/
		void setCssText( const TDOMString aCssText );

		/// A function to get the type of the value.
		void getCssValueType();
};

#endif

