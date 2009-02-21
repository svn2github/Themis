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
	Class Start Date: April 14, 2003
*/

/*	DTDParser implementation
	See DTDParser.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// SGMLParser headers
#include "DTDParser.hpp"
#include "SGMLScanner.hpp"
#include "ReadException.hpp"
#include "EntityDeclParser.hpp"
#include "CommentDeclParser.hpp"
#include "MarkedSecDeclParser.hpp"
#include "ElementDeclParser.hpp"
#include "AttrListDeclParser.hpp"

DTDParser :: DTDParser(SGMLScanner * aScanner, TSchemaPtr aSchema)
		  :  BaseParser(aScanner, aSchema) {

	mEntityDeclParser = new EntityDeclParser(aScanner, aSchema);
	mCommentDeclParser = new CommentDeclParser(aScanner, aSchema);
	mMarkedSecDeclParser = new MarkedSecDeclParser(aScanner, aSchema);
	mElementDeclParser = new ElementDeclParser(aScanner, aSchema);
	mAttrListDeclParser = new AttrListDeclParser(aScanner, aSchema);

}

DTDParser :: ~DTDParser() {
	
	delete mEntityDeclParser;
	delete mCommentDeclParser;
	delete mMarkedSecDeclParser;
	delete mElementDeclParser;

}

bool DTDParser :: parseDs() {

	bool result;

	switch (mToken) {
		case SPACE_SYM:
		case COMMENT_SYM: {
			result = true;
			mToken = mScanner->nextToken();
			break;
		}
		case PERCENTAGE_SYM: {
			result = parseParEntityReference();
			break;
		}
		case DECLARATION_SYM: {
			Token lookAheadToken = mScanner->lookAhead();
			if (lookAheadToken == COMMENT_SYM) {
				// Scan the tokens.
				mScanner->nextToken();
				// Parse the rest of the comment declaration.
				mToken = mScanner->nextToken();
				mToken = mCommentDeclParser->parse(mToken);
				result = true;
			}
			else if (lookAheadToken == LEFT_SQUARE_BRACKET_SYM) {
				// Scan the tokens.
				mScanner->nextToken();
				// Parse the rest of the marked section declaration.
				mToken = mScanner->nextToken();
				mToken = mMarkedSecDeclParser->parse(mToken);
				result = true;
			}
			else {
				result = false;
			}
			break;
		}
		default: {
			result = false;
		}
	}

	return result;

}

void DTDParser :: parseDsStar() {

	bool dsFound = true;

	while (dsFound) {
		dsFound = parseDs();
	}

}

TSchemaPtr DTDParser :: parse(const char * aFilename) {

	mScanner->setDocument(aFilename);
	mToken = mScanner->nextToken();
	
	bool finished = false;
	while (!finished) {
		try {
			// Ignoring space or comments
			parseDsStar();
			if (mToken == DECLARATION_SYM) {
				mToken = mScanner->nextToken();
				if (mToken == IDENTIFIER_SYM) {
					string token = mScanner->getTokenText();
					if (token == kENTITY) {
						mToken = mScanner->nextToken();
						mToken = mEntityDeclParser->parse(mToken);
					}
					if (token == kELEMENT) {
						mToken = mScanner->nextToken();
						mToken = mElementDeclParser->parse(mToken);
					}
					if (token == kATTLIST) {
						mToken = mScanner->nextToken();
						mToken = mAttrListDeclParser->parse(mToken);
					}
				}
				continue;
			}
			if (mToken == EOF_SYM) {
				// End of the text. Stop parsing.
				finished = true;
				continue;
			}
			// Found unknown token.
			string message = "Unknown token found: " + mScanner->getTokenText();
			throw ReadException(mScanner->getLineNr(),
								mScanner->getCharNr(),
								message,
								GENERIC,
								true);
					
		}
		catch(ReadException r) {
			printf(
				"Found error: line: %i char %i message: %s\n",
				r.getLineNr(),
				r.getCharNr(),
				r.getErrorMessage().c_str());
			finished = true;
		}
	}

	return mSchema;

}

void DTDParser :: setSchema(TSchemaPtr aSchema) {
	
	BaseParser::setSchema(aSchema);
	
	if (aSchema.get() != NULL) {
		mEntityDeclParser->setSchema(aSchema);
		mCommentDeclParser->setSchema(aSchema);
		mMarkedSecDeclParser->setSchema(aSchema);
		mElementDeclParser->setSchema(aSchema);
		mAttrListDeclParser->setSchema(aSchema);
	}
	
}
