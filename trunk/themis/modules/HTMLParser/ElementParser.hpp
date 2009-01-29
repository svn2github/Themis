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

/*	ElementParser
	Parses an element
	
	Mark Hellegers (mark@firedisk.net)
	17-04-2003
	
*/

#ifndef ELEMENTPARSER_HPP
#define ELEMENTPARSER_HPP

// DOM headers
#include "TElement.h"
#include "DOMSupport.h"

// SGMLParser headers
#include "SGMLSupport.hpp"
#include "BaseParser.hpp"
#include "SGMLScanner.hpp"
#include "ElementToken.hpp"

// Declarations of SGMLParser classes
class CommentDeclParser;

class ElementParser : public BaseParser {
	
	private:
		// Parser to parse comment declarations.
		CommentDeclParser * mCommentDeclParser;

		TDOMString mDocTypeName;
		
		ElementToken mElmToken;
		
		// The document to store the elements in.
		TDocumentPtr mDocument;
	
		bool parseAttrSpec(TElementPtr aElement);
		void parseAttrSpecList(TElementPtr aElement);
	
	public:
		// Constructor
		ElementParser(SGMLScanner * aScanner, TSchemaPtr aSchema, TDOMString aDocTypeName);
		// Destructor
		~ElementParser();
		// Temporarily publically visible function
		TElementPtr parseStartTag();
		// Temporarily publically visible function
		TDOMString parseEndTag();
		// Procedure to parse a declaration rule.
		bool parseDeclaration(TElementDeclarationPtr aDeclaration, TNodePtr aParentNode);
		// Procedure to parse a sequence rule (,)
		bool parseSequence(TSchemaRulePtr aRule, TNodePtr aParentNode);
		// Procedure to parse an all rule (&)
		bool parseAll(TSchemaRulePtr aRule, TNodePtr aParentNode);
		// Procedure to parse a choice rule (|)
		bool parseChoice(TSchemaRulePtr aRule, TNodePtr aParentNode);
		// Procedure to parse a content rule
		bool parseContent(TSchemaRulePtr aRule, TNodePtr aParentNode);
		// Kickstart parsing function.
		Token parse(ElementToken aElmToken, TSchemaRulePtr aRule);
		// General parsing function.
		bool parse(TSchemaRulePtr aRule, TNodePtr aParentNode);
		// Function to get the next element token.
		ElementToken nextElmToken(bool aSkipSpace = true);
		// Function to get the document
		TDocumentPtr getDocument();
};

#endif
