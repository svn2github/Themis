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
	Class Start Date: April 06, 2008
*/

/*	SGMLScanner
	Scans an SGML document into tokens.

	Mark Hellegers (mark@firedisk.net)
	06-04-2008
*/

#ifndef SGMLSCANNER_HPP
#define SGMLSCANNER_HPP

// Standard C++ headers
#include <string>

// SGMLParser headers
#include "SGMLText.hpp"
#include "Position.hpp"

// Namespaces used
using std::string;

enum Token {
	DECLARATION_SYM,
	DECLARATION_END_SYM,
	ELEMENT_OPEN_SYM,
	ELEMENT_CLOSE_SYM,
	ELEMENT_XML_CLOSE_SYM,
	COMMENT_SYM,
	MINUS_SYM,
	IDENTIFIER_SYM,
	NUMBER_SYM,
	TEXT_SYM,
	RAW_TEXT_SYM,
	PERCENTAGE_SYM,
	SEMICOLON_SYM,
	LEFT_BRACKET_SYM,
	RIGHT_BRACKET_SYM,
	LEFT_SQUARE_BRACKET_SYM,
	RIGHT_SQUARE_BRACKET_SYM,
	PIPE_SYM,
	STAR_SYM,
	PLUS_SYM,
	COMMA_SYM,
	AND_SYM,
	QUESTION_MARK_SYM,
	HASH_SYM,
	SPACE_SYM,
	EQUALS_SYM,
	EOF_SYM,
	NONE_SYM
};

/// Class to scan an SGML text.

/**
	This class scans an SGML text into tokens.
*/

class SGMLScanner {
	
	private:
		/// The SGMLText document.
		SGMLText mText;
		/// The next character in the document.
		char mChar;
		/// The actual token found;
		string mToken;
		/// An optional lookahead token.
		Token mLookAheadToken;
		/// The actual lookahead token.
		string mLookAheadTokenText;
		/// The start index of the token found
		unsigned int mTokenStartIndex;
	
	public:
		SGMLScanner(const char * aFilename = NULL);
		~SGMLScanner();
		void setDocument(const char * aFilename);
		void setDocument(string aText);
		Token nextToken(Token aEndToken = NONE_SYM, Token aEndToken2 = NONE_SYM);
		Token lookAhead();
		void addEntity(const Position aEntity);
		string getTokenText();
		unsigned int getTokenStartIndex() const;
		/// A function to get the index of the current position.
		unsigned int getIndex() const;
		/// A function to get the line number.
		unsigned int getLineNr() const;
		/// A function to get the character number.
		unsigned int getCharNr() const;
		/// A function to save the state of the scanner.
		State saveState();
		/// A function to restore the state.
		void restoreState(const State & aState);
		
		

};

#endif
