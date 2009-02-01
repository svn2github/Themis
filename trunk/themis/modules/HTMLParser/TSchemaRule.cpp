/*
	Copyright (c) 2006 Mark Hellegers. All Rights Reserved.
	
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
	Class Start Date: June 05, 2006
*/

/*
	SchemaRule implementation
	See TSchemaRule.h for some more information
*/

// Standard C headers
#include <stdio.h>

// Standard C++ headers
#include <algorithm>

// SGMLParser headers
#include "TSchemaRule.hpp"
#include "SGMLSupport.hpp"
#include "TSchema.hpp"
#include "TElementDeclaration.hpp"

// DOM headers
#include "TNode.h"
#include "TElement.h"
#include "TNodeList.h"
#include "TDocument.h"

TSchemaRule	::	TSchemaRule(const TDocumentPtr aOwnerDocument,
						    const TDOMString aTagName)
			:	TElement(aOwnerDocument, aTagName)	{

	mEmpty = false;

}

TSchemaRule	::	~TSchemaRule()	{
	
}

bool TSchemaRule :: computeEmpty() {

	bool subEmpty = false;
	bool subOneEmpty = false;
	bool subAllEmpty = true;
	
	TDOMString name = getTagName();
	if (name == "EMPTY") {
		mEmpty = true;
	}
	else if (name == "CDATA") {
		// Considering CDATA to be the same as EMPTY for now.
		mEmpty = true;
	}
	else if (name == "content") {
		TNodePtr child = getFirstChild();
		TSchemaRulePtr rule = shared_static_cast<TSchemaRule>(child);
		mEmpty = rule->computeEmpty();
	}
	else {
		TNodeListPtr children = getChildNodes();
		unsigned int length = children->getLength();
		for (unsigned int i = 0; i < length; i++) {
			TNodePtr child = children->item(i);
			TSchemaRulePtr rule = shared_static_cast<TSchemaRule>(child);
			subEmpty = rule->computeEmpty();
			subOneEmpty |= subEmpty;
			subAllEmpty &= subEmpty;
		}

		switch (name[0]) {
			case '|': {
				// If there is one subrule of this subrule
				// that can generate empty,
				// this whole subrule can generate empty.
				mEmpty = subOneEmpty;
				break;
			}
			case ',':
			case '&': {
				// All subrules of this subrule have to
				// generate empty, for this subrule to generate empty.
				mEmpty = subAllEmpty;
				break;
			}
		}

		if (!mEmpty && getAttribute("minOccurs") == "0") {
			mEmpty = true;
		}
	}

	return mEmpty;

}

bool TSchemaRule	::	computeFirst()	{

	bool changed = false;
	
	TDOMString name = getTagName();
	switch (name[0]) {
		case '&':
		case '|': {
			// Get the first of all the subrules.
			TNodeListPtr children = getChildNodes();
			unsigned int length = children->getLength();
			std::set<string> subFirst;
			bool subChanged = false;
			for (unsigned int i = 0; i < length; i++) {
				TNodePtr child = children->item(i);
				TSchemaRulePtr rule = shared_static_cast<TSchemaRule>(child);
				subChanged = rule->computeFirst();
				changed |= subChanged;
				if (subChanged) {
					subFirst = rule->getFirst();
					mFirst.insert(subFirst.begin(), subFirst.end());
					mFirstContent.insert(subFirst.begin(), subFirst.end());
				}
			}
			break;
		}
		case ',': {
			// Only add the first to the first of this rule, but always compute all childrens first.
			TNodeListPtr children = getChildNodes();
			unsigned int length = children->getLength();
			bool subEmpty = true;
			std::set<string> subFirst;
			bool subChanged = false;
			for (unsigned int i = 0; i < length; i++) {
				TNodePtr child = children->item(i);
				TSchemaRulePtr rule = shared_static_cast<TSchemaRule>(child);
				subChanged = rule->computeFirst();
				changed |= subChanged;
				if (subEmpty) {
					if (subChanged) {
						subFirst = rule->getFirst();
						mFirst.insert(subFirst.begin(), subFirst.end());
						mFirstContent.insert(subFirst.begin(), subFirst.end());
					}
					subEmpty &= rule->hasEmpty();
				}
			}
			break;
		}
		default: {
			if (name == "content") {
				TNodeListPtr children = getChildNodes();
				unsigned int length = children->getLength();
				std::set<string> subFirst;
				bool subChanged = false;
				for (unsigned int i = 0; i < length; i++) {
					TNodePtr child = children->item(i);
					TSchemaRulePtr rule = shared_static_cast<TSchemaRule>(child);
					subChanged = rule->computeFirst();
					changed |= subChanged;
					if (subChanged) {
						subFirst = rule->getFirst();
						mFirst.insert(subFirst.begin(), subFirst.end());
						mFirstContent.insert(subFirst.begin(), subFirst.end());
					}
				}
			}
			else if (name == "exceptions") {
				// Only add + exceptions
				// Actually remove - exceptions, but it doesn't bother me for now.
				if (getAttribute("type") == "+") {
					// Has one child: elements
					TNodePtr elementsChild = getFirstChild();
					TNodeListPtr children = elementsChild->getChildNodes();
					unsigned int length = children->getLength();
					for (unsigned int i = 0; i < length; i++) {
						TNodePtr child = children->item(i);
						TElementPtr exception = shared_static_cast<TElement>(child);
						TDOMString exceptionName = exception->getTagName();
						if (!mFirst.count(exceptionName)) {
							changed = true;
							// Add the tagname itself as a first.
							// It's a bit more complicated, but this will do for now.
							mFirst.insert(exceptionName);
							mFirstContent.insert(exceptionName);
						}
					}
				}
			}
			else if (name == "#PCDATA") {
				if (!mFirst.count(name)) {
					changed = true;
					mFirst.insert(name);
					mFirstContent.insert(name);
				}
			}
			else if (name == "EMPTY") {
				// Don't do anything.
			}
			else if (name == "CDATA") {
				if (!mFirst.count(name)) {
					changed = true;
					mFirst.insert(name);
					mFirstContent.insert(name);
				}
			}
			else {
				// Add the tagname itself as a first.
				if (!mFirst.count(name)) {
					changed = true;
					mFirst.insert(name);
				}
				// Add the first of the tagname.
				TSchemaPtr schema = shared_static_cast<TSchema>(getOwnerDocument());
				TElementDeclarationPtr declaration = schema->getDeclaration(name);
				std::set<string> subFirst = declaration->getFirst();
				if (!includes(mFirst.begin(), mFirst.end(),
							  subFirst.begin(), subFirst.end())) {
					mFirst.insert(subFirst.begin(), subFirst.end());
					changed = true;
				}
			}
		}
	}
	
	return changed;

}

std::set<string> TSchemaRule :: getFirst() {
	
	return mFirst;

}

void TSchemaRule :: computeLA() {

/*
	TElementPtr la = getOwnerDocument()->createElement("la");
	appendChild(la);

	// Add all the first tokens.
	TNodeListPtr children = getChildNodes();

	TNodePtr elements;

	unsigned int length = children->getLength();
	for (unsigned int i = 0; i < length; i++)	{
		TNodePtr child = children->item(i);
		if (child->getNodeName() == "first")	{
			elements = child;
			break;
		}
	}

	children = elements->getChildNodes();

	length = children->getLength();
	for (unsigned int i = 0; i < length; i++)	{
		TNodePtr child = children->item(i);
		TElementPtr token =
			shared_static_cast<TElement>(child->cloneNode(true));
		la->appendChild(token);
	}
*/

}

bool TSchemaRule :: hasEmpty() const {
	
	return mEmpty;

}

bool TSchemaRule :: hasToken(const TDOMString & aToken) const {

/*
	printf("First contains:\n");
	std::set<string>::iterator iterBegin = mFirst.begin();	
	std::set<string>::iterator iterEnd = mFirst.end();
	while (iterBegin != iterEnd) {
		printf("%s", (*iterBegin).c_str());
		++iterBegin;
		if (iterBegin != iterEnd) {
			printf(" | ");
		}
	}
	printf("\n");
*/
	
	return mFirst.count(aToken);

}

void TSchemaRule :: printFirst() const {

	printf("First contains:\n");
	std::set<string>::iterator iterBegin = mFirst.begin();	
	std::set<string>::iterator iterEnd = mFirst.end();
	while (iterBegin != iterEnd) {
		printf("%s", (*iterBegin).c_str());
		++iterBegin;
		if (iterBegin != iterEnd) {
			printf(" | ");
		}
	}
	printf("\n");

}

bool TSchemaRule :: hasContentToken(const TDOMString & aToken) const {

/*
	printf("First content contains:\n");
	std::set<string>::iterator iterBegin = mFirstContent.begin();	
	std::set<string>::iterator iterEnd = mFirstContent.end();
	while (iterBegin != iterEnd) {
		printf("%s", (*iterBegin).c_str());
		++iterBegin;
		if (iterBegin != iterEnd) {
			printf(" | ");
		}
	}
	printf("\n");
*/
	
	return mFirstContent.count(aToken);

}

bool TSchemaRule :: hasTagToken(const TDOMString & aToken) const {
	
	return mFirstTags.count(aToken);

}
