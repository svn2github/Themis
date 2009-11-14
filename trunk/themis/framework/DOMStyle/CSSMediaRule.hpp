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

#ifndef CSSMEDIARULE_HPP
#define CSSMEDIARULE_HPP

// DOM Style headers
#include "CSSRule.hpp"
#include "DOMStyleSupport.hpp"

// Standard C++ headers
#include <vector>

// Declarations used
class CSSStyleSheet;
class CSSRule;
class CSSRuleList;
class CSSMediaRule;
class MediaList;

// Typedefs used
typedef boost::shared_ptr<CSSRule> CSSRulePtr;
typedef boost::shared_ptr<CSSRuleList> CSSRuleListPtr;
typedef boost::shared_ptr<CSSStyleSheet> CSSStyleSheetPtr;
typedef boost::shared_ptr<CSSMediaRule> CSSMediaRulePtr;
typedef boost::shared_ptr<MediaList> MediaListPtr;

// Namespaces used;
using namespace std;

/// CSSMediaRule implementation of the DOM CSS.

/**
	The CSSMediaRule class stores a css media rule.
	For an extensive explanation, see the DOM CSS pages at
	http://www.w3.org/TR/2000/REC-DOM-Level-2-Style-20001113/css.html
*/


class CSSMediaRule : public CSSRule {

	private:
		vector<CSSRulePtr> mCssRuleList;
		CSSRuleListPtr mCssRules;
		MediaListPtr mMedia;

	public:
		/// Constructor of the CSSMediaRule class.
		/**
			The constructor of the CSSMediaRule class. It stores
			the media list and the css rules in addition to all the values
			stored by the CSSRule class.
			
			@param	aParentStyleSheet	The parent style sheet.
			@param	aParentRule			The parent rule.
			@param	aMedia				The list of media types.
		*/
		CSSMediaRule(CSSStyleSheetPtr aParentStyleSheet,
					 CSSRulePtr aParentRule,
					 MediaListPtr aMedia);
		
		/// Destructor of the CSSMediaRule class.
		/**
			The destructor of the CSSMediaRule class does nothing. Everything is cleaned up
			automatically.
		*/
		~CSSMediaRule();

		/// A function to get the media of the media rule.
		MediaListPtr getMedia() const;

		/// A function to get the css rule list of the media rule.
		CSSRuleListPtr getCssRules() const;

		/// A function to add a new rule.
		/**
			This function inserts a the rule specified by the first parameter at the
			index specified by the second parameter.
			
			@param	aRule	The parsable rule text to insert.
			@param	aIndex	The index in the media rule set to insert the rule at.

			@exception	HIERARCHY_REQUEST_ERR
								Thrown if the rule can not be inserted
								at this point in the style sheet.
			@exception	NO_MODIFICATION_ALLOWED_ERR
								Thrown if the rule is readonly.
			@exception	INDEX_SIZE_ERR
								Thrown if the specified index is not a valid insertion point.
		*/
		unsigned long insertRule(const CSSRulePtr aRule, unsigned long aIndex);
		
		/// A function to add a new rule.
		/**
			This function inserts a the rule specified by the first parameter at the
			index specified by the second parameter.
			
			@param	aRule	The parsable rule text to insert.
			@param	aIndex	The index in the media rule set to insert the rule at.

			@exception	SYNTAX_ERR
								Thrown if the rule is unparsable.
			@exception	HIERARCHY_REQUEST_ERR
								Thrown if the rule can not be inserted
								at this point in the style sheet.
			@exception	NO_MODIFICATION_ALLOWED_ERR
								Thrown if the rule is readonly.
			@exception	INDEX_SIZE_ERR
								Thrown if the specified index is not a valid insertion point.
		*/
		unsigned long insertRule(const TDOMString aRule, unsigned long aIndex);
		
		/// A function to delete a rule.
		/**
			This function deletes the rule at the index specified by the parameter.
			
			@param	aIndex	The index of the rule to delete.

			@exception	NO_MODIFICATION_ALLOWED_ERR
								Thrown if the rule is readonly.
			@exception	INDEX_SIZE_ERR
								Thrown if the rule at the specified index does not exist.
		*/
		void deleteRule(unsigned long aIndex);
};

#endif
