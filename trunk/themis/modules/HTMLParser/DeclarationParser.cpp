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

/*	DeclarationParser implementation
	See DeclarationParser.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// SGMLParser headers
#include "SGMLScanner.hpp"
#include "ReadException.hpp"
#include "TSchema.hpp"
#include "DeclarationParser.hpp"

DeclarationParser :: DeclarationParser(SGMLScanner * aScanner, TSchemaPtr aSchema)
				  :  BaseParser(aScanner, aSchema) {

}

DeclarationParser :: ~DeclarationParser() {

}

TElementPtr DeclarationParser :: parseNameGroup() {

	TElementPtr group = mSchema->createElement("elements");
	TElementPtr element;
	
	parseTsStar();
	if (mToken == IDENTIFIER_SYM) {
		element = mSchema->createElement(mScanner->getTokenText());
		group->appendChild(element);
		mToken = mScanner->nextToken();
		bool inGroup = true;
		while (inGroup) {
			parseTsStar();
			if (mToken == AND_SYM ||
			    mToken == COMMA_SYM ||
			    mToken == PIPE_SYM) {
			    mToken = mScanner->nextToken();
			}
			else {
				inGroup = false;
				continue;
			}
			parseTsStar();
			if (mToken == IDENTIFIER_SYM) {
				element = mSchema->createElement(mScanner->getTokenText());
				group->appendChild(element);
				mToken = mScanner->nextToken();
			}
			else {
				throw ReadException(mScanner->getLineNr(),
									mScanner->getCharNr(),
									"Name expected",
									GENERIC);
			}
		}
	}
	else {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Name expected",
							GENERIC);
	}
	parseTsStar();
	if (mToken != RIGHT_BRACKET_SYM) {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Name group not closed correctly",
							GENERIC);
	}
	
	mToken = mScanner->nextToken();
	
	return group;

}

void DeclarationParser :: parseNameTokenGroup() {
	
	parseTsStar();
	if (mToken == IDENTIFIER_SYM ||
		mToken == NUMBER_SYM) {
		mToken = mScanner->nextToken();
		bool inGroup = true;
		while (inGroup) {
			parseTsStar();
			if (mToken == AND_SYM ||
			    mToken == COMMA_SYM ||
			    mToken == PIPE_SYM) {
			    mToken = mScanner->nextToken();
			}
			else {
				inGroup = false;
				continue;
			}
			parseTsStar();
			if (mToken == IDENTIFIER_SYM ||
				mToken == NUMBER_SYM) {
				mToken = mScanner->nextToken();
			}
			else {
				throw ReadException(mScanner->getLineNr(),
									mScanner->getCharNr(),
									"Name expected",
									GENERIC);
			}
		}
	}
	else {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Name expected",
							GENERIC);
	}
	parseTsStar();
	if (mToken != RIGHT_BRACKET_SYM) {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Name group not closed correctly",
							GENERIC);
	}
	
	mToken = mScanner->nextToken();

}

void DeclarationParser :: parseExternalId(string aToken,
										  TElementPtr aEntity) {

	if (aToken == kPUBLIC) {
		parsePsPlus();

		if (mToken != TEXT_SYM) {
			throw ReadException(mScanner->getLineNr(),
								mScanner->getCharNr(),
								"Public id expected",
								GENERIC);
		}
		aEntity->setAttribute("text", mScanner->getTokenText());
		mToken = mScanner->nextToken();
	}

	if (mToken == SPACE_SYM) {
		parsePsPlus();
		if (mToken == TEXT_SYM) {
			mToken = mScanner->nextToken();
		}
	}


}

void DeclarationParser :: parseExtEntitySpec(string aToken,
											 TElementPtr aEntity) {

	parseExternalId(aToken, aEntity);

}
