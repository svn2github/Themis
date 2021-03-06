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

/*	AtRuleParser implementation
	See AtRuleParser.hpp for more information
	
*/

// Standard C headers
#include <stdio.h>

// CSSParser headers
#include "AtRuleParser.hpp"
#include "ReadException.hpp"

AtRuleParser :: AtRuleParser(CSSScanner * aScanner,
							 CSSStyleSheetPtr aStyleSheet)
			 : BaseParser(aScanner, aStyleSheet) {
	
}

AtRuleParser :: ~AtRuleParser() {

}

MediaListPtr AtRuleParser :: parseMediaList() {
	
	MediaListPtr result;
	
	if (mToken == IDENTIFIER_SYM) {
		// Found a medium. Create the list and add it.
		result = MediaListPtr(new MediaList());
		result->appendMedium(mScanner->getTokenText());
		mToken = mScanner->nextToken();
		parseSStar();
		bool mediumFound = true;
		while (mediumFound) {
			if (mToken == COMMA_SYM) {
				mToken = mScanner->nextToken();
				parseSStar();
				if (mToken == IDENTIFIER_SYM) {
					// Found another medium. Add it to the list.
					result->appendMedium(mScanner->getTokenText());
					mToken = mScanner->nextToken();
					parseSStar();
				}
				else {
					throw ReadException(mScanner->getLineNr(),
										mScanner->getCharNr(),
										"Expected identifier",
										GENERIC);
				}
			}
			else {
				mediumFound = false;
			}
		}
	}
	else {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Expected identifier",
							GENERIC);
	}
	
	return result;
	
}
