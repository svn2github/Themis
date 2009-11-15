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
	Class Start Date: November 5, 2009
*/

/*	MediaAtRuleParser
	Parses a media at-rule
	
	Mark Hellegers (mark@firedisk.net)
	05-11-2009
	
*/

#ifndef MEDIAATRULEPARSER_HPP
#define MEDIAATRULEPARSER_HPP

// Standard C++ headers
#include <string>

// CSSParser headers
#include "CSSScanner.hpp"
#include "AtRuleParser.hpp"

// Declarations of CSSParser classes
class RuleSetParser;

class MediaAtRuleParser : public AtRuleParser {

	private:
		RuleSetParser * mRuleSetParser;

	public:
		// Constructor
		MediaAtRuleParser(CSSScanner * aScanner, CSSStyleSheetPtr aStyleSheet);
		// Destructor
		~MediaAtRuleParser();
		// Parsing functions
		Token parse(Token aToken);

};

#endif
