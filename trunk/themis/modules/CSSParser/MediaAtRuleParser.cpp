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

/*	MediaAtRuleParser implementation
	See MediaAtRuleParser.hpp for more information
	
*/

// Standard C headers
#include <stdio.h>

// CSSParser headers
#include "MediaAtRuleParser.hpp"
#include "RuleSetParser.hpp"
#include "ReadException.hpp"

// DOM Style headers
#include "MediaList.hpp"
#include "CSSMediaRule.hpp"
#include "CSSRuleList.hpp"

MediaAtRuleParser :: MediaAtRuleParser(CSSScanner * aScanner,
									   CSSStyleSheetPtr aStyleSheet)
				   : AtRuleParser(aScanner, aStyleSheet) {
	
	mRuleSetParser = new RuleSetParser(aScanner, aStyleSheet);
	
}

MediaAtRuleParser :: ~MediaAtRuleParser() {

	delete mRuleSetParser;

}

Token MediaAtRuleParser :: parse(Token aToken) {
	
	printf("Parsing media at-rule\n");
	
	mToken = mScanner->nextToken();
	
	if (mToken == SPACE_SYM) {
		mToken = mScanner->nextToken();
		MediaListPtr list = parseMediaList();
		CSSMediaRulePtr rule(new CSSMediaRule(mStyleSheet, CSSRulePtr(), list));
		if (mToken == LCURLYBRACKET_SYM) {
			mToken = mScanner->nextToken();
			parseSStar();
			// Need to parse a rule list.
			bool ruleSetFound = mRuleSetParser->isRuleSet(mToken);
			while (ruleSetFound) {
				mToken = mRuleSetParser->parse(mToken, rule);
				ruleSetFound = mRuleSetParser->isRuleSet(mToken);
			}
			
			if (mToken == RCURLYBRACKET_SYM) {
				mToken = mScanner->nextToken();
			}
			else {
				throw ReadException(mScanner->getLineNr(),
									mScanner->getCharNr(),
									"Expected }",
									GENERIC);
			}
		}
		else {
			throw ReadException(mScanner->getLineNr(),
								mScanner->getCharNr(),
								"Expected {",
								GENERIC);
		}

		// Done parsing. Add the rule to the stylesheet.
		unsigned int index = mStyleSheet->getCSSRules()->getLength();
		mStyleSheet->insertRule(rule, index);
	}
	else {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Expected space",
							GENERIC);
	}
	
	printf("Succesfully parsed media at-rule\n");
	
	return mToken;
}
