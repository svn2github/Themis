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
	Class Start Date: Februari 14, 2004
*/

#ifndef CSSRULE_HPP
#define CSSRULE_HPP

/// CSSRule implementation of the DOM CSS.

/**
	The CSSRule class stores a css rule.
	For an extensive explanation, see the DOM CSS pages at
	http://www.w3.org/TR/2000/REC-DOM-Level-2-Style-20001113/css.html
*/

class CSSRule	{

	public:
		enum	{
			/// The type of the rule is not known.
			UNKNOWN_RULE = 0,
			/// The rule is a CSSStyleRule
			STYLE_RULE,
			/// The rule is a CSSCharsetRule
			CHARSET_RULE,
			/// The rule is a CSSImportRule
			IMPORT_RULE,
			/// The rule is a CSSMediaRule
			MEDIA_RULE,
			/// The rule is a CSSFontFaceRule
			FONT_FACE_RULE,
			/// The rule is a CSSPageRule
			PAGE_RULE
		};
		
		/// Constructor of the CSSRule class.
		/**
			The constructor of the CSSRule class. It stores
			values that can't be changed after construction.
			
			@param	aParentStyleSheet	The parent style sheet.
			@param	aParentRule			The parent rule.
			@param	aType					The type of the rule.
		*/
		CSSRule( CSSStyleSheetPtr aParentStyleSheet, CSSRulePtr aParentRule,
					  unsigned short aType );
		
		/// Destructor of the CSSRule class.
		/**
			The destructor of the CSSRule class does nothing. Everything is cleaned up
			automatically.
		*/
		~CSSRule();

		/// A function to get the text of the rule.
		/**
			This function gets the current text of the rule.
			This does not have to be the initial value of the rule.
		*/
		TDOMString getCssText() const;
		
		/// A function to set the text of the rule.
		/**
			This function sets the current text of the rule to
			the value of the supplied parameter.

			@param aCssText	The new text of the rule.
			
			@exception	SYNTAX_ERR
								Thrown if the rule is unparsable.
			@exception	INVALID_MODIFICATION_ERR
								Thrown if the parameter represents a different type of rule
								than the current one.
			@exception	HIERARCHY_REQUEST_ERR
								Thrown if the rule can not be inserted
								at this point in the style sheet.
			@exception	NO_MODIFICATION_ALLOWED_ERR
								Thrown if the rule is readonly.
		*/
		void setCssText( const TDOMString aCssText );
		
		/// A function to get the parent style sheet.
		CSSStyleSheetPtr getParentStyleSheet();

		/// A function to get the parent rule.
		/**
			This function gets the parent rule if this rule is contained in another rule.
			If it is not contained in another rule, it returns a null item.
		*/		
		CSSRulePtr getParentRule();
		
};

#endif
