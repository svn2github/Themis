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

#ifndef CSSPAGERULE_HPP
#define CSSPAGERULE_HPP

// DOM Style headers
#include "CSSRule.hpp"

// Declarations used
class CSSStyleDeclaration;

// Typedefs used
typedef boost::shared_ptr<CSSStyleDeclaration> CSSStyleDeclarationPtr;

/// CSSPageRule implementation of the DOM CSS.

/**
	The CSSPageRule class stores a css page rule.
	For an extensive explanation, see the DOM CSS pages at
	http://www.w3.org/TR/2000/REC-DOM-Level-2-Style-20001113/css.html
*/

class CSSPageRule	:	public CSSRule	{
	
	private:
		CSSStyleDeclarationPtr mStyle;
		TDOMString mSelectorText;

	public:
		/// Constructor of the CSSPageRule class.
		/**
			The constructor of the CSSPageRule class. It stores
			the selectorText and the style in addition to all the values
			stored by the CSSRule class.
			
			@param	aParentStyleSheet	The parent style sheet.
			@param	aParentRule			The parent rule.
			@param	aSelectorText		The textual represention of the selector.
			@param	aStyle					The declaration block of this rule set.
		*/
		CSSPageRule( CSSStyleSheetPtr aParentStyleSheet, CSSRulePtr aParentRule,
							 TDOMString aSelectorText, CSSStyleDeclarationPtr aStyle );
		
		/// Destructor of the CSSPageRule class.
		/**
			The destructor of the CSSPageRule class does nothing. Everything is cleaned up
			automatically.
		*/
		~CSSPageRule();

		/// A function to get the selector text of the rule.
		/**
			This function gets the selector text of the rule.
			Insignificant whitespace may have been stripped out.
		*/
		TDOMString getSelectorText() const;
		
		/// A function to set the selector text of the rule.
		/**
			This function sets the selector text of the rule to
			the value of the supplied parameter.

			@param aSelectorText	The new selector text of the rule.
			
			@exception	SYNTAX_ERR	Thrown if the rule is unparsable.
			@exception	NO_MODIFICATION_ALLOWED_ERR
								Thrown if the rule is readonly.
		*/
		void setSelectorText( const TDOMString aSelectorText );
		
		/// A function to get the declaration block of the rule.
		CSSStyleDeclarationPtr getStyle() const;

};

#endif
