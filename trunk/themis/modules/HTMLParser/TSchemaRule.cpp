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

// SGMLParser headers
#include "TSchemaRule.hpp"

// DOM headers
#include "TNode.h"
#include "TElement.h"
#include "TNodeList.h"
#include "TDocument.h"

TSchemaRule	::	TSchemaRule(const TDocumentPtr aOwnerDocument,
						    const TDOMString aTagName)
			:	TElement(aOwnerDocument, aTagName )	{

}

TSchemaRule	::	~TSchemaRule()	{

}

void TSchemaRule	::	setElements(TElementPtr aElements) {

	mElements = aElements;
	appendChild(aElements);

}

void TSchemaRule	::	setContent(TElementPtr aContent) {

	mContent = aContent;
	appendChild(aContent);

}

bool TSchemaRule	::	hasRule(const TDOMString & aTagName) {

	TNodeListPtr elements = mElements->getChildNodes();

	unsigned int length = elements->getLength();
	unsigned int i = 0;
	bool foundElement = false;
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
