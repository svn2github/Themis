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

#ifndef CSSFONTFACERULE_HPP
#define CSSFONTFACERULE_HPP

// DOM Style headers
#include "CSSRule.hpp"

/// CSSFontFaceRule implementation of the DOM CSS.

/**
	The CSSFontFaceRule class stores a css font face rule.
	For an extensive explanation, see the DOM CSS pages at
	http://www.w3.org/TR/2000/REC-DOM-Level-2-Style-20001113/css.html
*/

class CSSFontFaceRule	:	public CSSRule	{

	public:
		/// Constructor of the CSSFontFaceRule class.
		/**
			The constructor of the CSSFontFaceRule class. It stores
			the font style declaration.
			
			@param	aParentStyleSheet	The parent style sheet.
			@param	aParentRule			The parent rule.
			@param	aStyle					The font style.
		*/
		CSSFontFaceRule( CSSStyleSheetPtr aParentStyleSheet, CSSRulePtr aParentRule,
								   CSSStyleDeclarationPtr aStyle );
		
		/// Destructor of the CSSFontFaceRule class.
		/**
			The destructor of the CSSFontFaceRule class does nothing. Everything is cleaned up
			automatically.
		*/
		~CSSFontFaceRule();

		/// A function to get the declaration block of the rule.
		CSSStyleDeclarationPtr getStyle();

};

#endif
