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

/*	CSSParser implementation
	See CSSParser.hpp for more information
	
	Also includes main()
*/

// Standard C headers
#include <stdio.h>

// CSSParser headers
#include "CSSParser.hpp"
#include "CSSScanner.hpp"
#include "ReadException.hpp"
#include "ImportAtRuleParser.hpp"
#include "MediaAtRuleParser.hpp"
#include "RuleSetParser.hpp"

// DOM Style headers
#include "CSSStyleSheet.hpp"
#include "CSSRuleList.hpp"
#include "CSSRule.hpp"
#include "CSSStyleDeclaration.hpp"

CSSParser :: CSSParser() {
	
	mScanner = NULL;
	mImportAtRuleParser = NULL;
	mMediaAtRuleParser = NULL;
	mRuleSetParser = NULL;

}

CSSParser :: ~CSSParser() {
	
	delete mScanner;
	delete mImportAtRuleParser;
	delete mMediaAtRuleParser;
	delete mRuleSetParser;

}

void CSSParser :: showDeclaration(CSSStyleRulePtr aRule, unsigned int aSpacing) {

	CSSStyleDeclarationPtr style = aRule->getStyle();
	unsigned long length = style->getLength();
	for (unsigned long i = 0; i < length; i++) {
		TDOMString name = style->item(i);
		for (unsigned int j = 0; j < aSpacing; j++) {
			printf("  ");
		}
		printf("Property: %s", name.c_str());
		printf(": %s", style->getPropertyValue(name).c_str());
		TDOMString priority = style->getPropertyPriority(name);
		if (priority != "") {
			printf("; Priority: %s\n", priority.c_str());
		}
		else {
			printf("\n");
		}
	}
}

void CSSParser :: showStyleRule(CSSStyleRulePtr aRule) {

	printf("Style rule\n");
	printf("  Selector text: %s\n", aRule->getSelectorText().c_str());
	printf("  Declaration\n");
	showDeclaration(aRule, 2);

}

void CSSParser :: showMediaRule(CSSMediaRulePtr aRule) {

	MediaListPtr media = aRule->getMedia();
	unsigned long length = media->getLength();
	printf("  Media\n");
	for (unsigned long i = 0; i < length; i++) {
		TDOMString medium = media->item(i);
		printf("    %s\n", medium.c_str());
	}
	CSSRuleListPtr list = aRule->getCSSRules();
	printf("  Rules\n");
	length = list->getLength();
	for (unsigned long i = 0; i < length; i++) {
		CSSRulePtr rule = list->item(i);
		CSSStyleRulePtr styleRule = shared_static_cast<CSSStyleRule>(rule);
		printf("    Selector text: %s\n", styleRule->getSelectorText().c_str());
		printf("    Declaration\n");
		showDeclaration(styleRule, 4);
	}

}

void CSSParser :: showStyleSheet() {
	
	CSSRuleListPtr list = mStyleSheet->getCSSRules();
	unsigned long length = list->getLength();
	for (unsigned long i = 0; i < length; i++) {
		CSSRulePtr rule = list->item(i);
		switch (rule->getType()) {
			case CSSRule::STYLE_RULE: {
				CSSStyleRulePtr styleRule = shared_static_cast<CSSStyleRule>(rule);
				printf("Style rule\n");
				printf("  Selector text: %s\n", styleRule->getSelectorText().c_str());
				printf("  Declaration\n");
				showDeclaration(styleRule, 2);
				break;
			}
			case CSSRule::MEDIA_RULE: {
				CSSMediaRulePtr mediaRule = shared_static_cast<CSSMediaRule>(rule);
				printf("Media rule\n");
				showMediaRule(mediaRule);
				break;
			}
			default: {
				printf("Unknown rule found\n");
			}
		}
	}
}

void CSSParser :: parseDocument() {


	// Create a stylesheet to store the information.
	mStyleSheet = CSSStyleSheetPtr(new CSSStyleSheet());

	// Setup the parsers
	mImportAtRuleParser = new ImportAtRuleParser(mScanner, mStyleSheet);
	mMediaAtRuleParser = new MediaAtRuleParser(mScanner, mStyleSheet);
	mRuleSetParser = new RuleSetParser(mScanner, mStyleSheet);

	// See if we can scan the whole CSS document
	try {
		Token token = mScanner->nextToken();
		while (token != EOF_SYM && token != NONE_SYM) {
			switch (token) {
				case ATKEYWORD_SYM: {
					if (mScanner->getTokenText() == "import") {
						token = mImportAtRuleParser->parse(token);
					}
					else if (mScanner->getTokenText() == "media") {
						token = mMediaAtRuleParser->parse(token);
					}
					else {
						printf("Unknown at keyword found: %s. Skipping...\n", mScanner->getTokenText().c_str());
						token = mScanner->nextToken();
					}
					break;
				}
				case COMMENT_SYM: {
					// No need to do anything with that.
					token = mScanner->nextToken();
					break;
				}
				case SPACE_SYM: {
					// No need to do anything with that.
					token = mScanner->nextToken();
					break;
				}
				default: {
					if (mRuleSetParser->isRuleSet(token)) {
						token = mRuleSetParser->parse(token);
					}
					else {
						printf("Found token: %s\n", mScanner->getTokenText().c_str());
						token = mScanner->nextToken();
					}
				}
			}
		}
	}
	catch(ReadException r) {
		printf(
			"Found error: line %i char %i message: %s\n",
			r.getLineNr(),
			r.getCharNr(),
			r.getErrorMessage().c_str());
	}

	// Done parsing. Show the style sheet.
	showStyleSheet();

}

CSSStyleSheetPtr CSSParser :: parse(const char * aDocument) {
	
	mScanner = new CSSScanner(aDocument);
	parseDocument();

	return mStyleSheet;
}

CSSStyleSheetPtr CSSParser :: parse(string aDocument) {

	mScanner = new CSSScanner();
	mScanner->setDocument(aDocument);
	parseDocument();

	return mStyleSheet;
}

int main( int argc, char * argv[] )	{
	
	if ( argc < 2 )	{
		printf("Please supply a css document to load\n");
		return 1;
	}
	
	CSSParser parser;
	parser.parse(argv[1]);

	return 0;
	
}
