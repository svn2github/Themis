#ifndef AT_RULE_PARSER_H
#define AT_RULE_PARSER_H
/* AtRuleParser.h - reads and parses CSS at-rules
**
** Copyright 2001, pinc Software. All Rights Reserved.
*/

#include "Types.h"
#include "StyleSheet.h"

class StyleSheetParser;

enum rules
{
	SKIP_RULE	= -1,
	NO_RULE		= 0,

	CHARSET_RULE,
	IMPORT_RULE,
	MEDIA_RULE,
	PAGE_RULE,
	FONTFACE_RULE
};


class AtRuleParser
{
	public:
		static status_t Parse(StyleSheetParser *parser);
		static status_t BlockEnd(StyleSheetParser *parser,int32 type);

	private:
		static status_t ParseCharset(StyleSheetParser *parser);
		static status_t ParseImport(StyleSheetParser *parser);
		static status_t ParseMedia(StyleSheetParser *parser);
		static status_t ParsePage(StyleSheetParser *parser);
		static status_t ParseFontFace(StyleSheetParser *parser);
};

#endif	/* AT_RULE_PARSER */
