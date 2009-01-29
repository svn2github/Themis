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

/*	TSchemaRule
	Stores a schema rule from a DTD.

	Mark Hellegers (mark@firedisk.net)
	05-06-2006
*/

#ifndef TSCHEMARULE_HPP
#define TSCHEMARULE_HPP

// Standard C++ headers
#include <set>

// DOM headers
#include "DOMSupport.h"
#include "TElement.h"
#include "TNodeList.h"

/// Class to store a schema rule.

/**
	This class stores a schema rule.
	It is based on a DOM element and provides
	a few functions to make it easy to parse an SGML document.
*/

class TSchemaRule	:	public TElement	{

	private:
		TNodeListPtr computeSubFirst(TElementPtr subRule);

	protected:
		bool mEmpty;
		std::set<string> mFirst;
		std::set<string> mFirstTags;
		std::set<string> mFirstContent;

	public:
		TSchemaRule(const TDocumentPtr aOwnerDocument,
					const TDOMString aTagName);
		virtual ~TSchemaRule();
		virtual bool computeEmpty();
		virtual bool computeFirst();
		std::set<string> getFirst();
		void computeLA();
		bool hasEmpty() const;
		bool hasToken(const TDOMString & aToken) const;
		bool hasContentToken(const TDOMString & aToken) const;
		bool hasTagToken(const TDOMString & aToken) const;

};

#endif
