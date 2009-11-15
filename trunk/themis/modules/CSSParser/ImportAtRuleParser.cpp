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
	Class Start Date: November 4, 2009
*/

/*	ImportAtRuleParser implementation
	See ImportAtRuleParser.hpp for more information
	
*/

// Standard C headers
#include <stdio.h>

// CSSParser headers
#include "ImportAtRuleParser.hpp"
#include "ReadException.hpp"

ImportAtRuleParser :: ImportAtRuleParser(CSSScanner * aScanner,
										 CSSStyleSheetPtr aStyleSheet)
				   : AtRuleParser(aScanner, aStyleSheet) {
	
}

ImportAtRuleParser :: ~ImportAtRuleParser() {

}

Token ImportAtRuleParser :: parse(Token aToken) {
	
	mToken = mScanner->nextToken();
	
	if (mToken == SPACE_SYM) {
		mToken = mScanner->nextToken();
		if (mToken == TEXT_SYM) {
			mToken = mScanner->nextToken();
			parseSStar();
			if (mToken == IDENTIFIER_SYM) {
				parseMediaList();
			}
			if (mToken == SEMICOLON_SYM) {
				mToken = mScanner->nextToken();
				parseSStar();
			}
			else {
				throw ReadException(mScanner->getLineNr(),
									mScanner->getCharNr(),
									"Expected semicolon",
									GENERIC);
			}
		}
		else {
			throw ReadException(mScanner->getLineNr(),
								mScanner->getCharNr(),
								"Symbol not currently supported",
								GENERIC);
		}
	}
	else {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Expected space",
							GENERIC);
	}
	
	printf("Succesfully parsed import at-rule\n");
	
	return mToken;
}
