/* StyleSheetParser.cpp - reads and parses CSS files, builds Style tables
**
** Copyright 2001, pinc Software. All Rights Reserved.
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "StyleSheetParser.h"
#include "StyleSheet.h"
#include "PropertyParser.h"
#include "AtRuleParser.h"
#include "InputStream.h"

#define BUFFER_SIZE 2048

#define NO_BLOCK (~0L)

// all known pseudo classes and elements

struct pseudo_class {
	char *name;
	uint32 flag;
};

const struct pseudo_class pseudoClasses[] = {
	{"first-child",	SELECTOR_FIRST_CHILD},
	{"link",		SELECTOR_LINK},
	{"visited",		SELECTOR_VISITED},
	{"hover",		SELECTOR_HOVER},
	{"active",		SELECTOR_ACTIVE},
	{"focus",		SELECTOR_FOCUS},
	{"lang",		SELECTOR_LANG},
	{"first-line",	SELECTOR_FIRST_LINE},
	{"first-letter",SELECTOR_FIRST_LETTER},
	{"before",		SELECTOR_BEFORE},
	{"after",		SELECTOR_AFTER},
};

#define NUM_PSEUDO 11

// keyword properties

const char *keywordsBackgroundRepeat[] = {"no-repeat","repeat-x","repeat-y","repeat"};
const char *keywordsBackgroundAttach[] = {"scroll","fixed"};
const char *keywordsBorderCollapse[] = {"collapse","separate"};
const char *keywordsBorderStyle[] = {"none","hidden","dotted","dashed","solid","double","groove","ridge","inset","outset"};
const char *keywordsCaptionSide[] = {"top","bottom","left","right"};
const char *keywordsClear[] = {"none","left","right","both"};
const char *keywordsDirection[] = {"ltr","rtl"};
const char *keywordsDisplay[] = {"none","inline","block","list-item","run-in","compact",
	"marker","table","inline-table","table-row-group","table-header-group","table-footer-group",
	"table-row","table-column-group","table-column","table-cell","table-caption"};
const char *keywordsEmptyCells[] = {"show","hide"};
const char *keywordsFloat[] = {"none","left","right"};
const char *keywordsFontStretch[] = {"normal","wider","narrower","ultra-condensed",
	"extra-condensed","condensed","semi-condensed","semi-expanded","expanded",
	"extra-expanded","ultra-expanded"};
const char *keywordsFontStyle[] = {"normal","italic","oblique"};
const char *keywordsFontVariant[] = {"normal","small-caps"};
const char *keywordsListStylePosition[] = {"inside","outside"};
const char *keywordsListStyleType[] = {"none","disc","circle","square","decimal",
	"decimal-leading-zero","lower-roman","upper-roman","lower-greek","lower-alpha",
	"lower-latin","upper-alpha","upper-latin","hebrew","armenian","georgian",
	"cjk-ideographic","hiragana","katakana","hiragana-iroha","katakana-iroha"};
const char *keywordsOverflow[] = {"visible","hidden","scroll","auto"};
const char *keywordsPageBreak[] = {"auto","always","avoid","left","right"};
const char *keywordsPageBreakInside[] = {"auto","avoid"};
const char *keywordsPosition[] = {"static","relative","absolute","fixed"};
const char *keywordsSpeak[] = {"normal","none","spell-out"};
const char *keywordsSpeakHeader[] = {"once","always"};
const char *keywordsSpeakNumeral[] = {"continuous","digits"};
const char *keywordsSpeakPunctuation[] = {"none","code"};
const char *keywordsTableLayout[] = {"auto","fixed"};
const char *keywordsTextTransform[] = {"none","capitalize","lowercase","uppercase","none","capitalized","lowercased","uppercased"};
const char *keywordsUnicodeBidi[] = {"normal","embed","bidi-override"};
const char *keywordsVisibility[] = {"visible","hidden","collapse"};
const char *keywordsWhiteSpace[] = {"normal","pre","nowrap"};

const struct keywordProperty keywordProperties[] = {
	{keywordsBackgroundAttach,2},
	{keywordsBackgroundRepeat,4},
	{keywordsBorderCollapse,2},
	{keywordsCaptionSide,4},
	{keywordsClear,4},
	{keywordsDirection,2},
	{keywordsDisplay,17},
	{keywordsEmptyCells,2},
	{keywordsFloat,3},
	{keywordsFontStretch,11},
	{keywordsFontStyle,3},
	{keywordsFontVariant,2},
	{keywordsListStylePosition,2},
	{keywordsListStyleType,21},
	{keywordsBorderStyle,NUM_KEYWORDS_BORDER_STYLE},
	{keywordsOverflow,4},
	{keywordsPageBreak,5},
	{keywordsPageBreakInside,2},
	{keywordsPosition,4},
	{keywordsSpeak,3},
	{keywordsSpeakHeader,2},
	{keywordsSpeakNumeral,2},
	{keywordsSpeakPunctuation,2},
	{keywordsTableLayout,2},
	{keywordsTextTransform,8},
	{keywordsUnicodeBidi,3},
	{keywordsVisibility,3},
	{keywordsWhiteSpace,3},
};

// all known properties (and how they are parsed)

struct property {
	char *name;
	uint8 type;
	uint8 keywords;
	status_t (*parseFunction)(StyleSheetParser *parser,uint32 property);
};

#define PROPERTY_FUNCTION_TYPE 0
#define PROPERTY_KEYWORD_TYPE 1

const struct property properties[] = {
	/* aural */ {"azimuth",PROPERTY_FUNCTION_TYPE,0,NULL},
	{"background",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseBackgroundProperty},
	{"background-attachment",PROPERTY_KEYWORD_TYPE,BACKGROUND_ATTACH_KEYWORDS,NULL},
	{"background-color",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseBackgroundColorProperty},
	{"background-image",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseURINoneProperty},
	{"background-position",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseBackgroundPositionProperty},
	{"background-repeat",PROPERTY_KEYWORD_TYPE,BACKGROUND_REPEAT_KEYWORDS,NULL},
	{"border",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseBorderProperty},
	{"border-bottom",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseBorderProperty},
	{"border-bottom-color",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseBorderColorSidesProperty},
	{"border-bottom-style",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseBorderStyleSidesProperty},
	{"border-bottom-width",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseBorderWidthSidesProperty},
	{"border-collapse",PROPERTY_KEYWORD_TYPE,BORDER_COLLAPSE_KEYWORDS,NULL},
	{"border-color",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseBorderColorProperty},
	{"border-left",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseBorderProperty},
	{"border-left-color",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseBorderColorSidesProperty},
	{"border-left-style",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseBorderStyleSidesProperty},
	{"border-left-width",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseBorderWidthSidesProperty},
	{"border-right",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseBorderProperty},
	{"border-right-color",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseBorderColorSidesProperty},
	{"border-right-style",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseBorderStyleSidesProperty},
	{"border-right-width",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseBorderWidthSidesProperty},
	{"border-spacing",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseBorderSpacingProperty},
	{"border-style",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseBorderStyleProperty},
	{"border-top",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseBorderProperty},
	{"border-top-color",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseBorderColorSidesProperty},
	{"border-top-style",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseBorderStyleSidesProperty},
	{"border-top-width",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseBorderWidthSidesProperty},
	{"border-width",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseBorderWidthProperty},
	{"bottom",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseMarginPaddingOffsetSizeProperty},
	{"caption-side",PROPERTY_KEYWORD_TYPE,CAPTION_SIDE_KEYWORDS,NULL},
	{"clear",PROPERTY_KEYWORD_TYPE,CLEAR_KEYWORDS,NULL},
	{"clip",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseClipProperty},
	{"color",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseColorProperty},
	{"content",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseContentProperty},
	{"counter-increment",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseCounterResetIncrementProperty},
	{"counter-reset",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseCounterResetIncrementProperty},
	/* aural */ {"cue",PROPERTY_FUNCTION_TYPE,0,NULL},
	/* aural */ {"cue-after",PROPERTY_FUNCTION_TYPE,0,NULL /*&PropertyParser::ParseURINoneProperty*/ },
	/* aural */ {"cue-before",PROPERTY_FUNCTION_TYPE,0,NULL /*&PropertyParser::ParseURINoneProperty*/ },
	{"cursor",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseCursorProperty},
	{"direction",PROPERTY_KEYWORD_TYPE,DIRECTION_KEYWORDS,NULL},
	{"display",PROPERTY_KEYWORD_TYPE,DISPLAY_KEYWORDS,NULL},
	/* aural */ {"elevation",PROPERTY_FUNCTION_TYPE,0,NULL},
	{"empty-cells",PROPERTY_KEYWORD_TYPE,EMPTY_CELLS_KEYWORDS,NULL},
	{"float",PROPERTY_KEYWORD_TYPE,FLOAT_KEYWORDS,NULL},
	{"font",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseFontProperty},
	{"font-family",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseFontFamilyProperty},
	{"font-size",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseFontSizeProperty},
	{"font-size-adjust",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseFontSizeAdjustProperty},
	{"font-stretch",PROPERTY_KEYWORD_TYPE,FONT_STRETCH_KEYWORDS,NULL},
	{"font-style",PROPERTY_KEYWORD_TYPE,FONT_STYLE_KEYWORDS,NULL},
	{"font-variant",PROPERTY_KEYWORD_TYPE,FONT_VARIANT_KEYWORDS,NULL},
	{"font-weight",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseFontWeightProperty},
	{"height",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseMarginPaddingOffsetSizeProperty},
	{"left",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseMarginPaddingOffsetSizeProperty},
	{"letter-spacing",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseWordLetterSpacingProperty},
	{"line-height",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseLineHeightProperty},
	{"list-style",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseListStyleProperty},
	{"list-style-image",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseURINoneProperty},
	{"list-style-position",PROPERTY_KEYWORD_TYPE,LIST_STYLE_POSITION_KEYWORDS,NULL},
	{"list-style-type",PROPERTY_KEYWORD_TYPE,LIST_STYLE_TYPE_KEYWORDS,NULL},
	{"margin",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseMarginPaddingProperty},
	{"margin-bottom",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseMarginPaddingOffsetSizeProperty},
	{"margin-left",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseMarginPaddingOffsetSizeProperty},
	{"margin-right",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseMarginPaddingOffsetSizeProperty},
	{"margin-top",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseMarginPaddingOffsetSizeProperty},
	{"marker-offset",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseMarkerOffsetProperty},
	{"marks",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseMarksProperty},
	{"max-height",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseMinMaxSizesProperty},
	{"max-width",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseMinMaxSizesProperty},
	{"min-height",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseMinMaxSizesProperty},
	{"min-width",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseMinMaxSizesProperty},
	{"orphans",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseZIndexWidowsOrphansProperty},
	{"outline",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseOutlineProperty},
	{"outline-color",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseOutlineColorProperty},
	{"outline-style",PROPERTY_KEYWORD_TYPE,OUTLINE_STYLE_KEYWORDS,NULL},
	{"outline-width",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseOutlineWidthProperty},
	{"overflow",PROPERTY_KEYWORD_TYPE,OVERFLOW_KEYWORDS,NULL},
	{"padding",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseMarginPaddingProperty},
	{"padding-bottom",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseMarginPaddingOffsetSizeProperty},
	{"padding-left",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseMarginPaddingOffsetSizeProperty},
	{"padding-right",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseMarginPaddingOffsetSizeProperty},
	{"padding-top",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseMarginPaddingOffsetSizeProperty},
	{"page",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParsePageProperty},
	{"page-break-after",PROPERTY_KEYWORD_TYPE,PAGE_BREAK_KEYWORDS,NULL},
	{"page-break-before",PROPERTY_KEYWORD_TYPE,PAGE_BREAK_KEYWORDS,NULL},
	{"page-break-inside",PROPERTY_KEYWORD_TYPE,PAGE_BREAK_INSIDE_KEYWORDS,NULL},
	/* aural */ {"pause",PROPERTY_FUNCTION_TYPE,0,NULL},
	/* aural */ {"pause-after",PROPERTY_FUNCTION_TYPE,0,NULL},
	/* aural */ {"pause-before",PROPERTY_FUNCTION_TYPE,0,NULL},
	/* aural */ {"pitch",PROPERTY_FUNCTION_TYPE,0,NULL},
	/* aural */ {"pitch-range",PROPERTY_FUNCTION_TYPE,0,NULL},
	/* aural */ {"play-during",PROPERTY_FUNCTION_TYPE,0,NULL},
	{"position",PROPERTY_KEYWORD_TYPE,POSITION_KEYWORDS,NULL},
	{"quotes",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseQuotesProperty},
	/* aural */ {"richness",PROPERTY_FUNCTION_TYPE,0,NULL},
	{"right",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseMarginPaddingOffsetSizeProperty},
	{"size",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseSizeProperty},
	/* aural */ {"speak",PROPERTY_KEYWORD_TYPE,SPEAK_KEYWORDS,NULL},
	/* aural */ {"speak-header",PROPERTY_KEYWORD_TYPE,SPEAK_HEADER_KEYWORDS,NULL},
	/* aural */ {"speak-numeral",PROPERTY_KEYWORD_TYPE,SPEAK_NUMERAL_KEYWORDS,NULL},
	/* aural */ {"speak-punctuation",PROPERTY_KEYWORD_TYPE,SPEAK_PUNCTUATION_KEYWORDS,NULL},
	/* aural */ {"speech-rate",PROPERTY_FUNCTION_TYPE,0,NULL},
	/* aural */ {"stress",PROPERTY_FUNCTION_TYPE,0,NULL},
	{"table-layout",PROPERTY_KEYWORD_TYPE,TABLE_LAYOUT_KEYWORDS,NULL},
	{"text-align",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseTextAlignProperty},
	{"text-decoration",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseTextDecorationProperty},
	{"text-indent",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseTextIndentProperty},
	{"text-shadow",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseTextShadowProperty},
	{"text-transform",PROPERTY_KEYWORD_TYPE,TEXT_TRANSFORM_KEYWORDS,NULL},
	{"top",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseMarginPaddingOffsetSizeProperty},
	{"unicode-bidi",PROPERTY_KEYWORD_TYPE,UNICODE_BIDI_KEYWORDS,NULL},
	{"vertical-align",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseVerticalAlignProperty},
	{"visibility",PROPERTY_KEYWORD_TYPE,VISIBILITY_KEYWORDS,NULL},
	/* aural */ {"voice-family",PROPERTY_FUNCTION_TYPE,0,NULL},
	/* aural */ {"volume",PROPERTY_FUNCTION_TYPE,0,NULL},
	{"white-space",PROPERTY_KEYWORD_TYPE,WHITE_SPACE_KEYWORDS,NULL},
	{"widows",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseZIndexWidowsOrphansProperty},
	{"width",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseMarginPaddingOffsetSizeProperty},
	{"word-spacing",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseWordLetterSpacingProperty},
	{"z-index",PROPERTY_FUNCTION_TYPE,0,&PropertyParser::ParseZIndexWidowsOrphansProperty}
};


/********************************* misc functions *********************************/

bool
IsWhiteSpace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == 0x0c || c == 0x0d);
}


bool
IsStringChar(char c)
{
	return c == '"' || c == '\'';
}


bool
IsNameStart(char c)
{
	c = tolower(c);
	return c >= 'a' && c <= 'z';
}


bool
IsNameChar(char c)
{
	c = tolower(c);
	return c >= 'a' && c <= 'z' || c == '-' || c >= '0' && c <= '9' || c == '_';
}


bool
IsSelectorStart(char c)
{
	c = tolower(c);
	return c >= 'a' && c <= 'z' || c == '*' || c == '[' || c == '.' || c == ':' || c == '#';
}

#define IsSelectorChar IsNameChar


char *
strndup(char *string,int length)
{
	char *a = (char *)malloc(length+1);
	
	if (!a)
		return NULL;
	
	memcpy(a,string,length);
	*(a+length) = 0;
	
	return a;
}


int
cmpName(const char *name,const char **ident)
{
	return strcasecmp(name,*ident);
}


/********************************* the CSS parser *********************************/
// #pragma mark -


StyleSheetParser::StyleSheetParser(StyleSheet *styleSheet,InputStream *input)
{
	fStyleSheet = styleSheet;
	fInput = input;
	fInComment = false;
	fProperties = NULL;
	fStatus = Parse();
	
	fSelectors.MakeEmpty();
}


status_t
StyleSheetParser::InitCheck()
{
	return fStatus;
}


void
StyleSheetParser::Error(char *text)
{
	fprintf(stderr,"*** error at line %ld: %s\n",fLine,text);
}


void
StyleSheetParser::Error(status_t rc)
{
	char *error;

	switch(rc)
	{
		// warnings

		case UNHANDLED_PROPERTY:
			Warning("Known but currently unhandled property.");
			return;		
		case UNKNOWN_PROPERTY:
			Warning("Unknown property.");
			return;

		// real errors

		case INVALID_SYMBOL:
			error = "Unknown at-rule, block skipped.";
			break;
		case INVALID_RULE_LOCATION:
			error = "invalid location for at-rule, block skipped";
			break;
		case INVALID_SELECTOR:
			error = "invalid selector definition, block skipped";
			break;
		case INVALID_SELECTOR_CLASS:
			error = "invalid selector pseudo class, block skipped";
			break;
		case UNKNOWN_KEYWORD:
			error = "Unknown or invalid keyword for property.";
			break;
		case UNKNOWN_COLOR_KEYWORD:
			error = "Unknown color keyword.";
			break;
		case INVALID_ARGUMENT:
			error = "Invalid argument.";
			break;
		case RC_OK:
			error = NULL;
			break;
		default:
			error = "Invalid value for property.";
	}
	if (error)
		Error(error);
}


void
StyleSheetParser::Warning(char *text)
{
	fprintf(stderr,"*** warning at line %ld: %s\n",fLine,text);
}


void
StyleSheetParser::AddMedia(char *mediaType)
{
	if (!mediaType)
		return;
	
	fMediaTypes.AddItem(strdup(mediaType));
}


void
StyleSheetParser::MakeEmptyMedia()
{
	for (int i = fMediaTypes.CountItems();i-- > 0;)
		free((char *)fMediaTypes.ItemAt(i));

	fMediaTypes.MakeEmpty();
}


status_t
StyleSheetParser::Parse()
{
	bool inHTMLComment = false,skipProperty = false;
	uint32 skipBlock = NO_BLOCK;
	uint32 propertyCount = 0;
	ssize_t size;

	fProperties = new Properties();
	fInBlock = 0;
	fBlockType[0] = NO_BLOCK;
	fBeginAtRule = NO_RULE;
	fImportAllowed = true;

	while((size = fInput->ReadLine(&fCurrentChar,&fLine)) > 0)
	{
		while(*fCurrentChar)
		{
			if (NextToken() < RC_OK)
				break;

			char c = *fCurrentChar;

			if (fInBlock > 0 && c == '}')	// exit block
			{
				skipProperty = false;

				if (skipBlock > fInBlock && fInBlock < MAX_BLOCKS)	// process block
				{
					switch(fBlockType[fInBlock])
					{
						case SELECTOR_BLOCK:
						{
							// no properties? so lets remove the selectors
							if (!propertyCount)
							{
								for(int i = fSelectors.CountItems();i-- > 0;)
									delete (Selector *)fSelectors.ItemAt(i);

								fSelectors.MakeEmpty();
								break;
							}

							// copy properties
							Properties *properties = fProperties;
							fStyleSheet->AddProperties(properties);
							fProperties = new Properties();
	
							propertyCount = 0;
	
							for (int i = fSelectors.CountItems();i-- > 0;)
							{
								Selector *selector = (Selector *)fSelectors.ItemAt(i);
								
								selector->SetProperties(properties);
								fStyleSheet->AddSelector(selector);

								// add media types
								for (int j = fMediaTypes.CountItems();j-- > 0;)
									selector->AddMedia((char *)fMediaTypes.ItemAt(j));
							}
							fSelectors.MakeEmpty();
							break;
						}
						case MEDIA_RULE_BLOCK:
						case PAGE_RULE_BLOCK:
						case FONTFACE_RULE_BLOCK:
							AtRuleParser::BlockEnd(this,fBlockType[fInBlock]);
							break;
					}
				}
				else if (skipBlock == fInBlock)
					skipBlock = NO_BLOCK;

				fBlockType[fInBlock] = NO_BLOCK;
				--fInBlock;

				fCurrentChar++;
			}
			else if (skipBlock <= fInBlock || fInBlock >= MAX_BLOCKS)	// skip this block
			{
				if (IsStringChar(c))
					SkipString();
				else if (c == '{')
					fInBlock++;

				fCurrentChar++;
			}
			else if (fBlockType[fInBlock] == SELECTOR_BLOCK)
			{
				status_t rc;

				if (skipProperty)
				{
					if (IsStringChar(c))
						SkipString();
					else if (*fCurrentChar == ';')
						skipProperty = false;
				}
				else if (IsNameStart(c) && (rc = ParseProperty()) < RC_OK)
				{
					Error(rc);

					skipProperty = true;
					fCurrentChar--;
				}
				else
					propertyCount++;

				fCurrentChar++;
			}
			else if (c == '{')
			{
				fInBlock++;
				fImportAllowed = false;

				// is it a rule block or a property block?
				if (fBeginAtRule != NO_RULE)
				{
					fBlockType[fInBlock] = (uint8)fBeginAtRule;
					fBeginAtRule = NO_RULE;
				}
				else if (fInBlock < MAX_BLOCKS)
					fBlockType[fInBlock] = SELECTOR_BLOCK;

				fCurrentChar++;
			}
			else if (fBeginAtRule != NO_RULE && c == ';')
			{
				skipBlock = NO_BLOCK;
				fBeginAtRule = NO_RULE;
				AtRuleParser::BlockEnd(this,NO_RULE);

				fCurrentChar++;
			}
			else if (IsSelectorStart(c))
			{
				status_t rc;
				if ((rc = ParseSelector()) < RC_OK)
				{
					Error(rc);
					skipBlock = fInBlock+1;	// skip next block
				}
			}
			else if (c == '@')	// special symbols
			{
				status_t rc;

				fCurrentChar++;
				fBeginAtRule = SKIP_RULE;

				if ((rc = AtRuleParser::Parse(this)) < RC_OK)
				{
					Error(rc);
					skipBlock = fInBlock + 1;
				}
			}
			else if (!strcmp("<!--",fCurrentChar))
			{
				if (inHTMLComment)
				{
					Error("Nested HTML comment, rest of file skipped");
					return INVALID_COMMENTS;
				}
				inHTMLComment = true;
				fCurrentChar += 4;
			}
			else if (!strcmp("-->",fCurrentChar))
			{
				if (!inHTMLComment)
				{
					Error("Invalid HTML comment tag, rest of file skipped");
					return INVALID_COMMENTS;
				}
				inHTMLComment = false;
				fCurrentChar += 3;
			}
			else
			{
				printf("'%c'??\n",*fCurrentChar);
				Error("Unexpected character");
				fCurrentChar++;
			}
		}
	}
	return RC_OK;
}


status_t
StyleSheetParser::ReadLine()
{
	if (fInput->ReadLine(&fCurrentChar,&fLine) <= 0)
	{
		Error("Unexpected end of file");
		return RC_IO_ERROR;
	}
	return RC_OK;
}

/* skips comments and white spaces until something happens... */

status_t
StyleSheetParser::NextToken()
{
	bool inComment = false;

	while(true)
	{
		if (inComment)
		{
			while(*fCurrentChar)
			{
				if (*fCurrentChar == '*' && *(fCurrentChar+1) == '/')
				{
					inComment = false;
					fCurrentChar += 2;
					break;
				}
				fCurrentChar++;
			}
		}
		while(IsWhiteSpace(*fCurrentChar))
			fCurrentChar++;
		if (*fCurrentChar == '/' && *(fCurrentChar+1) == '*')
		{
			inComment = true;
			fCurrentChar += 2;
		}
		if (!*fCurrentChar)	// end of line reached
		{
			if (ReadLine() < RC_OK)
				return RC_IO_ERROR;
		}
		else if (!inComment)
			return RC_OK;
	}	
}


status_t
StyleSheetParser::SkipString()
{
	int in = *fCurrentChar++;

	while(*fCurrentChar && in)
	{
		if (*fCurrentChar == '\\')
		{
			fCurrentChar++;
			if (!*fCurrentChar)	// escaped newline
			{
				if (ReadLine() < RC_OK)
					return RC_IO_ERROR;
				continue;
			}
		}
		else if (*fCurrentChar == in)
		{
			in = 0;
			break;
		}
		fCurrentChar++;
	}
	if (in)
	{
		Error("String exceeds line.");
		fCurrentChar--;	// preserve terminating null byte

		return INVALID_VALUE;
	}
	return RC_OK;
}


status_t
StyleSheetParser::SkipLine(char *delimiter)
{
	char c;
	while(true)
	{
		c = *fCurrentChar;
		
		if (IsStringChar(c))
		{
			SkipString();
			fCurrentChar++;
		}
		else if (c != 0 && strchr(delimiter,c))
		{
			return RC_OK;
		}
		else if (IsWhiteSpace(c) || c == 0)
		{
			if (NextToken() < RC_OK)
				return RC_IO_ERROR;
		}
		else
			fCurrentChar++;
	}
}


status_t
StyleSheetParser::ParseName(char **name,bool firstChar = true)
{
	char *c = fCurrentChar;

	if (firstChar && !IsNameStart(*c) || !IsNameChar(*c))
		return INVALID_VALUE;

	while(IsNameChar(*c))
		c++;

	*name = strndup(fCurrentChar,c - fCurrentChar);
	fCurrentChar = c;

	return RC_OK;
}


status_t
StyleSheetParser::ParseSimpleSelector(SimpleSelector **storage)
{
	SimpleSelector *selector = new SimpleSelector();
	status_t rc = RC_OK;
	
	if (*fCurrentChar == '*')	// universal (wild-card) selector
		fCurrentChar++;
	else
		ParseName(&selector->name);

	if (selector->name == NULL)
		selector->flags = SELECTOR_UNIVERSAL;

	while(true)
	{
		char c = *fCurrentChar;
		if (c == ':')	// pseudo class/element
		{
			fCurrentChar++;
			
			for(int i = 0;i < NUM_PSEUDO;i++)
			{
				int32 length;

				if (!strncasecmp(pseudoClasses[i].name,fCurrentChar,length = strlen(pseudoClasses[i].name)))
				{
					selector->flags |= pseudoClasses[i].flag;
					fCurrentChar += length;
					
					if (pseudoClasses[i].flag == SELECTOR_LANG)
					{
						if (*fCurrentChar == '(')
						{
							fCurrentChar++;

							char *name;
							if ((rc = ParseName(&name)) == RC_OK)
							{
								if (*fCurrentChar == ')')
								{
									fCurrentChar++;
									selector->language = name;
									break;
								}
								free(name);
							}
						}
						rc = INVALID_SELECTOR_CLASS;
					}
					break;
				}
			}
			if (IsNameChar(*fCurrentChar))
			{
				rc = INVALID_SELECTOR_CLASS;
				break;
			}
		}
		else if (c == '.' || c == '#')	// class or id
		{
			++fCurrentChar;

			char *name;
			if ((rc = ParseName(&name,false)) < RC_OK)
				break;

			Attribute *attr = new Attribute();
			attr->type = c == '.' ? ATTRIBUTE_CLASS : ATTRIBUTE_ID;
			attr->name = name;
			attr->value = NULL;

			selector->AddAttribute(attr);
		}
		else if (c == '[')	// attribute selector
		{
			++fCurrentChar;

			char *name;
			if ((rc = ParseName(&name)) < RC_OK)
				break;

			int8 type = ATTRIBUTE_NONE;
			char *value = NULL;

			c = *fCurrentChar++;
			if (c == '=')
				type = ATTRIBUTE_EQUAL;
			else if (c == ']')
				type = ATTRIBUTE_SET;
			else if (*fCurrentChar == '=')
			{
				if (c == '~')
					type = ATTRIBUTE_IN;
				else if (c == '|')
					type = ATTRIBUTE_BEGIN;

				fCurrentChar++;
			}
			if (type >= ATTRIBUTE_EQUAL)	// attribute has a value
			{
				if (((rc = ParseName(&value,false)) < RC_OK &&
					(rc = PropertyParser::ParseString(this,&value)) < RC_OK) ||
					*fCurrentChar != ']')
				{
					rc = INVALID_SELECTOR;
					break;
				}
				fCurrentChar++;		// skip ']'
			}

			Attribute *attr = new Attribute();
			attr->type = type;
			attr->name = name;
			attr->value = value;
			
			selector->AddAttribute(attr);
		}
		else	// whatever follows, this selector ends here
			break;
	}

	if (rc < RC_OK)		// an error occured
	{
		delete selector;
		return rc;
	}
	*storage = selector;
	return RC_OK;
}


status_t
StyleSheetParser::ParseSelector()
{
	Selector *selector = new Selector();
	SimpleSelector *s = NULL;
	int8 lastOperator = OP_NONE;
	status_t rc = RC_OK;

	while(true)
	{
		char c;

		c = *fCurrentChar;
		if (IsSelectorStart(c))
		{
			if (s != NULL)	// the last simple selector
			{
				// pseudo elements are allowed only in the selector's subject
				if (s->flags & SELECTOR_PSEUDO_ELEMENT)
				{
					rc = INVALID_SELECTOR_CLASS;
					break;
				}

				// the white space operator is not very visible...
				if (lastOperator == OP_NONE)
					s->op = OP_DESCENDANT;
				else
					s->op = lastOperator;
			}
			if ((rc = ParseSimpleSelector(&s)) < RC_OK)
				break;
			
			selector->AddSimpleSelector(s);
			lastOperator = OP_NONE;
		}
		else if (s != NULL)
		{
			if (c == '>')	// child combinator
			{
				lastOperator = OP_CHILD;
				fCurrentChar++;
			}
			else if (c == '+')	// adjacent combinator
			{
				lastOperator = OP_ADJACENT;
				fCurrentChar++;
			}
			else if (c == ',')	// group
			{
				fSelectors.AddItem(selector);
				selector = new Selector();
				fCurrentChar++;
			}
			else if (c == '{')	// begin of declaration block
			{
				break;
			}
			else
			{
				rc = INVALID_SELECTOR;
				break;
			}
		}
		else	// invalid characters without a selector
		{
			rc = INVALID_SELECTOR;
			break;
		}
		if ((rc = NextToken()) < RC_OK)
			break;
	}

	if (rc < RC_OK || (s && s->op != OP_NONE))		// an error occured
	{
		// delete current selector
		delete selector;

		// empty selector list
		for(int i = fSelectors.CountItems();i-- > 0;)
			delete (Selector *)fSelectors.ItemAt(i);
		fSelectors.MakeEmpty();

		SkipLine("{");
		return rc;
	}

	// add the last selector to the list of current selectors
	fSelectors.AddItem(selector);
	return RC_OK;
}


status_t
StyleSheetParser::CheckForPropertyEnd(bool *important)
{
	if (NextToken() < RC_OK)
		return RC_IO_ERROR;

	if (*fCurrentChar == '!')	// check for "important" keyword
	{
		fCurrentChar++;
		if (NextToken() < RC_OK)
			return RC_IO_ERROR;
		if (strncasecmp("important",fCurrentChar,9) || IsNameChar(*(fCurrentChar+9)))
			return INVALID_VALUE;
			
		*important = true;
		fCurrentChar += 9;

		if (NextToken() < RC_OK)
			return RC_IO_ERROR;
	}
	else
		*important = false;

	if (*fCurrentChar != ';' && *fCurrentChar != '}')
		return INVALID_VALUE;
	
	return RC_OK;
}


status_t
StyleSheetParser::ParseProperty()
{
	// get name of property
	char *name;
	if (ParseName(&name) < RC_OK)
		return INVALID_IDENTIFIER;

	if (NextToken() < RC_OK || *fCurrentChar != ':')
	{
		free(name);
		return INVALID_IDENTIFIER;
	}
	fCurrentChar++;
	
	// find property
	struct property *property = (struct property *)bsearch(name,properties,NUM_PROPERTIES,sizeof(struct property),(int (*)(const void *,const void *))cmpName);
	free(name);
	if (property == NULL)
		return UNKNOWN_PROPERTY;

	if (NextToken() < RC_OK)
		return INVALID_VALUE;

	// array position
	uint32 propertyNumber = ((uint32)property - (uint32)&properties[0])/sizeof(struct property);
	status_t rc = RC_OK;

	fLastPropertyFlags.Reset();

	// check for the universal "inherit" value
	char *end = fCurrentChar+7;
	if (!strncasecmp(fCurrentChar,"inherit",7) && !IsNameChar(*end))
	{
		fCurrentChar = end;
		fProperties->inherit.Set(propertyNumber);
	}
	else if (property->type == PROPERTY_KEYWORD_TYPE)
	{
		int i = property->keywords;
		int8 keyword;
		
		if ((rc = PropertyParser::ParseKeywordOrUnit(this,&keyword,keywordProperties[i].keywords,keywordProperties[i].numKeywords,
													 NULL,UNIT_NONE)) < RC_OK)
			return rc;
		
		*(&fProperties->backgroundAttach + i) = keyword;
	}
	else if (property->type == PROPERTY_FUNCTION_TYPE && property->parseFunction)
		rc = property->parseFunction(this,propertyNumber);
	else
		rc = UNHANDLED_PROPERTY;

	if (rc == RC_OK)
	{
		bool important;
		if (CheckForPropertyEnd(&important) < RC_OK)
			return INVALID_VALUE;

		if (fLastPropertyFlags != 0)	// special flags are set
		{
			fProperties->specified |= fLastPropertyFlags;
			fProperties->important |= fLastPropertyFlags;
		}
		else
		{
			fProperties->specified.Set(propertyNumber);
			fProperties->important.Set(propertyNumber,important);
		}
	}
	if (*fCurrentChar == '}')	// leave block delimiter on stack
		fCurrentChar--;

	return rc;
}

