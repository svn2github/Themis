/* AtRuleParser.cpp - reads and parses CSS at-rules
**
** Copyright 2001, pinc Software. All Rights Reserved.
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "AtRuleParser.h"
#include "PropertyParser.h"
#include "StyleSheetParser.h"
#include "StyleSheet.h"

const char *mediaTypes[] = {"all","aural","braille","embossed","handheld","print",
							"projection","screen","tty","tv"};
#define NUM_MEDIA_TYPES 10


status_t
AtRuleParser::ParseCharset(StyleSheetParser */*parser*/)
{
	return RC_OK;
}


status_t
AtRuleParser::ParseImport(StyleSheetParser *parser)
{
	char *import;
	bool url = true;

	if (PropertyParser::ParseURL(parser,&import) < RC_OK)
	{
		if (PropertyParser::ParseString(parser,&import) < RC_OK)
			return INVALID_ARGUMENT;

		url = false;
	}
	if (parser->NextToken() < RC_OK || *parser->fCurrentChar != ';')
		return INVALID_ARGUMENT;

	/* do something! */
	printf("import %s: \"%s\"\n",url ? "url" : "file",import);

	return RC_OK;
}


status_t
AtRuleParser::ParseMedia(StyleSheetParser *parser)
{
	bool comma = false;
	bool first = true;
	status_t status;
	
	do
	{
		char *type;
		status = parser->ParseName(&type);
		if (status >= RC_OK)
		{
			parser->AddMedia(type);
			free(type);
			comma = false;

			status = parser->NextToken();
			if (status >= RC_OK)
			{
				if (*parser->fCurrentChar == ',')
				{
					comma = true;
					parser->fCurrentChar++;
					status = parser->NextToken();
				}
				first = false;
			}
		}
	} while (status == RC_OK);

	if (first || comma)
		parser->Error("invalid media type definition");

	parser->fBeginAtRule = MEDIA_RULE_BLOCK;
	return RC_OK;
}


status_t
AtRuleParser::ParsePage(StyleSheetParser *parser)
{
	parser->fBeginAtRule = PAGE_RULE_BLOCK;
	return RC_OK;
}


status_t
AtRuleParser::ParseFontFace(StyleSheetParser *parser)
{
	parser->fBeginAtRule = FONTFACE_RULE_BLOCK;
	return RC_OK;
}


// #pragma mark -


status_t
AtRuleParser::BlockEnd(StyleSheetParser *parser,int32 type)
{
	switch (type)
	{
		case MEDIA_RULE_BLOCK:
			parser->MakeEmptyMedia();
			break;
	}
	parser->fBeginAtRule = NO_RULE;
	return RC_OK;
}


status_t
AtRuleParser::Parse(StyleSheetParser *parser)
{
	char rule[32];
	{
		char *tempRule;
		if (PropertyParser::ParseKeyword(parser,&tempRule) < RC_OK)
			return INVALID_SYMBOL;

		strncpy(rule,tempRule,32);
		free(tempRule);
	}
	if (parser->NextToken() < RC_OK)
		return INVALID_SYMBOL;

	status_t rc = INVALID_SYMBOL;

	if (!strcasecmp(rule,"import"))
	{
		if (parser->fImportAllowed)
			return ParseImport(parser);

		rc = INVALID_RULE_LOCATION;
	}
	else if (!strcasecmp(rule,"media"))
		return ParseMedia(parser);
	else if (!strcasecmp(rule,"page"))
		return ParsePage(parser);
	else if (!strcasecmp(rule,"font-face"))
		return ParseFontFace(parser);
	else if (!strcasecmp(rule,"charset"))
	{
		if (parser->fLine == 1)
			return ParseCharset(parser);

		rc = INVALID_RULE_LOCATION;
	}

	if (rc < RC_OK)
		parser->fBeginAtRule = UNKNOWN_RULE_BLOCK;

	/* skip rest of line */
	parser->SkipLine("{;");

	return rc;
}

