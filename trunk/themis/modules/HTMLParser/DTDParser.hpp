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
	Class Start Date: April 14, 2003
*/

/*	DTDParser
	Parses a dtd
	
	Mark Hellegers (mark@firedisk.net)
	14-04-2003
	
*/

#ifndef DTDPARSER_HPP
#define DTDPARSER_HPP

// SGMLParser headers
#include "SGMLSupport.hpp"
#include "BaseParser.hpp"

// Declarations of SGMLParser classes
class EntityDeclParser;
class CommentDeclParser;
class MarkedSecDeclParser;
class ElementDeclParser;
class AttrListDeclParser;

class DTDParser : public BaseParser {
	
	private:
		// Parser for the entity declarations
		EntityDeclParser * mEntityDeclParser;
		// Parser for the comment declarations
		CommentDeclParser * mCommentDeclParser;
		// Parser for the marked section declarations
		MarkedSecDeclParser * mMarkedSecDeclParser;
		// Parser for the element declarations
		ElementDeclParser * mElementDeclParser;
		// Parser for the attribute list declarations
		AttrListDeclParser * mAttrListDeclParser;
		bool parseDs();
		void parseDsStar();
	
	public:
		// Constructor
		DTDParser(SGMLScanner * aScanner, TSchemaPtr aSchema);
		// Destructor
		~DTDParser();
		// Parsing function
		TSchemaPtr parse(const char * aFileName);
		virtual void setSchema(TSchemaPtr aSchema);
	
};

#endif
