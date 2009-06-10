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

/*	DocTypeDeclParser implementation
	See DocTypeDeclParser.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// SGMLParser headers
#include "SGMLScanner.hpp"
#include "ReadException.hpp"
#include "TSchema.hpp"
#include "DocTypeDeclParser.hpp"

DocTypeDeclParser :: DocTypeDeclParser(SGMLScanner * aScanner, TSchemaPtr aSchema)
				  :  DeclarationParser(aScanner, aSchema) {

}

DocTypeDeclParser :: ~DocTypeDeclParser() {

}

Token DocTypeDeclParser :: parse(Token aToken) {

	mToken = aToken;
	parsePsPlus();

	if (mToken == IDENTIFIER_SYM) {
		printf("Found document type name: %s\n", mScanner->getTokenText().c_str());
		mToken = mScanner->nextToken();
	}
	else {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Document type name expected",
							GENERIC);
	}

	TElementPtr extId = mSchema->createElement("externalId");
	
	if (mToken == SPACE_SYM) {
		parsePsPlus();
		if (mToken == IDENTIFIER_SYM) {
			string token = mScanner->getTokenText();
			if (token == kSYSTEM ||
				token == kPUBLIC) {
				mToken = mScanner->nextToken();
				parseExternalId(token, extId);
			}
			else
				throw ReadException(mScanner->getLineNr(),
									mScanner->getCharNr(),
									"PUBLIC or SYSTEM identifier expected",
									GENERIC);
		}
		else
			throw ReadException(mScanner->getLineNr(),
								mScanner->getCharNr(),
								"PUBLIC or SYSTEM identifier expected",
								GENERIC);
	}

	if (mToken != DECLARATION_END_SYM) {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Element declaration not closed correctly",
							GENERIC);
	}
	mToken = mScanner->nextToken();

	return mToken;
}
