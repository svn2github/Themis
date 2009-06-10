/*
	Copyright (c) 2008 Mark Hellegers. All Rights Reserved.
	
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
	Class Start Date: April 11, 2003
*/

/*	MarkedSecDeclParser implementation
	See MarkedSecDeclParser.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// SGMLParser headers
#include "SGMLScanner.hpp"
#include "ReadException.hpp"
#include "MarkedSecDeclParser.hpp"

MarkedSecDeclParser :: MarkedSecDeclParser(SGMLScanner * aScanner, TSchemaPtr aSchema)
					:  BaseParser(aScanner, aSchema) {

}

MarkedSecDeclParser :: ~MarkedSecDeclParser() {

}

void MarkedSecDeclParser :: parseStatusKeyWord() {

	if (mToken != IDENTIFIER_SYM) {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Status keyword expected",
							GENERIC);
	}

	string tokenText = mScanner->getTokenText();
	if (tokenText != kCDATA &&
	    tokenText != kIGNORE &&
	    tokenText != kINCLUDE &&
	    tokenText != kRCDATA &&
	    tokenText != kTEMP
	    ) {
	    string error = "Unknown status keyword found: " + tokenText;
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							error,
							GENERIC);
	}
	mToken = mScanner->nextToken();
	
}

Token MarkedSecDeclParser :: parse(Token aToken) {

	mToken = aToken;

	parsePsPlus();
	parseStatusKeyWord();
	// Check for optional whitespace
	parsePsStar();
	if (mToken != LEFT_SQUARE_BRACKET_SYM) {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"[ Expected",
							GENERIC);
	}
	mToken = mScanner->nextToken();
	
	// Currently always skipping over the content.
	bool endFound = false;
	while (!endFound) {
		if (mToken == RIGHT_SQUARE_BRACKET_SYM) {
			mToken = mScanner->nextToken();
			if (mToken == RIGHT_SQUARE_BRACKET_SYM) {
				endFound = true;
			}
		}
		mToken = mScanner->nextToken();
	}
	
	if (mToken != DECLARATION_END_SYM) {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Marked section declaration not closed correctly",
							GENERIC);
	}
	mToken = mScanner->nextToken();
	
	return mToken;
}
