#ifndef STYLE_SHEET_PARSER_H
#define STYLE_SHEET_PARSER_H
/* StyleSheetParser.h - reads and parses CSS files, builds Style tables
**
** Copyright 2001, pinc Software. All Rights Reserved.
*/

#include "Types.h"
#include "StyleSheet.h"
#include "Properties.h"

class InputStream;
class PropertyParser;
class AtRuleParser;

enum
{
	INVALID_SYMBOL = RC_ERROR_BASE + 0x10000,
	INVALID_RULE_LOCATION,
	INVALID_COMMENTS,
	INVALID_ARGUMENT,
	INVALID_IDENTIFIER,
	INVALID_SELECTOR,
	INVALID_SELECTOR_CLASS,
	UNKNOWN_PROPERTY,
	UNHANDLED_PROPERTY,
	UNKNOWN_KEYWORD,
	UNKNOWN_COLOR_KEYWORD,
	INVALID_VALUE,
	STRING_EXCEEDS_LINE
};

#define MAX_BLOCKS 16

class StyleSheetParser
{
	public:
		StyleSheetParser(StyleSheet *styleSheet,InputStream *input);

		status_t InitCheck();
		status_t Parse();

	private:
		friend class PropertyParser;
		friend class AtRuleParser;

		void Error(char *text);
		void Error(status_t rc);
		void Warning(char *text);

		status_t ReadLine();
		status_t NextToken();
		status_t SkipString();
		status_t SkipLine(char *delimiter);
		status_t ParseName(char **name,bool firstChar = true);

		status_t ParseSimpleSelector(SimpleSelector **s);
		status_t ParseSelector();
		status_t CheckForPropertyEnd(bool *important);
		status_t ParseProperty();
		
		void AddMedia(char *mediaType);
		void MakeEmptyMedia();

		StyleSheet	*fStyleSheet;
		InputStream	*fInput;
		status_t	fStatus;
		char		*fCurrentChar;
		int32		fLine;
		Selector	*fCurrentSelector;
		Properties	*fProperties;
		Flags		fLastPropertyFlags;
		List		fSelectors;
		List		fMediaTypes;
		bool		fImportAllowed;
		bool		fInComment;
		uint8		fBlockType[MAX_BLOCKS];
		uint32		fInBlock;
		int32		fBeginAtRule;
};

enum block_type
{
	SELECTOR_BLOCK	= 1,
	MEDIA_RULE_BLOCK,
	PAGE_RULE_BLOCK,
	FONTFACE_RULE_BLOCK,
	UNKNOWN_RULE_BLOCK,
};

extern "C" struct keywordProperty
{
	const char **keywords;
	int32 numKeywords;
};
extern "C" const struct keywordProperty keywordProperties[];
extern "C" const char *keywordsBorderStyle[];
#define NUM_KEYWORDS_BORDER_STYLE 10

enum keyword_properties
{
	BACKGROUND_ATTACH_KEYWORDS = 0,
	BACKGROUND_REPEAT_KEYWORDS,
	BORDER_COLLAPSE_KEYWORDS,
	CAPTION_SIDE_KEYWORDS,
	CLEAR_KEYWORDS,
	DIRECTION_KEYWORDS,
	DISPLAY_KEYWORDS,
	EMPTY_CELLS_KEYWORDS,
	FLOAT_KEYWORDS,
	FONT_STRETCH_KEYWORDS,
	FONT_STYLE_KEYWORDS,
	FONT_VARIANT_KEYWORDS,
	LIST_STYLE_POSITION_KEYWORDS,
	LIST_STYLE_TYPE_KEYWORDS,
	OUTLINE_STYLE_KEYWORDS,
	OVERFLOW_KEYWORDS,
	PAGE_BREAK_KEYWORDS,
	PAGE_BREAK_INSIDE_KEYWORDS,
	POSITION_KEYWORDS,
	SPEAK_KEYWORDS,
	SPEAK_HEADER_KEYWORDS,
	SPEAK_NUMERAL_KEYWORDS,
	SPEAK_PUNCTUATION_KEYWORDS,
	TABLE_LAYOUT_KEYWORDS,
	TEXT_TRANSFORM_KEYWORDS,
	UNICODE_BIDI_KEYWORDS,
	VISIBILITY_KEYWORDS,
	WHITE_SPACE_KEYWORDS
};

extern bool IsWhiteSpace(char c);
extern bool IsStringChar(char c);
extern bool IsNameStart(char c);
extern bool IsNameChar(char c);
extern char *strndup(char *string, int length);
extern int cmpName(const char *name,const char **ident);

#endif	/* STYLE_SHEET_PARSER_H */
