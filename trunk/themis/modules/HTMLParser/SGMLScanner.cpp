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

/*	SGMLScanner implementation
	See SGMLScanner.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// SGMLParser headers
#include "SGMLScanner.hpp"
#include "ReadException.hpp"

SGMLScanner :: SGMLScanner(const char * aFilename) {

	setDocument(aFilename);
}

SGMLScanner :: ~SGMLScanner() {

}

void SGMLScanner :: setDocument(const char * aFilename) {
	
	if (aFilename != NULL) {
		mText.loadText(aFilename);
		mChar = mText.getChar();
	}
	mLookAheadToken = NONE_SYM;
	mLookAheadTokenText = "";

}

void SGMLScanner :: setDocument(string aText) {
	
	mText.loadText(aText);
	mChar = mText.getChar();

	mLookAheadToken = NONE_SYM;
	mLookAheadTokenText = "";
}

Token SGMLScanner :: nextToken(Token aEndToken, Token aEndToken2) {

	/*
		Looking for the next token.
		If we find it, we immediately return it.
	*/
	if (mLookAheadToken != NONE_SYM) {
		// We looked ahead.
		// Return it as the real token and reset
		// the lookahead token.
		Token temp = mLookAheadToken;
		mLookAheadToken = NONE_SYM;
		mToken = mLookAheadTokenText;
		mLookAheadTokenText = "";
		return temp;
	}
	// Store the start of the token in case someone wants to know.
	mTokenStartIndex = mText.getIndex();
	// Look if we need to scan everything until we find another token.
	if (aEndToken != NONE_SYM) {
		switch (aEndToken) {
			case ELEMENT_OPEN_SYM: {
				mToken = "";
				bool onlySpace = true;
				while (mChar != '<' && mChar != '\0') {
					// Check if it is a space character.
					if (!(isspace(mChar) || iscntrl(mChar)))
						onlySpace = false;
					mToken += mChar;
					mChar = mText.nextChar();
				}
				// Only return a token if we actually find text.
				// Otherwise, just start processing in the normal way.
				if (mToken.length() > 0) {
					if (onlySpace)
						return SPACE_SYM;
					else
						return TEXT_SYM;
				}
				break;
			}
			case ELEMENT_CLOSE_SYM: {
				mToken = "";
				bool onlySpace = true;
				bool found = false;
				while (!found) {
					while (mChar != '<' && mChar != '\0') {
						// Check if it is a space character.
						if (!(isspace(mChar) || iscntrl(mChar)))
							onlySpace = false;
						mToken += mChar;
						mChar = mText.nextChar();
					}
					if (mChar == '<') {
						// Ugly, but it works.
						mChar = mText.nextChar();
						if (mChar == '/') {
							found = true;
							mLookAheadToken = ELEMENT_CLOSE_SYM;
							mChar = mText.nextChar();
						}
						else {
							mToken += '<';
						}
					}
					else {
						// We need to exit the loop, but don't do anything else.
						found = true;
					}
				}
				// Only return a token if we actually find text.
				// If we don't find a token, but did find the close sym, return that instead.
				// Otherwise, just start processing in the normal way.
				if (mToken.length() > 0) {
					if (onlySpace)
						return SPACE_SYM;
					else
						return RAW_TEXT_SYM;
				}
				else if (mLookAheadToken == ELEMENT_CLOSE_SYM) {
					mLookAheadToken = NONE_SYM;
					return ELEMENT_CLOSE_SYM;
				}
				break;
			}
			case SPACE_SYM: {
				mToken = "";
				if (mChar == '"' ||
					mChar == '\'') {
					char literal = mChar;
					bool textEnd = false;
					mToken = "";
					while (!textEnd) {
						mChar = mText.nextChar();
						if (mChar == literal) {
							textEnd = true;
							mChar = mText.nextChar();
						}
						else
							mToken += mChar;
					}
					return TEXT_SYM;
				}
				else {
					bool insideElement = false;
					if (aEndToken2 == DECLARATION_END_SYM) {
						insideElement = true;
					}
					while ((!isspace(mChar) && !iscntrl(mChar)) &&
							(!insideElement || mChar != '>') && mChar != '\0') {
						mToken += mChar;
						mChar = mText.nextChar();
					}
					// Only return a TEXT_SYM token if we actually find text.
					// Otherwise, just start processing in the normal way.
					if (mToken.length() > 0) {
						return TEXT_SYM;
					}
				}
				break;
			}
			default: {
				throw ReadException(mText.getLineNr(),
									mText.getCharNr(),
									"Unsupported end token",
									GENERIC);
			}
		}
	}
	// Start scanning the next token.
	if (mChar == '<') {
		mToken = mChar;
		mChar = mText.nextChar();
		if (mChar == '!') {
			mToken += mChar;
			mChar = mText.nextChar();
			return DECLARATION_SYM;
		}
		else if (mChar == '/') {
			mToken += mChar;
			mChar = mText.nextChar();
			return ELEMENT_CLOSE_SYM;
		}
		else {
			return ELEMENT_OPEN_SYM;
		}
	}
	if (mChar == '>') {
		mToken = mChar;
		mChar = mText.nextChar();
		return DECLARATION_END_SYM;
	}
	if (mChar == '/') {
		mToken = mChar;
		mChar = mText.nextChar();
		if (mChar == '>') {
			mToken += mChar;
			mChar = mText.nextChar();
			return ELEMENT_XML_CLOSE_SYM;
		}
		else {
			throw ReadException(mText.getLineNr(),
								mText.getCharNr(),
								"Character unexpected",
								GENERIC);
		}
	}
	if (mChar == '-') {
		mToken = mChar;
		mChar = mText.nextChar();
		if (mChar == '-') {
			mToken += mChar;
			bool commentEnd = false;
			while (!commentEnd) {
				mChar = mText.nextChar();
				mToken += mChar;
				if (mChar == '-') {
					mChar = mText.nextChar();
					mToken += mChar;
					if (mChar == '-') {
						commentEnd = true;
						mChar = mText.nextChar();
//						printf("Comment found: %s\n", mToken.c_str());
						return COMMENT_SYM;
					}
				}
			}
		}
		else {
			return MINUS_SYM;
		}
	}
	if (isalpha(mChar)) {
		bool identifierEnd = false;
		mToken = toupper(mChar);
		while (!identifierEnd) {
			mChar = mText.nextChar();
			if (!isalpha(mChar) && mChar != '/' && mChar != '.' && mChar != '-' && !isdigit(mChar) && mChar != ':') {
				identifierEnd = true;
			}
			else	{
				mToken += toupper(mChar);
			}
		}
		return IDENTIFIER_SYM;
	}
	if (isdigit(mChar)) {
		bool numberEnd = false;
		mToken = mChar;
		while (!numberEnd) {
			mChar = mText.nextChar();
			if (!isdigit(mChar)) {
				numberEnd = true;
			}
			else	{
				mToken += mChar;
			}
		}
		return NUMBER_SYM;
	}
	if (mChar == '"' ||
		mChar == '\'') {
		char literal = mChar;
		bool textEnd = false;
		mToken = "";
		while (!textEnd) {
			mChar = mText.nextChar();
			if (mChar == literal) {
				textEnd = true;
				mChar = mText.nextChar();
			}
			else
				mToken += mChar;
		}
		return TEXT_SYM;
	}
	if (mChar == '%') {
		mToken = mChar;
		mChar = mText.nextChar();
		return PERCENTAGE_SYM;
	}
	if (mChar == ';') {
		mToken = mChar;
		mChar = mText.nextChar();
		return SEMICOLON_SYM;
	}
	if (mChar == '(') {
		mToken = mChar;
		mChar = mText.nextChar();
		return LEFT_BRACKET_SYM;
	}
	if (mChar == ')') {
		mToken = mChar;
		mChar = mText.nextChar();
		return RIGHT_BRACKET_SYM;
	}
	if (mChar == '[') {
		mToken = mChar;
		mChar = mText.nextChar();
		return LEFT_SQUARE_BRACKET_SYM;
	}
	if (mChar == ']') {
		mToken = mChar;
		mChar = mText.nextChar();
		return RIGHT_SQUARE_BRACKET_SYM;
	}
	if (mChar == '|') {
		mToken = mChar;
		mChar = mText.nextChar();
		return PIPE_SYM;
	}
	if (mChar == '*') {
		mToken = mChar;
		mChar = mText.nextChar();
		return STAR_SYM;
	}
	if (mChar == '+') {
		mToken = mChar;
		mChar = mText.nextChar();
		return PLUS_SYM;
	}
	if (mChar == ',') {
		mToken = mChar;
		mChar = mText.nextChar();
		return COMMA_SYM;
	}
	if (mChar == '&') {
		mToken = mChar;
		mChar = mText.nextChar();
		return AND_SYM;
	}
	if (mChar == '?') {
		mToken = mChar;
		mChar = mText.nextChar();
		return QUESTION_MARK_SYM;
	}
	if (mChar == '#') {
		mToken = mChar;
		mChar = mText.nextChar();
		return HASH_SYM;
	}
	if (mChar == '=') {
		mToken = mChar;
		mChar = mText.nextChar();
		return EQUALS_SYM;
	}
	// Looking for space characters, but ignoring the special
	// eof marker '\0'.
	if ((isspace(mChar) || iscntrl(mChar)) && mChar != '\0') {
		mToken = mChar;
		bool spaceFound = true;
		while (spaceFound) {
			mChar = mText.nextChar();
			if ((!isspace(mChar) && !iscntrl(mChar)) || mChar == '\0') {
				spaceFound = false;
			}
			else {
				mToken += mChar;
			}
		}
		return SPACE_SYM;
	}
	if (mChar == '\0') {
		return EOF_SYM;
	}
	// If we got here, we found a character we didn't expect.
	throw ReadException(mText.getLineNr(),
						mText.getCharNr(),
						"Character unexpected",
						GENERIC);

}

Token SGMLScanner :: lookAhead() {

	if (mLookAheadToken == NONE_SYM) {
		// Haven't looked ahead yet after the last nextToken.
		string temp = mToken;
		mLookAheadToken = nextToken();
		mLookAheadTokenText = mToken;
		mToken = temp;
	}
	return mLookAheadToken;
}

void SGMLScanner :: addEntity(const Position aEntity) {
	
	mText.addEntity(aEntity);
}

string SGMLScanner :: getTokenText() {

	return mToken;

}

unsigned int SGMLScanner :: getTokenStartIndex() const {

	return mTokenStartIndex;
}

unsigned int SGMLScanner :: getIndex() const {

	return mText.getIndex();

}

unsigned int SGMLScanner :: getLineNr() const {

	return mText.getLineNr();

}

unsigned int SGMLScanner :: getCharNr() const {

	return mText.getCharNr();

}

State SGMLScanner :: saveState() {

	return mText.saveState();

}


void SGMLScanner :: restoreState(const State & aState) {
	
	mText.restoreState(aState);
	mToken = NONE_SYM;
	mLookAheadToken = NONE_SYM;
	mChar = mText.getChar();

}
