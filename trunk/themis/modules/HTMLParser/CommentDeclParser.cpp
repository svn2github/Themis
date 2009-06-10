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

/*	CommentDeclParser implementation
	See CommentDeclParser.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// SGMLParser headers
#include "SGMLScanner.hpp"
#include "ReadException.hpp"
#include "CommentDeclParser.hpp"

CommentDeclParser :: CommentDeclParser(SGMLScanner * aScanner, TSchemaPtr aSchema)
				  :  BaseParser(aScanner, aSchema) {

}

CommentDeclParser :: ~CommentDeclParser() {

}

Token CommentDeclParser :: parse(Token aToken, Token aEndToken) {

	mToken = aToken;
	
	bool moreData = true;
	while (moreData) {
		switch (mToken) {
			case SPACE_SYM:
			case COMMENT_SYM: {
				mToken = mScanner->nextToken();
				break;
			}
			case DECLARATION_END_SYM: {
				moreData = false;
				mToken = mScanner->nextToken(aEndToken);
				break;
			}
			default: {
				throw ReadException(mScanner->getLineNr(),
									mScanner->getCharNr(),
									"Comment declaration not closed correctly",
									GENERIC);
			}
		}
	}
	
	return mToken;
}
