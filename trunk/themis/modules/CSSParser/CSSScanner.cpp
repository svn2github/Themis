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

/*	CSSScanner implementation
	See CSSScanner.hpp for more information
	
*/

// Standard C headers
#include <stdio.h>

// Standard C++ headers
#include <fstream>

// CSSScanner headers
#include "CSSScanner.hpp"
#include "ReadException.hpp"

// Namespaces used
using std::ifstream;

CSSScanner :: CSSScanner(const char * aFileName) {

	setDocument(aFileName);
	mPosition = Position(0, mText.size(), 1, 1);

}

CSSScanner :: ~CSSScanner() {

}

void CSSScanner :: scanIdentifier() {

	if (isalpha(mChar)) {
		mToken = mChar;
		mChar = nextChar();
	}
	else {
		throw ReadException(mPosition.getLineNr(),
							mPosition.getCharNr(),
							"Character unexpected",
							GENERIC);
	}
	
	bool identFound = true;
	while (identFound) {
		if (isalpha(mChar) || isdigit(mChar) || (mChar == '-')) {
			mToken += mChar;
			mChar = nextChar();
			
		}
		else {
			identFound = false;
		}
	}

}

void CSSScanner :: scanNumber() {

	if (isdigit(mChar)) {
		mToken = mChar;
		mChar = nextChar();
	}
	else {
		throw ReadException(mPosition.getLineNr(),
							mPosition.getCharNr(),
							"Character unexpected",
							GENERIC);
	}
	
	bool numFound = true;
	while (numFound) {
		if (isdigit(mChar)) {
			mToken += mChar;
			mChar = nextChar();
		}
		else {
			numFound = false;
		}
	}

}

void CSSScanner :: scanName() {

	if (isalpha(mChar) || isdigit(mChar) || (mChar == '-')) {
		mToken = mChar;
		mChar = nextChar();
		bool nmCharFound = true;
		while (nmCharFound) {
			if (isalpha(mChar) || isdigit(mChar) || (mChar == '-')) {
				mToken += mChar;
				mChar = nextChar();
			}
			else {
				nmCharFound = false;
			}
		}
	}
	else {
		throw ReadException(mPosition.getLineNr(),
							mPosition.getCharNr(),
							"Character unexpected",
							GENERIC);
	}

}

void CSSScanner :: scanURI() {

	if (mChar == '(') {
		mChar = nextChar();
		while ((isspace(mChar) || iscntrl(mChar)) && mChar != '\0') {
			mChar = nextChar();
		}
		if (isalpha(mChar)) {
			scanName();
		}
		else {
			throw ReadException(mPosition.getLineNr(),
								mPosition.getCharNr(),
								"Character unexpected",
								GENERIC);
		}
		while ((isspace(mChar) || iscntrl(mChar)) && mChar != '\0') {
			mChar = nextChar();
		}
		if (mChar == ')') {
			mChar = nextChar();
		}
		else {
			throw ReadException(mPosition.getLineNr(),
								mPosition.getCharNr(),
								"Character unexpected",
								GENERIC);
		}
		
	}
	else {
		throw ReadException(mPosition.getLineNr(),
							mPosition.getCharNr(),
							"Character unexpected",
							GENERIC);
	}

}

void CSSScanner :: scanComment() {

	mToken = "";

	bool endFound = false;
	while (!endFound) {
		if (mChar == '*') {
			mChar = nextChar();
			if (mChar == '/') {
				// Found the end of the comment.
				mChar = nextChar();
				endFound = true;
			}
			else {
				mToken += '*';
				mToken += mChar;
				mChar = nextChar();
			}
		}
		else {
			mToken += mChar;
			mChar = nextChar();
		}
	}

}

void CSSScanner :: scanSpace() {

	mToken = mChar;
	bool spaceFound = true;
	while (spaceFound) {
		mChar = nextChar();
		if ((!isspace(mChar) && !iscntrl(mChar)) || mChar == '\0') {
			spaceFound = false;
		}
		else {
			mToken += mChar;
		}
	}

}

void CSSScanner :: setDocument(const char * aFileName) {

	const int bufferSize = 1024;
	char buffer[bufferSize];

	mText.erase();

	// Load text
	if (aFileName != NULL)	{
		ifstream file(aFileName);
	
		while (file.get(buffer, bufferSize, '\0'))	{
			int count = file.gcount();
			if (count < bufferSize - 1) {
				file.ignore();
			}
			mText.append(buffer, count);
		};
		
		mChar = mText[mPosition.getIndex()];
	}

}

void CSSScanner :: setDocument(string aText) {

	mText = aText;
	mPosition = Position(0, mText.size(), 1, 1);
	mChar = mText[mPosition.getIndex()];

}

char CSSScanner :: nextChar() {

	char c = mText[mPosition.getIndex()];

	if (!mPosition.nextPosition(c)) {
		return '\0';
	}

	return mText[mPosition.getIndex()];

}

Token CSSScanner :: nextToken() {

	if (mChar == '-') {
		mChar = nextChar();
		if (isalpha(mChar)) {
			// Found an identifier
			scanIdentifier();
			mToken = '-' + mToken;
			if (mChar == '(') {
				mChar = nextChar();
				return FUNCTION_SYM;
			}
			else {
				if (mToken.c_str() == "URL") {
					scanURI();
					return URI_SYM;
				}
				else {
					return IDENTIFIER_SYM;
				}
			}
		}
		else if (isdigit(mChar)) {
			scanNumber();
			mToken = '-' + mToken;
			return NUMBER_SYM;
		}
		else {
			return MINUS_SYM;
		}
	}
	if (isalpha(mChar)) {
		// Found an identifier
		scanIdentifier();
		if (mChar == '(') {
			mChar = nextChar();
			return FUNCTION_SYM;
		}
		else {
			if (mToken.c_str() == "URL") {
				scanURI();
				return URI_SYM;
			}
			else {
				return IDENTIFIER_SYM;
			}
		}
	}
	if (isdigit(mChar)) {
		scanNumber();
		if (mChar == '.') {
			mChar = nextChar();
			string tempToken = mToken;
			scanNumber();
			mToken = tempToken + '.' + mToken;
		}
		if (mChar == '%') {
			mChar = nextChar();
			return PERCENTAGE_SYM;
		}
		else {
			return NUMBER_SYM;
		}
	}
	if (mChar == '.') {
		mChar = nextChar();
		if (isdigit(mChar)) {
			scanNumber();
			mToken = '.' + mToken;
			return NUMBER_SYM;
		}
		else {
			mToken = '.';
			return DOT_SYM;
		}
	}
	if (mChar == '/') {
		mChar = nextChar();
		if (mChar == '*') {
			// Found a comment
			mChar = nextChar();
			scanComment();
			return COMMENT_SYM;
		}
		else {
			// A slash
			mToken = '/';
			return SLASH_SYM;
		}
		
	}
	// Looking for space characters, but ignoring the special
	// eof marker '\0'.
	if ((isspace(mChar) || iscntrl(mChar)) && mChar != '\0') {
		scanSpace();
		return SPACE_SYM;
	}
	if (mChar == '@') {
		mChar = nextChar();
		scanIdentifier();
		return ATKEYWORD_SYM;
	}
	if (mChar == '#') {
		mChar = nextChar();
		scanName();
		return HASH_SYM;
	}
	if (mChar == '"' ||
		mChar == '\'') {
		char literal = mChar;
		bool textEnd = false;
		mToken = "";
		while (!textEnd) {
			mChar = nextChar();
			if (mChar == literal) {
				textEnd = true;
				mChar = nextChar();
			}
			else
				mToken += mChar;
		}
		return TEXT_SYM;
	}
	if (mChar == '~') {
		mToken = mChar;
		mChar = nextChar();
		if (mChar == '=') {
			mToken += mChar;
			mChar = nextChar();
			return INCLUDES_SYM;
		}
		else {
			throw ReadException(mPosition.getLineNr(),
								mPosition.getCharNr(),
								"Character unexpected",
								GENERIC);
		}
	}
	if (mChar == '|') {
		mToken = mChar;
		mChar = nextChar();
		if (mChar == '=') {
			mToken += mChar;
			mChar = nextChar();
			return DASHMATCH_SYM;
		}
		else {
			throw ReadException(mPosition.getLineNr(),
								mPosition.getCharNr(),
								"Character unexpected",
								GENERIC);
		}
	}
	if (mChar == '!') {
		bool spaceOrCommentFound = true;
		while (spaceOrCommentFound) {
			if ((isspace(mChar) || iscntrl(mChar)) && mChar != '\0') {
				scanSpace();
			}
			else if (mChar == '/') {
				scanComment();
			}
			else {
				spaceOrCommentFound = false;
			}
		}
		if (mChar == 'i') {
			scanIdentifier();
			if (mToken.c_str() == "important") {
				return IMPORTANT_SYM;
			}
			else {
				throw ReadException(mPosition.getLineNr(),
									mPosition.getCharNr(),
									"Character unexpected",
									GENERIC);
			}
			
		}
		
	}
	if (mChar == ';') {
		mToken = mChar;
		mChar = nextChar();
		return SEMICOLON_SYM;
	}
	if (mChar == '{') {
		mToken = mChar;
		mChar = nextChar();
		return LCURLYBRACKET_SYM;
	}
	if (mChar == '}') {
		mToken = mChar;
		mChar = nextChar();
		return RCURLYBRACKET_SYM;
	}
	if (mChar == '(') {
		mToken = mChar;
		mChar = nextChar();
		return LROUNDBRACKET_SYM;
	}
	if (mChar == ')') {
		mToken = mChar;
		mChar = nextChar();
		return RROUNDBRACKET_SYM;
	}
	if (mChar == '[') {
		mToken = mChar;
		mChar = nextChar();
		return LBRACKET_SYM;
	}
	if (mChar == ']') {
		mToken = mChar;
		mChar = nextChar();
		return RBRACKET_SYM;
	}
	if (mChar == ':') {
		mToken = mChar;
		mChar = nextChar();
		return COLON_SYM;
	}
	if (mChar == ',') {
		mToken = mChar;
		mChar = nextChar();
		return COMMA_SYM;
	}
	if (mChar == '=') {
		mToken = mChar;
		mChar = nextChar();
		return EQUALS_SYM;
	}
	if (mChar == '+') {
		mToken = mChar;
		mChar = nextChar();
		return PLUS_SYM;
	}
	if (mChar == '>') {
		mToken = mChar;
		mChar = nextChar();
		return GREATERTHAN_SYM;
	}
	if (mChar == '*') {
		mToken = mChar;
		mChar = nextChar();
		return STAR_SYM;
	}
	if (mChar == '\0') {
		return EOF_SYM;
	}
	// Temporary. Apparently everything is a delimiter, if it is unknown.
	if (mChar == '?') {
		mToken = mChar;
		mChar = nextChar();
		return DELIM_SYM;
	}

	printf("Unknown character found: %c\n", mChar);
	
	return NONE_SYM;
}

string CSSScanner :: getTokenText() {
	
	return mToken;

}

unsigned int CSSScanner :: getLineNr() const {

	return mPosition.getLineNr();

}

unsigned int CSSScanner :: getCharNr() const {

	return mPosition.getCharNr();

}
