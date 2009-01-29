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
	Class Start Date: April 12, 2003
*/

/*	BaseParser implementation
	See BaseParser.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// DOM headers
#include "TNode.h"
#include "TNodeList.h"
#include "TElement.h"

// SGMLParser headers
#include "ReadException.hpp"
#include "TSchema.hpp"
#include "BaseParser.hpp"

BaseParser :: BaseParser(SGMLScanner * aScanner, TSchemaPtr aSchema) {

	mScanner = aScanner;
	setSchema(aSchema);
}

BaseParser :: ~BaseParser() {

}

bool BaseParser :: parseParEntityReference() {

	// Lookahead to see if the second token is an identifier.
	Token lookAheadToken = mScanner->lookAhead();
	if (lookAheadToken == IDENTIFIER_SYM) {
		// Found a parameter entity.
		// Discard the next token as it is the lookahead token.
		mToken = mScanner->nextToken();
		string tokenText = mScanner->getTokenText();
		// See if the optional semicolon symbol exists.
		// First, make sure we can go back in case the semicolon does not exist.
		State refState = mScanner->saveState();
		lookAheadToken = mScanner->lookAhead();
		if (lookAheadToken == SEMICOLON_SYM) {
			// Scan it.
			mToken = mScanner->nextToken();
		}
		else {
			/*
				No semicolon.
			 	We now have to go back to the saved state as we now have
			 	the wrong token in the buffer.
			*/
			mScanner->restoreState(refState);
		}
//		printf("Found a parameter entity %s\n", tokenText.c_str());
		// Insert the text of the found parameter into the SGML text.
		TNodeListPtr entities = mParEntities->getElementsByTagName(tokenText);
		TNodePtr node = entities->item(0);
		TElementPtr entity = shared_static_cast<TElement>(node);
		if (entity->getAttribute("type") != kPUBLIC) {
			Position pos = mSchema->getEntityPosition(tokenText);
			if (pos.getSize() != 0) {
				mScanner->addEntity(pos);
			}
		}
		else	{
			//printf( "Equal to PUBLIC\n" );
		}
		mToken = mScanner->nextToken();
		return true;
	}
	
	return false;

}

bool BaseParser :: parseS(Token aEndToken, Token aEndToken2) {
	
	bool result;
	
	if (mToken == SPACE_SYM) {
		result = true;
		mToken = mScanner->nextToken(aEndToken, aEndToken2);
	}
	else
		result = false;

	return result;
	
}

void BaseParser :: parseSStar(Token aEndToken, Token aEndToken2) {
	 
	bool sFound = true;

	while (sFound) {
		sFound = parseS(aEndToken, aEndToken2);
	}
}



bool BaseParser :: parseTs() {
	
	bool result = false;
	
	if (mToken == SPACE_SYM)
		result = true;
	else if (mToken == PERCENTAGE_SYM)
		result = parseParEntityReference();

	return result;
}	

void BaseParser :: parseTsStar() {

	while (parseTs()) {
		mToken = mScanner->nextToken();
	}

}

bool BaseParser :: parsePs() {
	
	bool result = false;
	
	if (mToken == SPACE_SYM ||
		mToken == COMMENT_SYM)
		result = true;
	else if (mToken == PERCENTAGE_SYM)
		result = parseParEntityReference();

	return result;
}	

void BaseParser :: parsePsStar() {

	while (parsePs()) {
		mToken = mScanner->nextToken();
	}

}

void BaseParser :: parsePsPlus() {

	if (!parsePs()) {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Space expected",
							GENERIC,
							true);
	}

	mToken = mScanner->nextToken();
	parsePsStar();

}

void BaseParser :: setSchema(TSchemaPtr aSchema) {
	
	if (aSchema.get() != NULL) {
		mSchema = aSchema;
		// Load the convenience variables.
		mCharEntities = mSchema->getCharEntities();
		mParEntities = mSchema->getParEntities();
		mElements = mSchema->getElements();
		mAttrLists = mSchema->getAttrLists();
	}
	
}
