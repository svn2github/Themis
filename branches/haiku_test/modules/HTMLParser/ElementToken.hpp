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
	Class Start Date: December 14, 2008
*/

/*	ElementToken
	Contains a high level element token.
	Either a start tag, end tag or text.

	Mark Hellegers (mark@firedisk.net)
	14-12-2008
*/

#ifndef ELEMENTTOKEN_HPP
#define ELEMENTTOKEN_HPP

// DOM headers
#include "DOMSupport.h"
#include "TNode.h"

// Namespaces used
using std::string;

enum ElementTokenType {
	START_TAG,
	END_TAG,
	TEXT,
	SPACE,
	NONE
};

/// Class to store an element token.

/**
	This class stores an element token.
*/

class ElementToken {
	
	private:
		ElementTokenType mType;
		TDOMString mName;
		TNodePtr mNode;
	
	public:
		ElementToken();
		ElementToken(ElementTokenType aType,
					 TDOMString aName = "",
					 TNodePtr aNode = TNodePtr());
		~ElementToken();
		ElementTokenType getType() const;
		TDOMString getName() const;
		TNodePtr getNode() const;

};

#endif
