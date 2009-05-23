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

#ifndef CSSUNKNOWNRULE_HPP
#define CSSUNKNOWNRULE_HPP

// DOM Style headers
#include "CSSRule.hpp"

/// CSSUnknownRule implementation of the DOM CSS.

/**
	The CSSUnknownRule class stores a css unknown rule.
	It adds no functionality to the CSSRule class, but has its rule type set to unknown.
	For an extensive explanation, see the DOM CSS pages at
	http://www.w3.org/TR/2000/REC-DOM-Level-2-Style-20001113/css.html
*/

class CSSUnknownRule	:	public CSSRule	{

	public:
		/// Constructor of the CSSUnknownRule class.
		/**
			The constructor of the CSSUnknownRule class.
			It does nothing besides setting the type of the rule to unknown
			and passing the parameters to the CSSRule class. 
			
			@param	aParentStyleSheet	The parent style sheet.
			@param	aParentRule			The parent rule.
		*/
		CSSUnknownRule( CSSStyleSheetPtr aParentStyleSheet, CSSRulePtr aParentRule );
		
		/// Destructor of the CSSUnknownRule class.
		/**
			The destructor of the CSSUnknownRule class does nothing. Everything is cleaned up
			automatically.
		*/
		~CSSUnknownRule();

};

#endif
