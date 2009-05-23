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
	Class Start Date: Februari 27, 2003
*/

/* SGMLParser
	Parses an sgml file, using a dtd
	
	Mark Hellegers (mark@firedisk.net)
	27-02-2003
	
*/

#ifndef SGMLPARSER_HPP
#define SGMLPARSER_HPP

// Standard C++ headers
#include <string>
#include <vector>
#include <map>

// Themis headers
#include "DOMSupport.h"
#include "TDocument.h"

// SGMLParser headers
#include "BaseParser.hpp"
#include "SGMLText.hpp"
#include "SGMLScanner.hpp"

// Declarations of SGMLParser classes
class CommentDeclParser;
class DTDParser;
class DocTypeDeclParser;
class ElementParser;

using std::string;
using std::vector;
using std::map;

class SGMLParser : BaseParser {
	
	private:
		// Parser to parse comment declarations.
		CommentDeclParser * mCommentDeclParser;
		// Parser to parse document type declarations;
		DocTypeDeclParser * mDocTypeDeclParser;
		// Parser to parse DTDs.
		DTDParser * mDTDParser;
		// Name of the doctype.
		TDOMString mDocTypeName;
		// Storage of parsed schemas.
		map<string, TSchemaPtr> mSchemas;
	
		void showTree(const TNodePtr aNode, int aSpacing);
		bool parseOtherProlog();
		void parseOtherPrologStar();
		void parseProlog();
		void parseBaseDocTypeDecl();
		TDocumentPtr parseDocument();

	public:
		// Constructor
		SGMLParser(SGMLScanner * aScanner, TSchemaPtr aSchema);
		// Destructor
		~SGMLParser();
		// Parsing functions
		void parseSchema(const char * aSchemaFile);
		TDocumentPtr parse(const char * aSchemaFile, string aText);
		void parse(const char * aSchemaFile, const char * aDocument);
		// Function to load a schema.
		void loadSchema(const char * aSchemaFile);
	
};

#endif
