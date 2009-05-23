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

#ifndef CSSIMPORTRULE_HPP
#define CSSIMPORTRULE_HPP

// DOM Style headers
#include "CSSRule.hpp"

// Declarations used
class CSSStyleSheet;
class MediaList;
class CSSRule;

// Typedefs used
typedef boost::shared_ptr<CSSStyleSheet> CSSStyleSheetPtr;
typedef boost::shared_ptr<MediaList> MediaListPtr;
typedef boost::shared_ptr<CSSRule> CSSRulePtr;

/// CSSImportRule implementation of the DOM CSS.

/**
	The CSSImportRule class stores a css import rule.
	For an extensive explanation, see the DOM CSS pages at
	http://www.w3.org/TR/2000/REC-DOM-Level-2-Style-20001113/css.html
*/

class CSSImportRule	:	public CSSRule	{

	private:
		TDOMString mHref;
		MediaListPtr mMediaList;
		CSSStyleSheetPtr mStyleSheet;

	public:
		/// Constructor of the CSSImportRule class.
		/**
			The constructor of the CSSImportRule class. It stores
			the href, media list and the style sheet in addition to all the values
			stored by the CSSRule class.
			
			@param	aParentStyleSheet	The parent style sheet.
			@param	aParentRule			The parent rule.
			@param	aHref					The location of the style sheet to import.
			@param	aMediaList				The list of media types.
			@param	aStyleSheet			The style sheet to import.
		*/
		CSSImportRule( CSSStyleSheetPtr aParentStyleSheet, CSSRulePtr aParentRule,
							   TDOMString aHref, MediaListPtr aMediaList,
							   CSSStyleSheetPtr aStyleSheet );
		
		/// Destructor of the CSSImportRule class.
		/**
			The destructor of the CSSImportRule class does nothing. Everything is cleaned up
			automatically.
		*/
		~CSSImportRule();

		/// A function to get the href of the import rule.
		TDOMString getHref() const;

		/// A function to get the media list of the import rule.
		MediaListPtr getMediaList() const;

		/// A function to get the style sheet of the import rule.
		/**
			This function retrieves the style sheet of the import rule.
			It is null if the stylesheet has not been imported yet or if
			the style sheet is for an unsupported media type.
		*/
		CSSStyleSheetPtr getStyleSheet() const;
		
};

#endif
