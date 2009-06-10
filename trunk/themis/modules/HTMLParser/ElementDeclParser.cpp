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

/*	ElementDeclParser implementation
	See ElementDeclParser.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// SGMLParser headers
#include "SGMLScanner.hpp"
#include "ReadException.hpp"
#include "TSchema.hpp"
#include "TElementDeclaration.hpp"
#include "ElementDeclParser.hpp"

ElementDeclParser :: ElementDeclParser(SGMLScanner * aScanner, TSchemaPtr aSchema)
				  :  DeclarationParser(aScanner, aSchema) {

}

ElementDeclParser :: ~ElementDeclParser() {

}

TSchemaRulePtr ElementDeclParser :: parseSubModelGroup() {

	parseTsStar();
	TSchemaRulePtr contentToken = parseContentToken();
	parseTsStar();
	
	return contentToken;
}

TSchemaRulePtr ElementDeclParser :: parseContentToken() {

	TSchemaRulePtr contentToken;

	if (mToken == HASH_SYM) {
		mToken = mScanner->nextToken();
		if (mToken == IDENTIFIER_SYM) {
			string tokenText = mScanner->getTokenText();
			if (tokenText == kPCDATA) {
				contentToken = mSchema->createSchemaRule('#' + kPCDATA);
				mToken = mScanner->nextToken();
			}
			else {
				throw ReadException(mScanner->getLineNr(),
									mScanner->getCharNr(),
									"PCDATA expected",
									GENERIC);
			}
		}
		else {
			throw ReadException(mScanner->getLineNr(),
								mScanner->getCharNr(),
								"PCDATA expected",
								GENERIC);
		}
	}
	else if (mToken == IDENTIFIER_SYM) {
		contentToken = mSchema->createSchemaRule(mScanner->getTokenText());
		mToken = mScanner->nextToken();
		if (mToken == QUESTION_MARK_SYM) {
			contentToken->setAttribute("minOccurs", "0");
			contentToken->setAttribute("maxOccurs", "1");
			mToken = mScanner->nextToken();
		}
		else if (mToken == PLUS_SYM) {
			contentToken->setAttribute("minOccurs", "1");
			mToken = mScanner->nextToken();
		}
		else if (mToken == STAR_SYM) {
			contentToken->setAttribute("minOccurs", "0");
			mToken = mScanner->nextToken();
		}
	}
	else if (mToken == LEFT_BRACKET_SYM) {
		mToken = mScanner->nextToken();
		contentToken = parseModelGroup();
	}
	else {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Content token expected",
							GENERIC);
	}
	
	return contentToken;
}

TSchemaRulePtr ElementDeclParser :: parseModelGroup() {

	parseTsStar();
	TSchemaRulePtr contentToken = parseContentToken();
	parseTsStar();

	TSchemaRulePtr subModelGroup;
	TSchemaRulePtr connector;
	bool smgFound = true;
	string contentTokenName = contentToken->getNodeName();
	while (smgFound) {
		if (mToken == AND_SYM ||
			mToken == PIPE_SYM ||
			mToken == COMMA_SYM) {
			string tokenText = mScanner->getTokenText();
			mToken = mScanner->nextToken();
			subModelGroup = parseSubModelGroup();
			if (tokenText == contentTokenName)	{
				contentToken->appendChild(subModelGroup);
			}
			else	{
				connector = mSchema->createSchemaRule(tokenText);
				connector->appendChild(contentToken);
				connector->appendChild(subModelGroup);
				contentToken = connector;
				contentTokenName = contentToken->getNodeName();
			}
		}
		else {
			smgFound = false;
		}
	}
	
	if (mToken == RIGHT_BRACKET_SYM) {
		mToken = mScanner->nextToken();
	}
	else {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Model group not closed correctly",
							GENERIC);
	}

	if (mToken == QUESTION_MARK_SYM) {
		contentToken->setAttribute("minOccurs", "0");
		contentToken->setAttribute("maxOccurs", "1");
		mToken = mScanner->nextToken();
	}
	else if (mToken == PLUS_SYM) {
		contentToken->setAttribute("minOccurs", "1");
		mToken = mScanner->nextToken();
	}
	else if (mToken == STAR_SYM) {
		contentToken->setAttribute("minOccurs", "0");
		mToken = mScanner->nextToken();
	}

	return contentToken;
}

TSchemaRulePtr ElementDeclParser :: parseContent() {

	TSchemaRulePtr content = mSchema->createSchemaRule("content");

	if (mToken == IDENTIFIER_SYM) {
		string tokenText = mScanner->getTokenText();
		if (tokenText == kCDATA ||
			tokenText == kRCDATA ||
			tokenText == kEMPTY ||
			tokenText == kANY) {
			TSchemaRulePtr contentType = mSchema->createSchemaRule(tokenText);
			content->appendChild(contentType);
			mToken = mScanner->nextToken();
		}
		else {
			throw ReadException(mScanner->getLineNr(),
								mScanner->getCharNr(),
								"Declared content or ANY expected",
								GENERIC);
		}
	}
	else if (mToken == LEFT_BRACKET_SYM) {
		mToken = mScanner->nextToken();
		TSchemaRulePtr group = parseModelGroup();
		content->appendChild(group);
		if (mToken == SPACE_SYM) {
			// There might be exceptions
			parsePsPlus();
			if (mToken == MINUS_SYM ||
				mToken == PLUS_SYM) {
				// There are exceptions
				TSchemaRulePtr exceptions = mSchema->createSchemaRule("exceptions");
				content->appendChild(exceptions);
				exceptions->setAttribute("type", mScanner->getTokenText());
				mToken = mScanner->nextToken();
				if (mToken == LEFT_BRACKET_SYM) {
					mToken = mScanner->nextToken();
					TElementPtr elements = parseNameGroup();
					exceptions->appendChild(elements);
				}
				else {
					throw ReadException(mScanner->getLineNr(),
										mScanner->getCharNr(),
										"Name group expected",
										GENERIC);
				}
			}
		}
	}
	else {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Declared content or content model expected",
							GENERIC);
	}
	
	return content;
}

bool ElementDeclParser :: parseSingleTagMin() {

	bool required = false;

	if (mToken == IDENTIFIER_SYM) {
		string tokenText = mScanner->getTokenText();
		if (tokenText == "O") {
			// Not required. Already set.
			mToken = mScanner->nextToken();
		}
		else {
			throw ReadException(mScanner->getLineNr(),
								mScanner->getCharNr(),
								"Tag minimization expected",
								GENERIC);
		}
	}
	else if (mToken == MINUS_SYM) {
		required = true;
		mToken = mScanner->nextToken();
	}
	else {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Tag minimization expected",
							GENERIC);
	}
	
	return required;
}

void ElementDeclParser :: parseTagMin(TElementDeclarationPtr aDeclaration) {

	bool start = parseSingleTagMin();
	parsePsPlus();
	bool end = parseSingleTagMin();

	aDeclaration->setMinimization(start, end);

}

TElementPtr ElementDeclParser :: parseElementType() {
	
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

Token ElementDeclParser :: parse(Token aToken) {

	// Define an element to store the element declaration
	TElementDeclarationPtr declaration = mSchema->createElementDeclaration();

	mToken = aToken;
	
	parsePsPlus();
	TElementPtr element = parseElementType();
	declaration->setElements(element);
	parsePsPlus();
	parseTagMin(declaration);
	parsePsPlus();
	TSchemaRulePtr content = parseContent();
	// Only add the content model if it has one.
	if (content->hasChildNodes()) {
		declaration->setContent(content);
	}

	// Check for optional whitespace
	parsePsStar();
	if (mToken != DECLARATION_END_SYM) {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Element declaration not closed correctly",
							GENERIC);
	}
	mToken = mScanner->nextToken();

	mElements->appendChild(declaration);
	
	return mToken;
}
