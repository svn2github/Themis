/*
	Copyright (c) 2009 Mark Hellegers. All Rights Reserved.
	
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
	Class Start Date: June 8, 2009
*/

/*	CSSParser
	Parses a css file
	
	Mark Hellegers (mark@firedisk.net)
	08-06-2009
	
*/

#ifndef CSSPARSER_HPP
#define CSSPARSER_HPP

// Standard C++ headers
#include <string>

// Namespaces used
using std::string;

// Declarations of CSSParser classes
class CSSScanner;
class ImportAtRuleParser;
class MediaAtRuleParser;
class RuleSetParser;

// DOM Style headers
#include "CSSStyleSheet.hpp"
#include "CSSStyleRule.hpp"
#include "CSSMediaRule.hpp"

class CSSParser {

	private:
		CSSScanner * mScanner;
		ImportAtRuleParser * mImportAtRuleParser;
		MediaAtRuleParser * mMediaAtRuleParser;
		RuleSetParser * mRuleSetParser;
		CSSStyleSheetPtr mStyleSheet;

		// Function to show a declaration block
		void showDeclaration(CSSStyleRulePtr aRule,
							 unsigned int aSpacing);
		// Function to show a style rule
		void showStyleRule(CSSStyleRulePtr aRule);
		// Function to show a media rule
		void showMediaRule(CSSMediaRulePtr aRule);
		// Function to show the style sheet.
		void showStyleSheet();
		void parseDocument();

	public:
		// Constructor
		CSSParser();
		// Destructor
		~CSSParser();
		// Parsing functions
		CSSStyleSheetPtr parse(const char * aDocument);
		CSSStyleSheetPtr parse(string aDocument);

};

#endif
