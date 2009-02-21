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

/*	AttrListDeclParser
	Parses an attribute list declaration
	
	Mark Hellegers (mark@firedisk.net)
	11-04-2003

*/

#ifndef ATTRLISTDECLPARSER_HPP
#define ATTRLISTDECLPARSER_HPP

// SGMLParser headers
#include "SGMLSupport.hpp"
#include "DeclarationParser.hpp"

// Declarations of SGMLParser classes
class SGMLScanner;

class AttrListDeclParser : public DeclarationParser {
	
	private:
		TElementPtr parseAssElementType();
		string parseDeclValue();
		string parseDefValue();
		TElementPtr parseAttrDef(string aName);
		TElementPtr parseAttrDefList();
	
	public:
		AttrListDeclParser(SGMLScanner * aScanner, TSchemaPtr aSchema);
		~AttrListDeclParser();
		Token parse(Token aToken);
	
};

#endif
