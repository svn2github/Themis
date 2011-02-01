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
	Class Start Date: November 5, 2009
*/

/*	RuleSetParser implementation
	See RuleSetParser.hpp for more information
	
*/

// Standard C headers
#include <stdio.h>

// CSSParser headers
#include "RuleSetParser.hpp"
#include "ReadException.hpp"

// DOM Style headers
#include "CSSStyleRule.hpp"
#include "CSSStyleDeclaration.hpp"
#include "CSSRuleList.hpp"
#include "CSSMediaRule.hpp"
#include "CSSValue.hpp"
#include "CSSPrimitiveValue.hpp"
#include "RGBColor.hpp"

RuleSetParser :: RuleSetParser(CSSScanner * aScanner,
							   CSSStyleSheetPtr aStyleSheet)
			  : BaseParser(aScanner, aStyleSheet) {
	
}

RuleSetParser :: ~RuleSetParser() {

}

ValueType RuleSetParser :: getValueTypeOfProperty(TDOMString aName) const {
	
	ValueType result;
	
	if (aName == "color") {
		result = COLOR_TYPE;
	}
	else {
		result = UNKNOWN_TYPE;
	}
	
	return result;
	
}

TDOMString RuleSetParser :: parseHash() {
	
	TDOMString hash = mScanner->getTokenText();
	
	mToken = mScanner->nextToken();
	if (mToken == IDENTIFIER_SYM) {
		hash += mScanner->getTokenText();
		mToken = mScanner->nextToken();
	}
	else {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Expected hash",
							GENERIC);
	}
	
	return hash;
}

TDOMString RuleSetParser :: parseClass() {
	
	TDOMString classString = mScanner->getTokenText();
	
	mToken = mScanner->nextToken();
	if (mToken == IDENTIFIER_SYM) {
		classString += mScanner->getTokenText();
		mToken = mScanner->nextToken();
	}
	else {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Expected class",
							GENERIC);
	}
	
	return classString;

}

TDOMString RuleSetParser :: parseAttrib() {
	
	TDOMString attrib = mScanner->getTokenText();
	
	mToken = mScanner->nextToken();
	parseSStar();
	if (mToken == IDENTIFIER_SYM) {
		attrib += mScanner->getTokenText();
		mToken = mScanner->nextToken();
		parseSStar();
		bool attribMetaFound = true;
		switch (mToken) {
			case EQUALS_SYM: {
				attrib += mScanner->getTokenText();
				mToken = mScanner->nextToken();
				break;
			}
			case INCLUDES_SYM: {
				attrib += mScanner->getTokenText();
				mToken = mScanner->nextToken();
				break;
			}
			case DASHMATCH_SYM: {
				attrib += mScanner->getTokenText();
				mToken = mScanner->nextToken();
				break;
			}
			default: {
				attribMetaFound = false;
			}
		}
		if (attribMetaFound) {
			parseSStar();
			switch (mToken) {
				case IDENTIFIER_SYM: {
					attrib += mScanner->getTokenText();
					mToken = mScanner->nextToken();
					break;
				}
				case TEXT_SYM: {
					attrib += mScanner->getTokenText();
					mToken = mScanner->nextToken();
					break;
				}
				default: {
					throw ReadException(mScanner->getLineNr(),
										mScanner->getCharNr(),
										"Expected identifier or text",
										GENERIC);
				}
			}
		}
		if (mToken == RBRACKET_SYM) {
			attrib += mScanner->getTokenText();
			mToken = mScanner->nextToken();
		}
		else {
			throw ReadException(mScanner->getLineNr(),
								mScanner->getCharNr(),
								"Expected ]",
								GENERIC);
		}
	}
	else {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Expected attrib",
							GENERIC);
	}
	
	return attrib;
	
}

TDOMString RuleSetParser :: parseFunction() {

	TDOMString function = mScanner->getTokenText();

	parseSStar();
	/*function += */parseExpression();
	if (mToken == RROUNDBRACKET_SYM) {
		function += mScanner->getTokenText();
		mToken = mScanner->nextToken();
		parseSStar();
	}
	else {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Expected )",
							GENERIC);
	}

	return function;

}

TDOMString RuleSetParser :: parsePseudoFunction() {

	TDOMString function = mScanner->getTokenText();

	parseSStar();
	mToken = mScanner->nextToken();
	if (mToken == IDENTIFIER_SYM) {
		function += mScanner->getTokenText();
		if (mToken == RROUNDBRACKET_SYM) {
			function += mScanner->getTokenText();
			mToken = mScanner->nextToken();
			parseSStar();
		}
		else {
			throw ReadException(mScanner->getLineNr(),
								mScanner->getCharNr(),
								"Expected )",
								GENERIC);
		}
	}
	else {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Expected identifier",
							GENERIC);
	}

	return function;
}

TDOMString RuleSetParser :: parsePseudo() {

	TDOMString pseudo = mScanner->getTokenText();

	mToken = mScanner->nextToken();
	switch (mToken) {
		case IDENTIFIER_SYM: {
			pseudo += mScanner->getTokenText();
			mToken = mScanner->nextToken();
			break;
		}
		case FUNCTION_SYM: {
			pseudo += mScanner->getTokenText();
			mToken = mScanner->nextToken();
			pseudo += parsePseudoFunction();
			break;
		}
		default: {
			throw ReadException(mScanner->getLineNr(),
								mScanner->getCharNr(),
								"Expected pseudo",
								GENERIC);
		}
	}
	
	return pseudo;
}


TDOMString RuleSetParser :: parseSimpleSelector() {

	TDOMString selector = "";

	if (mToken == IDENTIFIER_SYM ||
		mToken == STAR_SYM) {
		selector = mScanner->getTokenText();
		mToken = mScanner->nextToken();
		bool selectorMetaFound = true;
		while (selectorMetaFound) {
			switch (mToken) {
				case HASH_SYM: {
					selector += parseHash();
					break;
				}
				case DOT_SYM: {
					selector += parseClass();
					break;
				}
				case LBRACKET_SYM: {
					selector += parseAttrib();
					break;
				}
				case COLON_SYM: {
					selector += parsePseudo();
					break;
				}
				default: {
					// Found something else.
					selectorMetaFound = false;
				}
			}
		}
	}
	else {
		// Need to find at least once.
		bool selectorMetaFound = false;
		do {
			switch (mToken) {
				case HASH_SYM: {
					selector += parseHash();
					selectorMetaFound = true;
					break;
				}
				case DOT_SYM: {
					selector += parseClass();
					selectorMetaFound = true;
					break;
				}
				case LBRACKET_SYM: {
					selector += parseAttrib();
					selectorMetaFound = true;
					break;
				}
				case COLON_SYM: {
					selector += parsePseudo();
					selectorMetaFound = true;
					break;
				}
				default: {
					// Found something else.
					selectorMetaFound = false;
				}
			}
		}
		while (selectorMetaFound);
		if (selectorMetaFound) {
			throw ReadException(mScanner->getLineNr(),
								mScanner->getCharNr(),
								"Expected selector",
								GENERIC);
		}
	}
	
	return selector;

}

TDOMString RuleSetParser :: parseSelector() {
	
	TDOMString selector = parseSimpleSelector();
	
	switch (mToken) {
		case PLUS_SYM: {
			selector += mScanner->getTokenText();
			mToken = mScanner->nextToken();
			parseSStar();
			selector += parseSelector();
			break;
		}
		case GREATERTHAN_SYM: {
			selector += mScanner->getTokenText();
			mToken = mScanner->nextToken();
			parseSStar();
			selector += parseSelector();
			break;
		}
		case SPACE_SYM: {
			selector += mScanner->getTokenText();
			mToken = mScanner->nextToken();
			parseSStar();
			switch (mToken) {
				case PLUS_SYM: {
					selector += mScanner->getTokenText();
					mToken = mScanner->nextToken();
					parseSStar();
					break;
				}
				case GREATERTHAN_SYM: {
					selector += mScanner->getTokenText();
					mToken = mScanner->nextToken();
					parseSStar();
					break;
				}
				default: {
					// Found something else. Do nothing.
				}
			}
			selector += parseSelector();
			break;
		}
		default: {
			// Found something else. Do nothing.
		}
	}
	
	return selector;
	
}

CSSValuePtr RuleSetParser :: parseNumericTerm(bool aPositiveValue) {

	TDOMString numericTerm = "";
	unsigned short primitiveType;
	float value = 0;

	switch (mToken) {
		case NUMBER_SYM: {
			numericTerm = mScanner->getTokenText();
			mToken = mScanner->nextToken();
			if (mToken == IDENTIFIER_SYM) {
				string tokenText = mScanner->getTokenText();
				if (tokenText == "px") {
					primitiveType = CSSPrimitiveValue::CSS_PX;
				}
				else if (tokenText == "cm") {
					primitiveType = CSSPrimitiveValue::CSS_CM;
				}
				else if (tokenText == "mm") {
					primitiveType = CSSPrimitiveValue::CSS_MM;
				}
				else if (tokenText == "in") {
					primitiveType = CSSPrimitiveValue::CSS_IN;
				}
				else if (tokenText == "pt") {
					primitiveType = CSSPrimitiveValue::CSS_PT;
				}
				else if (tokenText == "pc") {
					primitiveType = CSSPrimitiveValue::CSS_PC;
				}
				else if (tokenText == "em") {
					primitiveType = CSSPrimitiveValue::CSS_EMS;
				}
				else if (tokenText == "ex") {
					primitiveType = CSSPrimitiveValue::CSS_EXS;
				}
				else if (tokenText == "deg") {
					primitiveType = CSSPrimitiveValue::CSS_DEG;
				}
				else if (tokenText == "rad") {
					primitiveType = CSSPrimitiveValue::CSS_RAD;
				}
				else if (tokenText == "grad") {
					primitiveType = CSSPrimitiveValue::CSS_GRAD;
				}
				else if (tokenText == "ms") {
					primitiveType = CSSPrimitiveValue::CSS_MS;
				}
				else if (tokenText == "s") {
					primitiveType = CSSPrimitiveValue::CSS_S;
				}
				else if (tokenText == "hz") {
					primitiveType = CSSPrimitiveValue::CSS_HZ;
				}
				else if (tokenText == "khz") {
					primitiveType = CSSPrimitiveValue::CSS_KHZ;
				}
				else {
					throw ReadException(mScanner->getLineNr(),
										mScanner->getCharNr(),
										"Expected term",
										GENERIC);
				}
				mToken = mScanner->nextToken();
				parseSStar();
			}
			else {
				// Just a number is also possible.
				primitiveType = CSSPrimitiveValue::CSS_NUMBER;
				parseSStar();
			}
			value = atof(numericTerm.c_str());
			break;
		}
		case PERCENTAGE_SYM: {
			numericTerm = mScanner->getTokenText();
			primitiveType = CSSPrimitiveValue::CSS_PERCENTAGE;
			mToken = mScanner->nextToken();
			value = atof(numericTerm.c_str());
			break;
		}
		default: {
			throw ReadException(mScanner->getLineNr(),
								mScanner->getCharNr(),
								"Expected term",
								GENERIC);
		}
	}
	
	if (!aPositiveValue) {
		// Multiply by -1 to get a negative value.
		value *= -1;
	}
	
	CSSPrimitiveValuePtr result = CSSPrimitiveValuePtr(new CSSPrimitiveValue(value, primitiveType));
	
	return result;

}

CSSValuePtr RuleSetParser :: parseTerm(ValueType aType) {

	TDOMString term = "";
	CSSValuePtr result;

	if (mToken == MINUS_SYM) {
		mToken = mScanner->nextToken();
		result = parseNumericTerm(false);
	}
	else if (mToken == PLUS_SYM) {
		mToken = mScanner->nextToken();
		 result = parseNumericTerm();
	}
	else {
		switch (mToken) {
			case NUMBER_SYM:
			case PERCENTAGE_SYM: {
				result = parseNumericTerm();
				break;
			}
			case TEXT_SYM: {
				term = mScanner->getTokenText();
				mToken = mScanner->nextToken();
				parseSStar();
				result = CSSPrimitiveValuePtr(new CSSPrimitiveValue(term, CSSPrimitiveValue::CSS_STRING));
				break;
			}
			case IDENTIFIER_SYM: {
				term = mScanner->getTokenText();
				mToken = mScanner->nextToken();
				parseSStar();
				switch(aType) {
					case COLOR_TYPE: {
						RGBColorPtr colorPtr = RGBColorPtr(new RGBColor(term));
						result = CSSPrimitiveValuePtr(new CSSPrimitiveValue(colorPtr));
						break;
					}
					default: {
						result = CSSPrimitiveValuePtr(new CSSPrimitiveValue(term, CSSPrimitiveValue::CSS_IDENT));
					}
				}
				break;
			}
			case URI_SYM: {
				term = mScanner->getTokenText();
				mToken = mScanner->nextToken();
				parseSStar();
				result = CSSPrimitiveValuePtr(new CSSPrimitiveValue(term, CSSPrimitiveValue::CSS_URI));
				break;
			}
			case HASH_SYM: {
				term = mScanner->getTokenText();
				mToken = mScanner->nextToken();
				parseHash();
				break;
			}
			case FUNCTION_SYM: {
				term = mScanner->getTokenText();
				mToken = mScanner->nextToken();
				term += parseFunction();
				break;
			}
			default: {
				throw ReadException(mScanner->getLineNr(),
									mScanner->getCharNr(),
									"Expected term",
									GENERIC);
			}
				
		}
	}

	return result;

}

bool RuleSetParser :: isTerm() {
	
	bool termFound = false;
	
	switch (mToken) {
		case MINUS_SYM:
		case PLUS_SYM:
		case NUMBER_SYM:
		case PERCENTAGE_SYM:
		case TEXT_SYM:
		case IDENTIFIER_SYM:
		case URI_SYM:
		case HASH_SYM:
		case FUNCTION_SYM: {
			termFound = true;
			break;
		}
		default: {
			// Returning false by default.
		}
	}
	
	return termFound;
}

CSSValuePtr RuleSetParser :: parseExpression(ValueType aType) {
	
	CSSValuePtr result = parseTerm(aType);
	
	bool termFound = true;
	while (termFound) {
		if (mToken == SLASH_SYM ||
			mToken == COMMA_SYM) {
			/*expression +=*/ mScanner->getTokenText();
			mToken = mScanner->nextToken();
			parseSStar();
			/*expression +=*/ parseTerm();
		}
		else if (isTerm()) {
			/*expression +=*/ parseTerm();
		}
		else {
			termFound = false;
		}
	}

	return result;

}

Property RuleSetParser :: parseDeclaration() {

	TDOMString name = mScanner->getTokenText();
	CSSValuePtr value;
	TDOMString important = "";
	ValueType type = getValueTypeOfProperty(name);
	
	mToken = mScanner->nextToken();
	parseSStar();
	if (mToken == COLON_SYM) {
		mToken = mScanner->nextToken();
		parseSStar();
		value = parseExpression(type);
		if (mToken == IMPORTANT_SYM) {
			mToken = mScanner->nextToken();
			parseSStar();
		}
	}
	else {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Expected :",
							GENERIC);
	}
	
	Property result(name, value, important);
	
	return result;
	
}

bool RuleSetParser :: isRuleSet(Token aToken) {

	if (aToken == IDENTIFIER_SYM ||
		aToken == STAR_SYM ||
		aToken == HASH_SYM ||
		aToken == DOT_SYM ||
		aToken == LBRACKET_SYM ||
		aToken == COLON_SYM) {
		return true;
	}
	else {
		return false;
	}
	
}

Token RuleSetParser :: parse(Token aToken, CSSRulePtr aParentRule) {
	
	mToken = aToken;
	TDOMString selector = parseSelector();
	
	bool selectorFound = true;
	while (selectorFound) {
		if (mToken == COMMA_SYM) {
			selector += mScanner->getTokenText();
			mToken = mScanner->nextToken();
			parseSStar();
			selector += parseSelector();
		}
		else {
			selectorFound = false;
		}
	}
	
	CSSStyleRulePtr rule(new CSSStyleRule(mStyleSheet, aParentRule, selector));
	if (aParentRule.get() != NULL) {
		// Attach it to the parent.
		if (aParentRule->getType() == CSSRule::MEDIA_RULE) {
			CSSMediaRulePtr mediaRule = shared_static_cast<CSSMediaRule>(aParentRule);
			mediaRule->insertRule(rule, mediaRule->getCSSRules()->getLength());
		}
	}
	else {
		// Attach it directly to the stylesheet.
		mStyleSheet->insertRule(rule, mStyleSheet->getCSSRules()->getLength());
	}
	
	if (mToken == LCURLYBRACKET_SYM) {
		mToken = mScanner->nextToken();
		parseSStar();
		CSSStyleDeclarationPtr style(new CSSStyleDeclaration(rule));
		if (mToken == IDENTIFIER_SYM) {
			Property prop = parseDeclaration();
			style->setPropertyCSSValue(prop.getName(), prop.getValue(), prop.getPriority());
			// Found at least one property, so there is a style to set.
			rule->setStyle(style);
		}
		
		bool declarationFound = true;
		while (declarationFound) {
			if (mToken == SEMICOLON_SYM) {
				mToken = mScanner->nextToken();
				parseSStar();
				if (mToken == IDENTIFIER_SYM) {
					Property prop = parseDeclaration();
					style->setPropertyCSSValue(prop.getName(),
											   prop.getValue(),
											   prop.getPriority());
				}
			}
			else {
				declarationFound = false;
			}
		}
		
		if (mToken == RCURLYBRACKET_SYM) {
			mToken = mScanner->nextToken();
		}
		else {
			throw ReadException(mScanner->getLineNr(),
								mScanner->getCharNr(),
								"Expected }",
								GENERIC);
		}
		parseSStar();

	}
	else {
		throw ReadException(mScanner->getLineNr(),
							mScanner->getCharNr(),
							"Expected {",
							GENERIC);
	}
	
	return mToken;
}
