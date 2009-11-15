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

/*	RuleSetParser
	Parses a rule set
	
	Mark Hellegers (mark@firedisk.net)
	05-11-2009
	
*/

#ifndef RULESETPARSER_HPP
#define RULESETPARSER_HPP

// Standard C++ headers
#include <string>

// CSSParser headers
#include "BaseParser.hpp"

// DOM Style headers
#include "Property.hpp"

class RuleSetParser : public BaseParser {

	private:
		TDOMString parseHash();
		TDOMString parseClass();
		TDOMString parseAttrib();
		TDOMString parseFunction();
		TDOMString parsePseudo();
		TDOMString parseSimpleSelector();
		TDOMString parseSelector();
		TDOMString parseNumericTerm();
		TDOMString parseTerm();
		bool isTerm();
		TDOMString parseExpression();
		Property parseDeclaration();

	public:
		// Constructor
		RuleSetParser(CSSScanner * aScanner, CSSStyleSheetPtr aStyleSheet);
		// Destructor
		~RuleSetParser();
		// Support functions.
		bool isRuleSet(Token aToken);
		// Parsing functions
		Token parse(Token aToken, CSSRulePtr aParentRule = CSSRulePtr());

};

#endif
