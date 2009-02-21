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
	Class Start Date: April 12, 2003
*/

/// Base class for all parser classes.

/**
	This class provides the base functionality for all the parser classes
	the SGML parser uses.

	Mark Hellegers (mark@firedisk.net)
	12-04-2003
*/

#ifndef BASEPARSER_HPP
#define BASEPARSER_HPP

// SGMLParser headers
#include "SGMLSupport.hpp"
#include "SGMLScanner.hpp"

// DOM headers
#include "DOMSupport.h"

class BaseParser {
	
	protected:
		SGMLScanner * mScanner;
		Token mToken;
		/// The schema needed to parse or store information in.
		TSchemaPtr mSchema;
		/// The parameter entities stored in the schema.
		TElementPtr mParEntities;
		/// The character entities stored in the schema.
		TElementPtr mCharEntities;
		/// The elements stored in the schema.
		TElementPtr mElements;
		/// The attribute lists stored in the schema.
		TElementPtr mAttrLists;
		bool parseParEntityReference();
		bool parseS(Token aEndToken = NONE_SYM, Token aEndToken2 = NONE_SYM);
		void parseSStar(Token aEndToken = NONE_SYM, Token aEndToken2 = NONE_SYM);
		bool parseTs();
		void parseTsStar();
		bool parsePs();
		void parsePsStar();
		void parsePsPlus();
	
	public:
		BaseParser(SGMLScanner * aScanner, TSchemaPtr aSchema);
		virtual ~BaseParser();
		virtual void setSchema(TSchemaPtr aSchema);

};

#endif
