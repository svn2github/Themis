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

#ifndef CSSCHARSETRULE_HPP
#define CSSCHARSETRULE_HPP

// DOM Style headers
#include "CSSRule.hpp"

/// CSSCharsetRule implementation of the DOM CSS.

/**
	The CSSCharsetRule class stores a css charset rule.
	For an extensive explanation, see the DOM CSS pages at
	http://www.w3.org/TR/2000/REC-DOM-Level-2-Style-20001113/css.html
*/

class CSSCharsetRule	:	public CSSRule	{

	public:
		/// Constructor of the CSSCharsetRule class.
		/**
			The constructor of the CSSCharsetRule class. It stores
			the encoding in addition to all the values
			stored by the CSSRule class.
			
			@param	aParentStyleSheet	The parent style sheet.
			@param	aParentRule			The parent rule.
			@param	aEncoding				The encoding information.
		*/
		CSSCharsetRule( CSSStyleSheetPtr aParentStyleSheet, CSSRulePtr aParentRule,
								 TDOMString aEncoding );
		
		/// Destructor of the CSSCharsetRule class.
		/**
			The destructor of the CSSCharsetRule class does nothing. Everything is cleaned up
			automatically.
		*/
		~CSSCharsetRule();

		/// A function to get the encoding of the charset rule.
		TDOMString getEncoding() const;

		/// A function to set the encoding of the rule.
		/**
			This function sets the encoding of the rule to
			the value of the supplied parameter.

			@param aEncoding	The new encoding of the rule.
			
			@exception	SYNTAX_ERR
								Thrown if the rule is unparsable.
			@exception	INVALID_MODIFICATION_ERR
								Thrown if the parameter represents a different type of rule
								than the current one.
		*/
		void setCssText( const TDOMString aCssText );

};

#endif
