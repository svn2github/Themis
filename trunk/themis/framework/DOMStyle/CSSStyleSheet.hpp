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

#ifndef CSSSTYLESHEET_HPP
#define CSSSTYLESHEET_HPP

// DOM Style headers
#include "DOMStyleSupport.hpp"

// Declarations used
class CSSRule;
class CSSRuleList;

// Typedefs used
typedef boost::shared_ptr<CSSRule> CSSRulePtr;
typedef boost::shared_ptr<CSSRuleList> CSSRuleListPtr;

// DOM Style headers
#include "StyleSheet.hpp"

/// CSSStyleSheet implementation of the DOM CSS.

/**
	The CSSStyleSheet represents a CSS style sheet.
	It represents a single style sheet.
	For an extensive explanation, see the DOM CSS pages at
	http://www.w3.org/TR/2000/REC-DOM-Level-2-Style-20001113/css.html
*/

class CSSStyleSheet	:	public StyleSheet	{

	private:
		/// The owner rule.
		CSSRulePtr mOwnerRule;
		vector<CSSRulePtr> mCssRuleList;
		CSSRuleListPtr mCssRules;

	public:
		/// Constructor of the CSSStyleSheet class.
		/**
			The constructor of the CSSStyleSheet class.
			It initialized the read-only values that can not be changed after
			construction.
			
			@param aOwnerRule	The owner rule of the style sheet.
		*/
		CSSStyleSheet( CSSRulePtr aOwnerRule );
		
		/// Destructor of the CSSStyleSheet class.
		/**
			The destructor of the CSSStyleSheet class does nothing.
			Everything is cleaned up automatically.
		*/
		virtual ~CSSStyleSheet();
		
		/// A function to get the owner rule of the stylesheet.
		/**
			This function gets the owner rule of the stylesheet.
			If the style sheet comes from an import rule, a CSSImportRule
			will be returned and the owner node will be a null item.
			If the style sheet comes from an element or processing instruction
			the owner rule will be a null item and the owner node will contain
			the element or processing instruction.
		*/
		CSSRulePtr getOwnerRule() const;
		
		/// A function to get all the rules in the stylesheet.
		CSSRuleListPtr getCssRules() const;
		
		/// A function to insert a rule.
		/**
			This function inserts a new rule in the stylesheet.
			The new rule becomes part of the cascade.
			
			@param aRule	The new rule to insert.
			@param aIndex	The index to insert the new rule at.
									If the index is equal to the number of rules in the stylesheet,
									the rule is appended to the list of rules.
			
			@exception	HIERARCHY_REQUEST_ERR	Thrown if the rule can not be inserted
																		at the specified index.
			@exception	INDEX_SIZE_ERR	Thrown if the specified index is not a valid
														insertion point.
			@exception	NO_MODIFICATION_ALLOWED_ERR
								Thrown if the style sheet is readonly.

		*/
		unsigned long insertRule( const CSSRulePtr aRule, unsigned long aIndex );

		/// A function to insert a rule.
		/**
			This function inserts a new rule in the stylesheet.
			The new rule becomes part of the cascade.
			
			@param aRule	The new rule to insert.
			@param aIndex	The index to insert the new rule at.
									If the index is equal to the number of rules in the stylesheet,
									the rule is appended to the list of rules.
			
			@exception	HIERARCHY_REQUEST_ERR	Thrown if the rule can not be inserted
																		at the specified index.
			@exception	INDEX_SIZE_ERR	Thrown if the specified index is not a valid
														insertion point.
			@exception	NO_MODIFICATION_ALLOWED_ERR
								Thrown if the style sheet is readonly.
			@exception	SYNTAX_ERR	Thrown if the rule is unparseble.

		*/
		unsigned long insertRule( const TDOMString aRule, unsigned long aIndex );
		
		/// A function to delete a rule.
		/**
			This function deletes a rule from the style sheet.
			
			@param aIndex	The index of the rule to remove.

			@exception	INDEX_SIZE_ERR	Thrown if the specified index
														does not correspond to a rule
														in the style sheet.
			@exception	NO_MODIFICATION_ALLOWED_ERR
								Thrown if the style sheet is readonly.
		*/
		void deleteRule( unsigned long aIndex );

};

#endif
