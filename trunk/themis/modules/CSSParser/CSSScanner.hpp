/*
	Copyright (c) 2009 Mark Hellegers. All Rights Reserved.
	
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
	Class Start Date: June 8, 2009
*/

/*	CSSScanner
	Scans a css file into tokens
	
	Mark Hellegers (mark@firedisk.net)
	08-06-2009
	
*/

#ifndef CSSSCANNER_HPP
#define CSSSCANNER_HPP

// Standard C++ headers
#include <string>

// CSSParser headers
#include "Position.hpp"

// Namespaces used
using std::string;

enum Token {
	IDENTIFIER_SYM,
	URI_SYM,
	NUMBER_SYM,
	PERCENTAGE_SYM,
	ATKEYWORD_SYM,
	HASH_SYM,
	FUNCTION_SYM,
	TEXT_SYM,
	INCLUDES_SYM,
	DASHMATCH_SYM,
	IMPORTANT_SYM,
	SEMICOLON_SYM,
	COMMA_SYM,
	DOT_SYM,
	EQUALS_SYM,
	PLUS_SYM,
	MINUS_SYM,
	STAR_SYM,
	GREATERTHAN_SYM,
	LCURLYBRACKET_SYM,
	RCURLYBRACKET_SYM,
	LROUNDBRACKET_SYM,
	RROUNDBRACKET_SYM,
	LBRACKET_SYM,
	RBRACKET_SYM,
	COLON_SYM,
	DELIM_SYM,
	SLASH_SYM,
	COMMENT_SYM,
	SPACE_SYM,
	EOF_SYM,
	NONE_SYM
};

class CSSScanner {

	private:
		string mText;
		char mChar;
		string mToken;
		Position mPosition;
		
		// Function to scan an identifier token
		void scanIdentifier();
		// Function to scan a number.
		void scanNumber();
		// Function to scan a name
		void scanName();
		// Function to scan a URI
		void scanURI();
		// Function to scan a comment
		void scanComment();
		// Function to scan space
		void scanSpace();
		

	public:
		// Constructor
		CSSScanner(const char * aFileName = NULL);
		// Destructor
		~CSSScanner();
		void setDocument(const char * aFileName);
		void setDocument(string aText);
		char nextChar();
		Token nextToken();
		string getTokenText();
		/// A function to get the line number.
		unsigned int getLineNr() const;
		/// A function to get the character number.
		unsigned int getCharNr() const;

};

#endif
