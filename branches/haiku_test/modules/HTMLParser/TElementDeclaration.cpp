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
	ElementDeclaration implementation
	See TElementDeclaration.hpp for some more information
*/

// Standard C headers
#include <stdio.h>

// SGMLParser headers
#include "TElementDeclaration.hpp"

TElementDeclaration :: TElementDeclaration(const TDocumentPtr aOwnerDocument)
					: TSchemaRule(aOwnerDocument, "declaration") {

}

TElementDeclaration :: ~TElementDeclaration() {

}

bool TElementDeclaration :: computeEmpty() {

	bool subEmpty = false;
	
	if (mContent.get() != NULL) {
		subEmpty = mContent->computeEmpty();
	}
	
	bool start = false;
	bool end = false;
	getMinimization(start, end);
	
	mEmpty = !start && !end && subEmpty;
	
	return mEmpty;
}

bool TElementDeclaration :: computeFirst() {

	bool changed = false;
	
	if (mFirst.empty()) {
		// The first run.
		changed = true;
	}

	if (changed) {
		// Just add the elements name as a first token.
		TNodeListPtr elements = mElements->getChildNodes();
	
		unsigned int length = elements->getLength();
		for (unsigned int i = 0; i < length; i++) {
			TNodePtr elementNode = elements->item(i);
			TElementPtr element = shared_static_cast<TElement>(elementNode);
			TDOMString tagName = element->getTagName();
			mFirst.insert(tagName);
			mFirstTags.insert(tagName);
		}
	}
	
	bool start, end;
	getMinimization(start, end);

	// Always compute the first of the content.
	bool subChanged = mContent->computeFirst();
	changed |= subChanged;
	if (subChanged) {
		mFirstContent = mContent->getFirst();
	}
	
	if (mContent.get() != NULL && !start && subChanged) {
		// Add the content first tokens as well.
		mFirst.insert(mFirstContent.begin(), mFirstContent.end());
	}

/*	
	printf("First contains:\n");
	std::set<string>::iterator iterBegin = mFirst.begin();	
	std::set<string>::iterator iterEnd = mFirst.end();
	while (iterBegin != iterEnd) {
		printf("%s\n", (*iterBegin).c_str());
		++iterBegin;
	}
*/
	
	return changed;
	
}

void TElementDeclaration :: setMinimization(bool aStart, bool aEnd) {
	
	if (aStart)
		setAttribute("start", "true");
	else
		setAttribute("start", "false");
	if (aEnd)
		setAttribute("end", "true");
	else
		setAttribute("end", "false");

}

void TElementDeclaration :: getMinimization(bool & aStart, bool & aEnd) {

	if (getAttribute("start") == "true")
		aStart = true;
	else
		aStart = false;

	if (getAttribute("end") == "true")
		aEnd = true;
	else
		aEnd = false;
	
}

void TElementDeclaration :: setElements(TElementPtr aElements) {

	mElements = aElements;
	appendChild(aElements);

}

void TElementDeclaration :: setContent(TSchemaRulePtr aContent) {

	mContent = aContent;
	appendChild(aContent);

}

TSchemaRulePtr TElementDeclaration :: getContent() {

	return mContent;

}

bool TElementDeclaration :: hasRule(const TDOMString & aTagName) {

	bool foundElement = false;

	TNodeListPtr elements = mElements->getChildNodes();

	unsigned int length = elements->getLength();
	unsigned int i = 0;
	while (i < length && !foundElement) {
		TNodePtr elementNode = elements->item(i);
		TElementPtr element = shared_static_cast<TElement>(elementNode);
		if (element->getTagName() == aTagName) {
			foundElement = true;
		}
		i++;
	}
	
	return foundElement;

}

TDOMString TElementDeclaration :: getElementName() {
	
	TNodePtr node = mElements->getFirstChild();
	TElementPtr element = shared_static_cast<TElement>(node);
	return element->getTagName();

}
