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

/*	AttrListDeclParser implementation
	See AttrListDeclParser.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// DOM headers
#include "TElement.h"

// SGMLParser headers
#include "SGMLScanner.hpp"
#include "ReadException.hpp"
#include "TSchema.hpp"
#include "AttrListDeclParser.hpp"

AttrListDeclParser :: AttrListDeclParser(SGMLScanner * aScanner, TSchemaPtr aSchema)
				   :  DeclarationParser(aScanner, aSchema) {

}

AttrListDeclParser :: ~AttrListDeclParser() {

}

TElementPtr AttrListDeclParser :: parseAssElementType() {

	TElementPtr elements;

	if (mToken == LEFT_BRACKET_SYM) {
		// It's a name group
		mToken = mScanner->nextToken();
		elements = parseNameGroup();
	}
	else if (mToken == IDENTIFIER_SYM) {
		elements = mSchema->createElement("elements");
		TElementPtr element = mSchema->createElement(mScanner->getTokenText());
		elements->appendChild(element);
		mToken = mScanner->nextToken();
	}
	else {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Element type expected",
							GENERIC);
	}
	
	return elements;

}

string AttrListDeclParser :: parseDeclValue() {
	
	string tokenText = "";
	
	if (mToken == LEFT_BRACKET_SYM) {
		mToken = mScanner->nextToken();
		parseNameTokenGroup();
		// Ho ho ho.
		tokenText = "nameTokenGroup";
	}
	else if (mToken == IDENTIFIER_SYM) {
		tokenText = mScanner->getTokenText();
		if (tokenText == kCDATA ||
			tokenText == kENTITY ||
			tokenText == kID ||
			tokenText == kIDREF ||
			tokenText == kIDREFS ||
			tokenText == kNAME ||
			tokenText == kNAMES ||
			tokenText == kNMTOKEN ||
			tokenText == kNMTOKENS ||
			tokenText == kNUMBER ||
			tokenText == kNUMBERS ||
			tokenText == kNUTOKEN ||
			tokenText == kNUTOKENS) {
			mToken = mScanner->nextToken();
		}
		else {
			throw ReadException(mScanner->getLineNr(),
								mScanner->getCharNr(),
								"Declared value expected",
								GENERIC);
		}
	}
	else {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Declared value expected",
							GENERIC);
	}
	
	return tokenText;
		
}

string AttrListDeclParser :: parseDefValue() {

	string tokenText = "";

	if (mToken == TEXT_SYM ||
		mToken == IDENTIFIER_SYM ||
		mToken == NUMBER_SYM) {
		// Attr value spec, for now.
		tokenText = mScanner->getTokenText();
		mToken = mScanner->nextToken();
	}
	else if (mToken == HASH_SYM) {
		mToken = mScanner->nextToken();
		tokenText = mScanner->getTokenText();
		if (tokenText == kREQUIRED ||
			tokenText == kCURRENT ||
			tokenText == kCONREF ||
			tokenText == kIMPLIED) {
			mToken = mScanner->nextToken();
		}
		else if (tokenText == kFIXED) {
			mToken = mScanner->nextToken();
			parsePsPlus();
			if (mToken == TEXT_SYM) {
				tokenText = mScanner->getTokenText();
				mToken = mScanner->nextToken();
			}
			else {
				throw ReadException(mScanner->getLineNr(),
									mScanner->getCharNr(),
									"Text expected",
									GENERIC);
			}
		}
		else {
			throw ReadException(mScanner->getLineNr(),
								mScanner->getCharNr(),
								"Default value expected",
								GENERIC);
		}
	}
	else {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Default value expected",
							GENERIC);
	}
	
	return tokenText;
}

TElementPtr AttrListDeclParser :: parseAttrDef(string aName) {

	TElementPtr attrDef = mSchema->createElement(aName);
	parsePsPlus();
	string declValue = parseDeclValue();
	attrDef->setAttribute("declValue", declValue);
	parsePsPlus();
	string defValue = parseDefValue();
	attrDef->setAttribute("defValue", defValue);
	
	return attrDef;
}

TElementPtr AttrListDeclParser :: parseAttrDefList() {

	TElementPtr attrDefList = mSchema->createElement("attributes");
	TElementPtr attrDef;

	if (mToken == IDENTIFIER_SYM) {
		mToken = mScanner->nextToken();
		attrDef = parseAttrDef(mScanner->getTokenText());
		attrDefList->appendChild(attrDef);
	}
	else {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Attribute definition expected",
							GENERIC);
	}

	bool adFound = true;
	while (adFound) {
		if (mToken == SPACE_SYM) {
			parsePsPlus();
			if (mToken == IDENTIFIER_SYM) {
				mToken = mScanner->nextToken();
				attrDef = parseAttrDef(mScanner->getTokenText());
				attrDefList->appendChild(attrDef);
			}
			else {
				adFound = false;
			}
		}
		else {
			adFound = false;
		}
	}
	
	return attrDefList;

}

Token AttrListDeclParser :: parse(Token aToken) {

	TElementPtr declaration = mSchema->createElement("declaration");

	mToken = aToken;
	
	parsePsPlus();
	TElementPtr elements = parseAssElementType();
	declaration->appendChild(elements);
	parsePsPlus();
	TElementPtr attrDefList = parseAttrDefList();
	declaration->appendChild(attrDefList);

	// Check for optional whitespace
	parsePsStar();
	if (mToken != DECLARATION_END_SYM) {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Attribute list declaration not closed correctly",
							GENERIC);
	}
	mToken = mScanner->nextToken();
	
	mAttrLists->appendChild(declaration);

	return mToken;
}
