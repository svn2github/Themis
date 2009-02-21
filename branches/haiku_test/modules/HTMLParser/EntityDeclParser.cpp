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

/*	EntityDeclParser implementation
	See EntityDeclParser.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// SGMLParser headers
#include "SGMLScanner.hpp"
#include "ReadException.hpp"
#include "TSchema.hpp"
#include "EntityDeclParser.hpp"

EntityDeclParser :: EntityDeclParser(SGMLScanner * aScanner, TSchemaPtr aSchema)
				 :  DeclarationParser(aScanner, aSchema) {

}

EntityDeclParser :: ~EntityDeclParser() {

}

TElementPtr EntityDeclParser :: parseEntityName() {

	TElementPtr entity;

	switch (mToken) {
		case PERCENTAGE_SYM: {
			mToken = mScanner->nextToken();
			if (mToken != SPACE_SYM) {
				throw ReadException(mScanner->getLineNr(),
									mScanner->getCharNr(),
									"Space expected",
									GENERIC,
									true);
			}
			
			mToken = mScanner->nextToken();
			if (mToken != IDENTIFIER_SYM) {
				throw ReadException(mScanner->getLineNr(),
									mScanner->getCharNr(),
									"Name expected",
									GENERIC,
									true);
			}
			entity = mSchema->createElement(mScanner->getTokenText());
			mParEntities->appendChild(entity);
			break;
		}
		case IDENTIFIER_SYM: {
			entity = mSchema->createElement(mScanner->getTokenText());
			mCharEntities->appendChild(entity);
			break;
		}
		default: {
			throw ReadException(mScanner->getLineNr(),
								mScanner->getCharNr(),
								"Entity name expected",
								GENERIC,
								true);
		}
	}
	mToken = mScanner->nextToken();
	
	return entity;
}

void EntityDeclParser :: parseDataText(TElementPtr aEntity) {

	parsePsPlus();
	
	if (mToken != TEXT_SYM) {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Parameter literal expected",
							GENERIC,
							true);
	}

	unsigned int tokenStartIndex = mScanner->getTokenStartIndex();
	Position pos(tokenStartIndex,
				 mScanner->getIndex() - tokenStartIndex - 1,
				 mScanner->getLineNr(),
				 mScanner->getCharNr());
	aEntity->setAttribute("text", mScanner->getTokenText());
	mSchema->addEntity(aEntity->getNodeName(), pos);

	mToken = mScanner->nextToken();

}

void EntityDeclParser :: parseEntityText(TElementPtr aEntity) {

	switch (mToken) {
		case TEXT_SYM: {
			unsigned int tokenStartIndex = mScanner->getTokenStartIndex();
			Position pos(tokenStartIndex,
						 mScanner->getIndex() - tokenStartIndex - 1,
						 mScanner->getLineNr(),
						 mScanner->getCharNr());
			aEntity->setAttribute("text", mScanner->getTokenText());
			mSchema->addEntity(aEntity->getNodeName(), pos);
			mToken = mScanner->nextToken();
			break;
		}
		case IDENTIFIER_SYM: {
			string token = mScanner->getTokenText();
			if (token == kCDATA ||
				token == kSDATA ||
				token == kPI) {
				mToken = mScanner->nextToken();
				aEntity->setAttribute("type", token);
				parseDataText(aEntity);
			}
			else if (token == kSYSTEM ||
					 token == kPUBLIC) {
				aEntity->setAttribute("type", token);
				mToken = mScanner->nextToken();
				parseExtEntitySpec(token, aEntity);
			}
			else {
				throw ReadException(mScanner->getLineNr(),
									mScanner->getCharNr(),
									"CDATA SDATA, PI or PUBLIC or SYSTEM identifier expected",
									GENERIC,
									true);
			}
			break;
		}
		default: {
			throw ReadException(mScanner->getLineNr(),
								mScanner->getCharNr(),
								"Entity text expected",
								GENERIC,
								true );
		}
	}

}

Token EntityDeclParser :: parse(Token aToken) {

	mToken = aToken;
	
	parsePsPlus();
	TElementPtr entity = parseEntityName();
	parsePsPlus();
	parseEntityText(entity);

	// Check for optional whitespace
	parsePsStar();
	if (mToken != DECLARATION_END_SYM) {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Entity declaration not closed correctly",
							GENERIC,
							true);
	}
	mToken = mScanner->nextToken();
	
	return mToken;
}
