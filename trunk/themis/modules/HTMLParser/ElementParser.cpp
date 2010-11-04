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
	Class Start Date: April 07, 2003
*/

/*	ElementParser implementation
	See ElementParser.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// DOM headers
#include "TText.h"
#include "TDocument.h"

// SGMLParser headers
#include "ElementParser.hpp"
#include "ReadException.hpp"
#include "TSchema.hpp"
#include "TElementDeclaration.hpp"
#include "CommentDeclParser.hpp"
#include "ElementDeclException.hpp"

ElementParser :: ElementParser(SGMLScanner * aScanner, TSchemaPtr aSchema, TDOMString aDocTypeName)
			  :  BaseParser(aScanner, aSchema) {

	mDocTypeName = aDocTypeName;
	mCommentDeclParser = NULL;

	mDocument = TDocumentPtr( new TDocument() );

}

ElementParser :: ~ElementParser() {
	
}

bool ElementParser :: parseAttrSpec(TElementPtr aElement) {

	bool result = false;
	
	parseSStar();
	
	if (mToken == IDENTIFIER_SYM) {
		string attribute = mScanner->getTokenText();
		mToken = mScanner->nextToken();
		parseSStar();
		if (mToken == EQUALS_SYM) {
			mToken = mScanner->nextToken(SPACE_SYM, DECLARATION_END_SYM);
			parseSStar(SPACE_SYM, DECLARATION_END_SYM);
			if (mToken == TEXT_SYM) {
				string value = mScanner->getTokenText();
				aElement->setAttribute(attribute, value);
				mToken = mScanner->nextToken();
				result = true;
			}
			else {
				string message = "Unknown token found: " + mScanner->getTokenText();
				throw ReadException(mScanner->getLineNr(),
									mScanner->getCharNr(),
									message,
									GENERIC);
			}
		}
		else {
			if (mToken == IDENTIFIER_SYM ||
				mToken == DECLARATION_END_SYM) {
				// Probably an attribute, with standard value.
				// Don't parse anything anymore as the whole attribute has been parsed.
				aElement->setAttribute(attribute, "");
				result = true;
			}
			else {
				string message = "Unknown token found: " + mScanner->getTokenText();
				throw ReadException(mScanner->getLineNr(),
									mScanner->getCharNr(),
									message,
									GENERIC);
			}
		}
	}
	
	return result;
}

void ElementParser :: parseAttrSpecList(TElementPtr aElement) {

	bool attrSpecFound = true;
	while ( attrSpecFound )	{
		attrSpecFound = parseAttrSpec(aElement);
	}
	
}

TElementPtr ElementParser :: parseStartTag() {
	
	TElementPtr element;

	mToken = mScanner->nextToken();
	
	if (mToken == IDENTIFIER_SYM) {
		string tokenName = mScanner->getTokenText();
		element = mDocument->createElement(tokenName);
		mToken = mScanner->nextToken();
		switch (mToken) {
			case SPACE_SYM: {
				parseAttrSpecList(element);
				if (mToken == DECLARATION_END_SYM ||
					mToken == ELEMENT_XML_CLOSE_SYM) {
					try {
						TElementDeclarationPtr declaration = mSchema->getDeclaration(tokenName);
						if (declaration->hasContentToken("CDATA")) {
							// Will have to assume it is CDATA.
							mToken = mScanner->nextToken(ELEMENT_CLOSE_SYM);
						}
						else {
							mToken = mScanner->nextToken(ELEMENT_OPEN_SYM);
						}
					}
					catch(ElementDeclException) {
						// This element does not exist. Pretend it does, without CDATA content
						mToken = mScanner->nextToken(ELEMENT_OPEN_SYM);
					}
				}
				else {
					string message = "Unknown token found: " + mScanner->getTokenText();
					throw ReadException(mScanner->getLineNr(),
										mScanner->getCharNr(),
										message,
										GENERIC);
				}
				break;
			}
			case DECLARATION_END_SYM:
			case ELEMENT_XML_CLOSE_SYM: {
				try {
					TElementDeclarationPtr declaration = mSchema->getDeclaration(tokenName);
					if (declaration->hasContentToken("CDATA")) {
						// Will have to assume it is CDATA.
						mToken = mScanner->nextToken(ELEMENT_CLOSE_SYM);
					}
					else {
						mToken = mScanner->nextToken(ELEMENT_OPEN_SYM);
					}
				}
				catch(ElementDeclException) {
					// This element does not exist. Pretend it does, without CDATA content
					mToken = mScanner->nextToken(ELEMENT_OPEN_SYM);
				}
				break;
			}
			default: {
				string message = "Unknown token found: " + mScanner->getTokenText();
				throw ReadException(mScanner->getLineNr(),
									mScanner->getCharNr(),
									message,
									GENERIC);
			}
		}
	}
	else {
		string message = "Unknown token found: " + mScanner->getTokenText();
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							message,
							GENERIC);
	}
	
	return element;

}

TDOMString ElementParser :: parseEndTag() {
	
	TDOMString tagName = "";
	
	mToken = mScanner->nextToken();
	
	if (mToken == IDENTIFIER_SYM) {
		tagName = mScanner->getTokenText();
		mToken = mScanner->nextToken();
		if (mToken == DECLARATION_END_SYM) {
			mToken = mScanner->nextToken(ELEMENT_OPEN_SYM);
		}
		else {
			string message = "Unknown token found: " + mScanner->getTokenText();
			throw ReadException(mScanner->getLineNr(),
								mScanner->getCharNr(),
								message,
								GENERIC);
		}
	}
	else {
		string message = "Unknown token found: " + mScanner->getTokenText();
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							message,
							GENERIC);
	}

	return tagName;

}

bool ElementParser :: parseDeclaration(TElementDeclarationPtr aDeclaration,
									   TNodePtr aParentNode) {

	bool found = false;

	switch (mElmToken.getType()) {
		case START_TAG: {
			// Is the tag in the current declaration ?
			TDOMString name = mElmToken.getName();
			TDOMString elementName = name;
			bool start, end;
			aDeclaration->getMinimization(start, end);
			bool tagTokenExists = aDeclaration->hasTagToken(name);
			if (tagTokenExists || !start) {
				// Get the next element token and parse again.
				TNodePtr currentNode;
				if (tagTokenExists) {
					// This is the right rule. Get the next token.
					currentNode = mElmToken.getNode();
					aParentNode->appendChild(currentNode);
					mElmToken = nextElmToken();
				}
				else {
					currentNode = mDocument->createElement(aDeclaration->getElementName());
					aParentNode->appendChild(currentNode);
				}
				// Get the content rule.
				TSchemaRulePtr rule = aDeclaration->getContent();
				name = mElmToken.getName();
				if (rule->hasToken(name)) {
					found = parse(rule, currentNode);
				}
				else if (rule->hasEmpty()) {
					// Doing nothing
				}
				// I don't think this should be here, but we will see what happens
				if (mElmToken.getType() == END_TAG && (elementName == mElmToken.getName())) {
					mElmToken = nextElmToken();
					found = true;
				}
				else {
					if (!end) {
						found = true;
					}
					else {
						found = false;
					}
				}
			}
			break;
		}
		default: {
			break;
		}
	}
	
	return found;
}

bool ElementParser :: parseSequence(TSchemaRulePtr aRule,
									TNodePtr aParentNode) {
	
	bool found = false;
	bool tokenFound = true;
	TDOMString name = mElmToken.getName();
	
	// We need to parse all the subrules of the rule.
	TNodeListPtr children = aRule->getChildNodes();
	unsigned int length = children->getLength();
	unsigned int i = 0;
	unsigned int j = 0; // Contains the number of rules that were found
	while (i < length && tokenFound) {
		TNodePtr child = children->item(i);
		TSchemaRulePtr rule = shared_static_cast<TSchemaRule>(child);
		if (rule->hasToken(name)) {
			// Parse token with this rule.
			tokenFound = parse(rule, aParentNode);
			name = mElmToken.getName();
			if (tokenFound)
				j++;
		}
		else if (rule->hasEmpty()) {
			// We can skip it. It is optional.
			j++;
		}
		else
			tokenFound = false;
		i++;
	}
	
	if (j == length) {
		// Found all the rules of the sequence.
		found = true;
	}
	
	return found;
	
}

bool ElementParser :: parseAll(TSchemaRulePtr aRule,
							   TNodePtr aParentNode) {
	
	bool found = false;
	TDOMString name = mElmToken.getName();
	
	// We need to parse all the subrules of the rule.
	TNodeListPtr children = aRule->getChildNodes();
	unsigned int length = children->getLength();
	bool progress = true;
	bool tokenFound = true;
	vector<bool> rulesDone( length, false );
	unsigned int j = 0;

	while (progress) {
		progress = false;
		unsigned int i = 0;
		while (i < length && tokenFound) {
			if (!rulesDone[i]) {
				// Haven't found this rule yet.
				TNodePtr child = children->item(i);
				TSchemaRulePtr rule = shared_static_cast<TSchemaRule>(child);
				if (rule->hasToken(name)) {
					// Parse token with this rule.
					tokenFound = parse(rule, aParentNode);
					name = mElmToken.getName();
					if (tokenFound) {
						// We maanged to find one of the rules.
						progress = true;
						rulesDone[i] = true;
						j++;
					}
				}
			}
			i++;
		}
	}

	if (j == length) {
		// Found all the rules of the sequence.
		found = true;
	}
	
	return found;
	
}

bool ElementParser :: parseChoice(TSchemaRulePtr aRule,
								  TNodePtr aParentNode) {

	// We need to find one rule that works.
	bool found = false;
	TNodeListPtr children = aRule->getChildNodes();
	unsigned int length = children->getLength();
	unsigned int i = 0;
	TDOMString tokenName = mElmToken.getName();
	if ((mElmToken.getType() != END_TAG) && aRule->hasToken(tokenName)) {
		i = 0;
		while (!found && i < length) {
			TNodePtr child = children->item(i);
			TSchemaRulePtr rule = shared_static_cast<TSchemaRule>(child);
			if (rule->hasToken(tokenName)) {
				found = parse(rule, aParentNode);
				if (found) {
					tokenName = mElmToken.getName();
				}
				else {
					tokenName = mElmToken.getName();
				}
			}
			i++;
		}
	}

	return found;

}

bool ElementParser :: parseContent(TSchemaRulePtr aRule,
								   TNodePtr aParentNode) {

	bool found = false;

	TDOMString name = mElmToken.getName();
	
	if (name != "") {
		if (aRule->hasChildNodes()) {
			TNodeListPtr children = aRule->getChildNodes();
			unsigned int length = children->getLength();
			if (length > 1) {
				// There are exceptions.
				// Look if the tag is in the exceptions.
				TNodePtr child = children->item(1);
				TSchemaRulePtr rule = shared_static_cast<TSchemaRule>(child);
				// Only look if it is a plus exception
				if (rule->getAttribute("type") == "+") {
					if (rule->hasToken(name)) {
						// It is in the exceptions.
						// Find the declaration of the tag and go from there.
						TElementDeclarationPtr declaration = mSchema->getDeclaration(name);
						found = parse(declaration, aParentNode);
					}
				}
			}
			if (length > 0) {
				TNodePtr child = aRule->getFirstChild();
				TSchemaRulePtr rule = shared_static_cast<TSchemaRule>(child);
				found = parse(rule, aParentNode);
			}
			// Hmm, there could still be exceptions.
			// Loop until we can't find any anymore.
			if (length > 1) {
				TNodePtr child = children->item(1);
				TSchemaRulePtr rule = shared_static_cast<TSchemaRule>(child);
				if (rule->getAttribute("type") == "+") {
					bool exceptionFound = true;
					while (exceptionFound) {
						name = mElmToken.getName();
						if (rule->hasToken(name)) {
							// It is in the exceptions.
							// Find the declaration of the tag and go from there.
							TElementDeclarationPtr declaration = mSchema->getDeclaration(name);
							found = parseDeclaration(declaration, aParentNode);
						}
						else {
							exceptionFound = false;
						}
					}
				}
			}
		}
	}
	
	return found;
}

Token ElementParser :: parse(ElementToken aElmToken, TSchemaRulePtr aRule) {
	
	mElmToken = aElmToken;
	parse(aRule, mDocument);

	if (mElmToken.getType() != NONE) {
		// Fallback.
		printf("Going to fallback mode\n");
		while (mElmToken.getType() != NONE) {
			switch (mElmToken.getType()) {
				case START_TAG: {
					printf("Start tag: %s\n", mElmToken.getName().c_str());
					break;
				}
				case END_TAG: {
					printf("End tag: %s\n", mElmToken.getName().c_str());
					break;
				}
				case SPACE: {
					printf("Space\n");
					break;
				}
				case TEXT: {
					printf("Text\n");
					break;
				}
				default: {
					printf("Something else\n");
				}
			}
			mElmToken = nextElmToken();
		}
	}
	else {
		printf("Finished document without going to fallback mode\n");
	}

	return mToken;

	
}

bool ElementParser :: parse(TSchemaRulePtr aRule, TNodePtr aParentNode) {

	bool found = false;
	unsigned int maxOccurs = atoi(aRule->getAttribute("maxOccurs").c_str());
	unsigned int i = 0;
	bool tokenFound = true;
	bool unlimited = false;
	string ruleName = aRule->getTagName();
	// Find out how often this rule may be played.
	if (maxOccurs == 0) {
		// We can repeat this rule as often as needed.
		unlimited = true;
	}

	while (tokenFound && (unlimited || i < maxOccurs)) {
		switch (mElmToken.getType()) {
			case START_TAG: {
				// We need to see if the rule contains the token we found.
				TDOMString name = mElmToken.getName();
				if (aRule->hasToken(name)) {
					if (ruleName == "declaration") {
						TElementDeclarationPtr declaration = shared_static_cast<TElementDeclaration>(aRule);
						tokenFound = parseDeclaration(declaration, aParentNode);
					}
					else if (ruleName == "content") {
						tokenFound = parseContent(aRule, aParentNode);
					}
					else if (ruleName == ",") {
						tokenFound = parseSequence(aRule, aParentNode);
					}
					else if (ruleName == "&") {
						tokenFound = parseAll(aRule, aParentNode);
					}
					else if (ruleName == "|") {
						tokenFound = parseChoice(aRule, aParentNode);
					}
					else {
						// Could be an element declaration.
						try {
							TElementDeclarationPtr declaration = mSchema->getDeclaration(ruleName);
							tokenFound = parse(declaration, aParentNode);
						}
						catch(ElementDeclException e) {
							// Nope. Do nothing for now.
						}
					}
				}
				else if (aRule->hasEmpty()) {
					tokenFound = false;
				}
				else {
					tokenFound = false;
				}
				break;
			}
			case TEXT: {
				TDOMString name = mElmToken.getName();
				if (aRule->hasToken(name)) {
					if (ruleName == "declaration") {
						TElementDeclarationPtr declaration = shared_static_cast<TElementDeclaration>(aRule);
						tokenFound = parseDeclaration(declaration, aParentNode);
					}
					else if (ruleName == "content") {
						tokenFound = parseContent(aRule, aParentNode);
					}
					else if (ruleName == name) {
						// Get the next element token and return true.
						aParentNode->appendChild(mElmToken.getNode());
						mElmToken = nextElmToken();
						tokenFound = true;
					}
					else if (ruleName == "|") {
						tokenFound = parseChoice(aRule, aParentNode);
					}
					else {
						tokenFound = false;
					}
				}
				else {
					tokenFound = false;
				}
				break;
			}
			default: {
				tokenFound = false;
			}
		}
		if (tokenFound) {
			i++;
		}
	}

	if (i > 0) {
		// Found at least one instance, so found the rule.
		found = true;
	}

	return found;

}

ElementToken ElementParser :: nextElmToken(bool aSkipSpace) {

	bool elmTokenFound = false;
	ElementToken elmToken;
	
	while (!elmTokenFound) {
		switch (mToken) {
			case SPACE_SYM: {
				if (!aSkipSpace) {
					elmToken = ElementToken(SPACE, mScanner->getTokenText());
					mToken = mScanner->nextToken();
		
					elmTokenFound = true;
				}
				else {
					mToken = mScanner->nextToken();
				}
				break;
			}
			case ELEMENT_OPEN_SYM: {
				// Found a start tag.
				TElementPtr element = parseStartTag();
				TDOMString tagName = element->getTagName();
				if (mSchema->hasDeclaration(tagName)) {
					elmToken = ElementToken(START_TAG, tagName, element);
	
					elmTokenFound = true;
				}
				break;
			}
			case ELEMENT_CLOSE_SYM: {
				TDOMString tagName = parseEndTag();
				if (mSchema->hasDeclaration(tagName)) {
					elmToken = ElementToken(END_TAG, tagName);
	
					elmTokenFound = true;
				}
				break;
			}
			case TEXT_SYM: {
				TTextPtr text = mDocument->createText(mScanner->getTokenText());
				elmToken = ElementToken(TEXT, "#PCDATA", text);
				mToken = mScanner->nextToken();
				
				elmTokenFound = true;
				break;
			}
			case RAW_TEXT_SYM: {
				TTextPtr text = mDocument->createText(mScanner->getTokenText());
				elmToken = ElementToken(TEXT, "CDATA", text);
				mToken = mScanner->nextToken();
				
				elmTokenFound = true;
				break;
			}
			case DECLARATION_SYM: {
				mToken = mScanner->nextToken();
				if (mToken == COMMENT_SYM) {
					if (mCommentDeclParser == NULL)
						mCommentDeclParser = new CommentDeclParser(mScanner, mSchema);
					mToken = mCommentDeclParser->parse(mToken, ELEMENT_OPEN_SYM);
				}
				else
					throw ReadException(mScanner->getLineNr(),
										mScanner->getCharNr(),
										"Expected comment sym",
										GENERIC);
				break;
			}
			case EOF_SYM: {
				elmToken = ElementToken(NONE);
				elmTokenFound = true;
				break;
			}
			default: {
				throw ReadException(mScanner->getLineNr(),
									mScanner->getCharNr(),
									"Unexpected token found: " + mScanner->getTokenText(),
									GENERIC);
			}
		}
	}
	
	return elmToken;

}

TDocumentPtr ElementParser :: getDocument() {

	return mDocument;

}
