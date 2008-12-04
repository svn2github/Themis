/* PropertyParser.cpp - reads and parses CSS properties
**
** Copyright 2001, pinc Software. All Rights Reserved.
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "PropertyParser.h"
#include "StyleSheetParser.h"
#include "StyleSheet.h"

#ifdef __BEOS__
#	include <InterfaceDefs.h>
#endif

float gMediumFontSizeBase = 12;	// pt
float gFontSizeFactor = 0.15;
float gBorderSizes[] = {0.5, 1.0, 2.0};	// thin, medium, thick

struct color {
	char *name;
	rgb_color rgbColor;
};

const struct color colors[] = {
	{"aqua",	{  0,  0,  0,255}},
	{"black",	{  0,  0,  0,255}},
	{"blue",	{  0,  0,240,255}},
	{"fuchsia",	{  0,  0,  0,255}},
	{"gray",	{ 80, 80, 80,255}},
	{"green",	{  0,240,  0,255}},
	{"grey",	{ 80, 80, 80,255}},
	{"lime",	{  0,  0,  0,255}},
	{"maroon",	{  0,  0,  0,255}},
	{"navy",	{  0,  0,  0,255}},
	{"olive",	{  0,  0,  0,255}},
	{"purple",	{240,  0,240,255}},
	{"red",		{240,  0,  0,255}},
	{"silver",	{200,200,200,255}},
	{"teal",	{  0,  0,  0,255}},
	{"white",	{255,255,255,255}},
	{"yellow",	{240,240,  0,255}}
};

#define NUM_COLORS 17

const char *systemColors[] = {
	"ActiveBorder",
	"ActiveCaption",
	"AppWorkspace",
	"Background",
	"ButtonFace",
	"ButtonHighlight",
	"ButtonShadow",
	"ButtonText",
	"CaptionText",
	"GrayText",
	"Highlight",
	"HighlightText",
	"InactiveBorder",
	"InactiveCaption",
	"InactiveCaptionText",
	"InfoBackground",
	"InfoText",
	"Menu",
	"MenuText",
	"Scrollbar",
	"ThreeDDarkShadow",
	"ThreeDFace",
	"ThreeDHighlight",
	"ThreeDLightShadow",
	"ThreeDShadow",
	"Window",
	"WindowFrame",
	"WindowText"
};

enum system_colors {
	ACTIVE_BORDER_COLOR = 0,
	ACTIVE_CAPTION_COLOR,
	APP_WORKSPACE_COLOR,
	BACKGROUND_COLOR,
	BUTTON_FACE_COLOR,
	BUTTON_HIGHLIGHT_COLOR,
	BUTTON_SHADOW_COLOR,
	BUTTON_TEXT_COLOR,
	CAPTION_TEXT_COLOR,
	GRAY_TEXT_COLOR,
	HIGHLIGHT_COLOR,
	HIGHLIGHT_TEXT_COLOR,
	INACTIVE_BORDER_COLOR,
	INACTIVE_CAPTION_COLOR,
	INACTIVE_CAPTION_TEXT_COLOR,
	INFO_BACKGROUND_COLOR,
	INFO_TEXT_COLOR,
	MENU_COLOR,
	MENU_TEXT_COLOR,
	SCROLLBAR_COLOR,
	THREED_DARK_SHADOW_COLOR,
	THREED_FACE_COLOR,
	THREED_HIGHLIGHT_COLOR,
	THREED_LIGHT_SHADOW_COLOR,
	THREED_SHADOW_COLOR,
	WINDOW_COLOR,
	WINDOW_FRAME_COLOR,
	WINDOW_TEXT_COLOR
};

#define NUM_SYSTEM_COLORS 28

const char *keywordsNormal[] = {"normal"};
#define NUM_KEYWORDS_NORMAL 1
const char *keywordsAuto[] = {"auto"};
#define NUM_KEYWORDS_AUTO 1

const char *keywordsFont[] = {"caption","icon","menu","message-box","small-caption","status-bar"};
#define NUM_KEYWORDS_FONT 6
const char *genericFontFamilies[] = {"serif","sans-serif","cursive","fantasy","monospace"};
#define NUM_GENERIC_FONT_FAMILIES 5
const char *keywordsFontWeight[] = {"normal","bold","bolder","lighter","100","200","300","400","500","600","700","800","900"};
#define NUM_KEYWORDS_FONT_WEIGHT 13

const char *keywordsCursor[] = {"auto","crosshair","default","pointer","move","e-resize",
	"ne-resize","nw-resize","n-resize","se-resize","sw-resize","s-resize","w-resize",
	"text","wait","help"};
#define NUM_KEYWORDS_CURSOR 16

const char *keywordsContent[] = {"open-quote","close-quote","no-open-quote","no-close-quote"};
#define NUM_KEYWORDS_CONTENT 4

const char *keywordsBorderWidth[] = {"thin","medium","thick"};
#define NUM_KEYWORDS_BORDER_WIDTH 3

const char *keywordsTextDeco[] = {"none","underline","overline","line-through","blink"};
#define NUM_KEYWORDS_TEXT_DECO 5
const char *keywordsVerticalAlign[] = {"baseline","sub","middle","super","text-top",
	"text-bottom","top","bottom"};
#define NUM_KEYWORDS_VERTICAL_ALIGN 8
const char *keywordsTextAlign[] = {"left","right","center","justify"};
#define NUM_KEYWORDS_TEXT_ALIGN 4



bool
IsPropertyDelimiter(char c)
{
	return c == ';' || c == '}' || c == '!';
}


bool
IsPropertyValueDelimiter(char c)
{
	return c == ';' || c == '}' || c == '!' || c == '/' || c == ',' || c == ')';
}


int
hex2dec(const char c)
{
	if (c <= '9')
		return c - '0';
	return tolower(c) - 'a' + 10;
}


inline bool
UnitIsLength(UnitValue *unit)
{
	return unit->flags & UNIT_LENGTH;
}


/********************************* CSS property parser *********************************/
// #pragma mark -

status_t
PropertyParser::CheckForEnd(StyleSheetParser *parser)
{
	if (parser->NextToken() < RC_OK)
		return RC_IO_ERROR;

	if (!IsPropertyDelimiter(*parser->fCurrentChar))
		return INVALID_VALUE;
	
	return RC_OK;
}


status_t
PropertyParser::ParseNumber(StyleSheetParser *parser,float *value,uint8 *type = NULL)
{
	char *c = parser->fCurrentChar;
	bool negative = false;
	float n = 0;
	int i = 0;

	if (*c == '-' || *c == '+')
	{
		if (*c == '-')
			negative = true;
		parser->fCurrentChar++;
		
		c = parser->fCurrentChar;
	}	
	if ((*c < '0' || *c > '9') && *c != '.')
		return INVALID_VALUE;
	
	while(*c >= '0' && *c <= '9')
	{
		n *= 10;
		n += *c++ - '0';
		i++;
	}
	if (i == 0 && *c != '.')
		return INVALID_VALUE;
	
	if (*c == '.')
	{
		i = 10;  c++;
		while(*c >= '0' && *c <= '9')
		{
			n += 1.0*(*c++ - '0')/i;
			i *= 10;
		}
		if (i == 10)
			return INVALID_VALUE;

		if (type)
			*type = FLOAT_NUMBER;
	}
	else if (type)
		*type = INTEGER_NUMBER;

	if (negative)
		n = -n;
	*value = n;
	parser->fCurrentChar = c;

	return RC_OK;
}


status_t
PropertyParser::ParseUnit(StyleSheetParser *parser,UnitValue *length)
{
//	char *start = parser->fCurrentChar;

	float value;
	uint8 type;

	if (ParseNumber(parser,&value,&type) < RC_OK)
		return INVALID_VALUE;

	uint32 flags = 0;
	char *c = parser->fCurrentChar;
	char second = tolower(*(c+1));
	switch(tolower(*c++))
	{
		case '%':	// percent
			value /= 100;
			flags = UNIT_PERCENT;
			break;
		case 'c':	// cm
			if (second == 'm')
			{
				value *= 10;	// metric base is mm
				flags = UNIT_METRIC;
				c++;
			}
			break;
		case 'd':	// deg
			if (second == 'e' && tolower(*(c+1)) == 'g')
			{
				value = (value * PI) / 180;
				flags = UNIT_RADIAN;
				c += 2;
			}
			break;
		case 'e':	// em, ex
			if (second == 'm')
			{
				flags = UNIT_EM;
				c++;
			}
			else if (second == 'x')
			{
				flags = UNIT_EX;
				c++;
			}
			break;
		case 'g':	// grad
			if (second == 'r' && tolower(*(c+1)) == 'a' && tolower(*(c+2)) == 'd')
			{
				value = (value * PI) / 200;
				flags = UNIT_RADIAN;
				c += 3;
			}
			break;
		case 'h':	// Hz
			if (second == 'z')
			{
				flags = UNIT_HERTZ;
				c++;
			}
			break;
		case 'i':	// inch
			if (second == 'n')
			{
				value *= 25.4;	// metric base is mm
				flags = UNIT_METRIC;
				c++;
			}
			break;
		case 'k':	// kHz
			if (second == 'h' && tolower(*(c+1)) == 'z')
			{
				value *= 1000;	// frequency base is Hz
				flags = UNIT_HERTZ;
				c += 2;
			}
			break;
		case 'm':	// mm, ms
			if (second == 'm')
			{
				flags = UNIT_METRIC;
				c++;
			}
			else if (second == 's')
			{
				flags = UNIT_MILLI;
				c++;
			}
			break;
		case 'p':	// pt, px, pc
			if (second == 't')
			{
				flags = UNIT_PT;
				c++;
			}
			else if (second == 'x')
			{
				flags = UNIT_PIXEL;
				c++;
			}
			else if (second == 'c')
			{
				value *= 12;	// 1pc == 12pt
				flags = UNIT_PT;
				c++;
			}
			break;
		case 'r':	// rad
			if (second == 'a' && tolower(*(c+1)) == 'd')
			{
				flags = UNIT_RADIAN;
				c += 2;
			}
			break;
		case 's':	// rad
			value *= 1000;
			flags = UNIT_MILLI;
			break;
		default:
			c--;
			if (IsWhiteSpace(*c) || IsPropertyValueDelimiter(*c))
			{
				if (length->flags & UNIT_NOT_SPECIFIED)
					flags = UNIT_NOT_SPECIFIED;
				else if (value == 0)	// zero doesn't need a unit specifier
					flags = UNIT_METRIC;
			}
	}
	if (flags == 0 || (!IsWhiteSpace(*c) && !IsPropertyValueDelimiter(*c)))
		return INVALID_VALUE;
	
	parser->fCurrentChar = c;
	length->value = value;
	length->flags = flags;
	
	return RC_OK;
}


status_t
PropertyParser::ParseLength(StyleSheetParser *parser,UnitValue *length)
{
	status_t rc;
	if ((rc = ParseUnit(parser,length)) < RC_OK)
		return rc;
	
	if (!UnitIsLength(length))
		return INVALID_VALUE;
	
	return RC_OK;
}


status_t
PropertyParser::ParseKeyword(StyleSheetParser *parser,char **keyword,bool firstChar = true)
{
	char *start = parser->fCurrentChar;

	if (parser->ParseName(keyword,firstChar) < RC_OK)
		return INVALID_VALUE;

	char c = *parser->fCurrentChar;
	if (c && !IsWhiteSpace(c) && !IsPropertyValueDelimiter(c))
	{
		free(*keyword);  *keyword = NULL;
		parser->fCurrentChar = start;
		return INVALID_VALUE;
	}

	return RC_OK;
}


status_t
PropertyParser::ParseString(StyleSheetParser *parser,char **string)
{
	char *c = parser->fCurrentChar;
	
	if (!IsStringChar(*c))
		return INVALID_VALUE;

	char in = *c++;

	char buffer[512];
	int pos = 0;
	while(*c && *c != in)
	{
		if (*c == '\\')
		{
			c++;
			if (*c == 0)	// escaped newline
			{
				if (parser->ReadLine() < RC_OK)
					return RC_IO_ERROR;

				c = parser->fCurrentChar;
				continue;
			}
		}
		if (pos < 511)
			buffer[pos++] = *c;
		c++;
	}
	buffer[pos] = 0;

	parser->fCurrentChar = c;
	if (!*c)
		return STRING_EXCEEDS_LINE;

	parser->fCurrentChar++;
	*string = strdup(buffer);

	return RC_OK;
}


status_t
PropertyParser::ParseShape(StyleSheetParser *parser,Rect<UnitValue> *rect)
{
	if (strncasecmp("rect(",parser->fCurrentChar,5))
		return INVALID_VALUE;

	parser->fCurrentChar += 5;
	if (parser->NextToken() < RC_OK)
		return INVALID_VALUE;

	status_t rc;

	for(int i = 0;i < 4;i++)
	{
		UnitValue *value = NULL;

		switch(i)
		{
			case 0: value = &rect->top; break;
			case 1: value = &rect->right; break;
			case 2: value = &rect->bottom; break;
			case 3: value = &rect->left; break;
		}
		if (ParseLength(parser,value) < RC_OK)
			return INVALID_VALUE;
		
		rc = parser->NextToken();
		if (rc == RC_OK && i < 3)
		{
			if (*parser->fCurrentChar != ',')
				return INVALID_VALUE;
			parser->fCurrentChar++;
			rc = parser->NextToken();
		}
		if (rc < RC_OK)
			return rc;
	}
	if (*parser->fCurrentChar != ')')
		return INVALID_VALUE;
	parser->fCurrentChar++;

	return RC_OK;
}


status_t
PropertyParser::ParseURL(StyleSheetParser *parser,char **url)
{
	if (strncasecmp("url(",parser->fCurrentChar,4))
		return INVALID_VALUE;

	parser->fCurrentChar += 4;
	if (parser->NextToken() < RC_OK)
		return INVALID_VALUE;

	char *c = parser->fCurrentChar;
	
	if (IsStringChar(*c))
	{
		status_t rc;
		if ((rc = ParseString(parser,url)) < RC_OK)
			return rc;
	}
	else
	{
		char buffer[512];
		int pos = 0;
		while(*c && *c != ')' && !IsWhiteSpace(*c))
		{
			if (*c == '\\')
				c++;
			// should test for other characters which have to be escaped here
	
			buffer[pos++] = *c++;
		}
		buffer[pos] = 0;
	
		*url = strdup(buffer);
		parser->fCurrentChar = c;	
	}
	if (parser->NextToken() < RC_OK || *parser->fCurrentChar != ')')
	{
		free(url);
		return INVALID_VALUE;
	}
	parser->fCurrentChar++;

	return RC_OK;
}


void
PropertyParser::GetSystemColor(uint32 num,rgb_color *color)
{
#ifdef __BEOS__
	switch(num)
	{
		case ACTIVE_BORDER_COLOR:
			*color = ui_color(B_WINDOW_TAB_COLOR);
			break;
		case ACTIVE_CAPTION_COLOR:
		case BUTTON_TEXT_COLOR:
		case CAPTION_TEXT_COLOR:
		case INFO_TEXT_COLOR:
		case HIGHLIGHT_TEXT_COLOR:
		case WINDOW_TEXT_COLOR:
			color->red = color->green = color->blue = 0;
			color->alpha = 255;
			break;
		case BACKGROUND_COLOR:		// desktop background
			*color = ui_color(B_DESKTOP_COLOR);
			break;
		case APP_WORKSPACE_COLOR:
		case INFO_BACKGROUND_COLOR:
		case WINDOW_COLOR:
			*color = ui_color(B_PANEL_BACKGROUND_COLOR);
			break;
		case MENU_TEXT_COLOR:
			*color = ui_color(B_MENU_ITEM_TEXT_COLOR);
			break;
		case MENU_COLOR:
			*color = ui_color(B_MENU_BACKGROUND_COLOR);
			break;
		case THREED_LIGHT_SHADOW_COLOR:
			*color = tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),B_LIGHTEN_1_TINT/2.0f);
			break;
		case THREED_HIGHLIGHT_COLOR:
		case BUTTON_HIGHLIGHT_COLOR:
			*color = tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),B_LIGHTEN_2_TINT);
			break;
		case THREED_FACE_COLOR:
		case BUTTON_FACE_COLOR:
		case INACTIVE_BORDER_COLOR:
			*color = tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),B_LIGHTEN_1_TINT);
			break;
		case SCROLLBAR_COLOR:
		case WINDOW_FRAME_COLOR:
			*color = tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),B_DARKEN_1_TINT);
			break;
		case THREED_DARK_SHADOW_COLOR:
		case GRAY_TEXT_COLOR:
		case INACTIVE_CAPTION_COLOR:
		case INACTIVE_CAPTION_TEXT_COLOR:
		case HIGHLIGHT_COLOR:
			*color = tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),B_DARKEN_3_TINT);
			break;
		case BUTTON_SHADOW_COLOR:
		case THREED_SHADOW_COLOR:
			*color = tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),B_DARKEN_2_TINT);
			break;
	}
#else
#	error No BeOS, no system colors...
#endif
}


status_t
PropertyParser::GetColorValue(StyleSheetParser *parser,uint8 *color,char delimiter)
{
	if (parser->NextToken() < RC_OK)
		return INVALID_VALUE;

	float value;
	uint8 type;

	if (ParseNumber(parser,&value,&type) < RC_OK)
		return INVALID_VALUE;

	int32 gun;
	if (*parser->fCurrentChar == '%')	// percent
	{
		gun = (int32)(value*255/100);
		parser->fCurrentChar++;
	}
	else if (type == FLOAT_NUMBER)
		gun = (int32)(value*255);
	else
		gun = (int32)value;

	if (parser->NextToken() < RC_OK || *parser->fCurrentChar != delimiter)
		return INVALID_VALUE;
	parser->fCurrentChar++;	// skip valid delimiter
	
	if (gun > 255)
		*color = 255;
	else if (gun < 0)
		*color = 0;
	else
		*color = (uint8)gun;
	
	return RC_OK;
}


status_t
PropertyParser::ParseColor(StyleSheetParser *parser,rgb_color *color)
{
	char *start = parser->fCurrentChar;
	char *keyword;
	if (ParseKeyword(parser,&keyword) >= RC_OK)
	{
		struct color *colorDefinition = (struct color *)bsearch(keyword,colors,NUM_COLORS,sizeof(struct color),(int (*)(const void *,const void *))cmpName);

		if (colorDefinition == NULL)
		{
			char *c = (char *)bsearch(keyword,systemColors,NUM_SYSTEM_COLORS,sizeof(char *),(int (*)(const void *,const void *))cmpName);
			if (c != NULL)
			{
				GetSystemColor(((uint32)c - (uint32)&systemColors[0])/sizeof(char *),color);
				colorDefinition = (struct color *)true;
			}
		}
		else
			*color = colorDefinition->rgbColor;
		
		free(keyword);

		if (colorDefinition == NULL)
		{
			parser->fCurrentChar = start;
			return UNKNOWN_COLOR_KEYWORD;
		}
		return RC_OK;
	}

	char *first = parser->fCurrentChar;
	char *c = first;

	if (*c != '#' && strncasecmp(c,"rgb(",4))
		return INVALID_VALUE;

	color->alpha = 255;	// color should be opaque

	if (*c == '#')
	{
		c++;  first++;
		
		int i;
		for(i = 0;*c >= '0' && *c <= '9' || tolower(*c) >= 'a' && tolower(*c) <= 'f';c++,i++);

		if (i == 6)
		{
			color->red = hex2dec(*(first++)) << 4 | hex2dec(*(first++));
			color->green = hex2dec(*(first++)) << 4 | hex2dec(*(first++));
			color->blue = hex2dec(*(first++)) << 4 | hex2dec(*(first++));
		}
		else if (i == 3)
		{
			uint8 value;
			
			value = hex2dec(*(first++));
			color->red = (value << 4) | value;
			value = hex2dec(*(first++));
			color->green = (value << 4) | value;
			value = hex2dec(*(first++));
			color->blue = (value << 4) | value;
		}
		else
			return INVALID_VALUE;

		parser->fCurrentChar = first;
	}
	else	// rgb
	{
		parser->fCurrentChar = first + 4;

		if (GetColorValue(parser,&color->red,',') < RC_OK)
			return INVALID_VALUE;
		if (GetColorValue(parser,&color->green,',') < RC_OK)
			return INVALID_VALUE;
		if (GetColorValue(parser,&color->blue,')') < RC_OK)
			return INVALID_VALUE;		
	}
	return RC_OK;
}


status_t
PropertyParser::ParseLongName(StyleSheetParser *parser, char **name)
{
	char *c = parser->fCurrentChar;

	if (!IsNameStart(*c))
		return INVALID_VALUE;

	char buffer[512];
	int pos = 0;
	while(IsNameChar(*c) || IsWhiteSpace(*c))
	{
		if (pos < 511)
			buffer[pos++] = *c;
		c++;

		if (*c == 0)
		{
			if (parser->ReadLine() < RC_OK)
				return RC_IO_ERROR;

			if (pos < 511)	// insert a space
				buffer[pos++] = ' ';

			c = parser->fCurrentChar;
		}
	}
	buffer[pos] = 0;

	while(IsWhiteSpace(buffer[--pos]))
		buffer[pos] = 0;

	*name = strdup(buffer);
	parser->fCurrentChar = c;

	return RC_OK;
}


bool
PropertyParser::TestForKeyword(StyleSheetParser *parser,char *keyword)
{
	int length = strlen(keyword);

	if (strncasecmp(parser->fCurrentChar,keyword,length) || IsNameChar(*(parser->fCurrentChar+length)))
		return false;

	parser->fCurrentChar += length;
	return true;
}


int8
PropertyParser::FindKeyword(const char *keyword,const char **keywords,int numKeywords)
{
	if (!keyword || !*keyword)
		return -1;

	for(int i = 0;i < numKeywords;i++)
	{
		if (!strcasecmp(keyword,keywords[i]))
			return i;
	}
	return -1;
}


status_t
PropertyParser::ParseKeywordOrUnit(StyleSheetParser *parser,int8 *keyword,const char **keywords,int numKeywords,UnitValue *value,uint32 flags)
{
	status_t rc = INVALID_VALUE;

	if (keyword)
	{
		char *start = parser->fCurrentChar;
		*keyword = -1;

		char *word;
		if (ParseKeyword(parser,&word) == RC_OK)
		{
			*keyword = FindKeyword(word,keywords,numKeywords);
			
			free(word);
			if (*keyword != -1)	// matching keyword found
				return RC_OK;
			
			parser->fCurrentChar = start;
			rc = UNKNOWN_KEYWORD;
		}
	}
	if (value)
	{
		if (flags & UNIT_NOT_SPECIFIED)
			value->flags = UNIT_NOT_SPECIFIED;

		if (ParseUnit(parser,value) == RC_OK)
		{
			if (!(flags & UNIT_PERCENT) && (value->flags & UNIT_PERCENT) ||
				!(flags & UNIT_LENGTH) && (value->flags & UNIT_LENGTH) ||
				!(flags & UNIT_TIME) && (value->flags & UNIT_TIME) ||
				!(flags & UNIT_ANGLE) && (value->flags & UNIT_ANGLE) ||
				!(flags & UNIT_FREQUENCY) && (value->flags & UNIT_FREQUENCY) ||
				!(flags & UNIT_NOT_SPECIFIED) && (value->flags & UNIT_NOT_SPECIFIED))
				return INVALID_VALUE;

			return RC_OK;
		}
	}
	return rc;
}


// #pragma mark -

status_t
PropertyParser::ParseURINoneProperty(StyleSheetParser *parser,uint32 property)
{
	char *url;

	status_t rc;
	if ((rc = ParseURL(parser,&url)) < RC_OK)
	{
		if (rc != INVALID_VALUE || !TestForKeyword(parser,"none"))
			return rc;

		url = NULL;
	}

	if (CheckForEnd(parser) < RC_OK)
		return INVALID_VALUE;

	switch(property)
	{
		case BACKGROUND_IMAGE_PROPERTY:
			parser->fProperties->backgroundImage = url;
			break;
		case LIST_STYLE_IMAGE_PROPERTY:
			parser->fProperties->listStyleImage = url;
			break;
		default:
			free(url);
			break;
	}
	return RC_OK;
}


// #pragma mark -


status_t
PropertyParser::ParseColorProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	rgb_color color;

	status_t rc;
	if ((rc = ParseColor(parser,&color)) < RC_OK)
		return rc;
	
	if (CheckForEnd(parser) < RC_OK)
		return INVALID_VALUE;

	parser->fProperties->color = color;
	return RC_OK;
}


status_t
PropertyParser::ParseBackgroundPosition(StyleSheetParser *parser)
{
	UnitValue left,top;

	status_t rc;
	if ((rc = ParseUnit(parser,&left)) >= RC_OK)
	{
		parser->NextToken();
		ParseUnit(parser,&top);
		
		if (left.flags & ~(UNIT_LENGTH | UNIT_RELATIVE))
			return INVALID_VALUE;
	}
	else	// one or more keywords
	{
		const char *position[] = {"left","right","center","top","bottom"};			
		char *keyword[2] = {NULL,NULL};
		char *start;
		int i,a = -1,b = -1,n;
		for(i = 0;i < 2;i++)
		{
			start = parser->fCurrentChar;

			if ((rc = ParseKeyword(parser,&keyword[i])) < RC_OK)
				break;
			if ((n = FindKeyword(keyword[i],position,5)) == -1)
			{
				free(keyword[0]);	free(keyword[1]);
				parser->fCurrentChar = start;
				if (i == 0)
					return INVALID_VALUE;
				else
					break;
			}
			if (i == 0)
				a = n;
			else
				b = n;

			rc = parser->NextToken();
		}
		if (keyword[0])
		{
			if (a != -1)
			{
				left.flags = UNIT_PERCENT;
				top.flags = UNIT_PERCENT;

				if (keyword[1])
				{
					int b = FindKeyword(keyword[1],position,2);
					if (b != -1)
					{
						if (b < a)	// swap values, so that 'a' represents a horizontal value
						{
							int c = b;
							b = a;
							a = c;
						}
						switch(a)	// horizontal
						{
							case 1:
								left.value = 1.0f;
								break;
							case 2:
								left.value = 0.5f;
								break;
						}
						switch(b)	// vertical
						{
							case 4:
								top.value = 1.0f;
								break;
							case 2:
								top.value = 0.5f;
								break;
						}
						rc = RC_OK;
					}
					free(keyword[1]);	
				}
				else
				{
					switch(a)
					{
						case 1:
							left.value = 1.0f;
						case 0:
						case 2:
							top.value = 0.5f;
							break;
					}
					switch(a)
					{
						case 4:
							top.value = 1.0f;
						case 3:
						case 2:
							left.value = 0.5f;
							break;
					}
					rc = RC_OK;
				}
			}
			free(keyword[0]);
		}
	}
	if (rc < RC_OK)
		return INVALID_VALUE;

	parser->fProperties->backgroundLeft = left;
	parser->fProperties->backgroundTop = top;
	return RC_OK;
}


status_t
PropertyParser::ParseBackgroundProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	bool attachSet = false,positionSet = false,imageSet = false;
	bool colorSet = false,repeatSet = false;
	rgb_color color;

	while(!attachSet || !colorSet || !repeatSet || !positionSet || !imageSet)
	{
		char *keyword;
		int8 value;
		char *start = parser->fCurrentChar;

		if (!imageSet && ParseURL(parser,&keyword) == RC_OK)
		{
			imageSet = true;
			parser->fProperties->backgroundImage = keyword;
		}
		else if (!colorSet && ParseColor(parser,&color) == RC_OK)
		{
			colorSet = true;
		}
		else if (!positionSet && ParseBackgroundPosition(parser) == RC_OK)
		{
			positionSet = true;
			parser->fLastPropertyFlags.Set(BACKGROUND_POSITION_PROPERTY);
		}
		else if (ParseKeyword(parser,&keyword) == RC_OK)
		{
			if (!strcasecmp(keyword,"none"))
			{
				imageSet = true;
			}
			else if (!strcasecmp(keyword,"transparent"))
			{
				color.red = color.green = color.blue = color.alpha = 0;
				colorSet = true;
			}
			else if (!repeatSet && (value = FindKeyword(keyword,keywordProperties[BACKGROUND_REPEAT_KEYWORDS].keywords,keywordProperties[BACKGROUND_REPEAT_KEYWORDS].numKeywords)) != -1)
			{
				repeatSet = true;
				parser->fProperties->backgroundRepeat = value;
				parser->fLastPropertyFlags.Set(BACKGROUND_REPEAT_PROPERTY);
			}
			else if (!attachSet && (value = FindKeyword(keyword,keywordProperties[BACKGROUND_ATTACH_KEYWORDS].keywords,keywordProperties[BACKGROUND_ATTACH_KEYWORDS].numKeywords)) != -1)
			{
				attachSet = true;
				parser->fProperties->backgroundAttach = value;
				parser->fLastPropertyFlags.Set(BACKGROUND_ATTACHMENT_PROPERTY);
			}
			else
			{
				parser->fCurrentChar = start;
				free(keyword);
				break;
			}
			free(keyword);
		}
		else
			break;

		if (parser->NextToken() < RC_OK)
			return RC_IO_ERROR;
	}
	if (CheckForEnd(parser) < RC_OK)
		return INVALID_VALUE;

	if (imageSet)	
		parser->fLastPropertyFlags.Set(BACKGROUND_IMAGE_PROPERTY);
	if (colorSet)
	{
		parser->fProperties->backgroundColor = color;
		parser->fLastPropertyFlags.Set(BACKGROUND_COLOR_PROPERTY);
	}
	return RC_OK;
}


status_t
PropertyParser::ParseBackgroundColorProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	char *first = parser->fCurrentChar;
	rgb_color color;

	char *keyword;
	status_t rc = ParseKeyword(parser,&keyword);

	if (rc == RC_OK && strcasecmp(keyword,"transparent"))
		rc = INVALID_VALUE;
	free(keyword);
	
	if (rc == RC_OK)
	{
		color.red = color.green = color.blue = color.alpha = 0;
	}
	else
	{
		parser->fCurrentChar = first;

		if ((rc = ParseColor(parser,&color)) < RC_OK)
			return rc;
	}		
	if (CheckForEnd(parser) < RC_OK)
		return INVALID_VALUE;

	parser->fProperties->backgroundColor = color;
	return RC_OK;
}


status_t
PropertyParser::ParseBackgroundPositionProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	status_t rc;
	if ((rc = ParseBackgroundPosition(parser)) < RC_OK)
		return rc;

	if (CheckForEnd(parser) < RC_OK)
		return INVALID_VALUE;

	return RC_OK;
}


// #pragma mark -


template<class T> void
PropertyParser::PropagateValuesToRect(T *value, Rect<T> *target, int count)
{
	switch (count) {
		case 1:	// special case for shorthand properties
			target->top = value[0];
			target->right = value[0];
			target->bottom = value[0];
			target->left = value[0];
			return;
		case 2:
			value[2] = value[0];
		case 3:
			value[3] = value[1];
			break;
	}

	target->top = value[0];
	target->right = value[1];
	target->bottom = value[2];
	target->left = value[3];
}


template<class T> void
PropertyParser::SetRectValue(Rect<T> *rect, T *value, int index)
{
	switch(index)
	{
		case 0:
			rect->bottom = *value;
			break;
		case 1:
			rect->left = *value;
			break;
		case 2:
			rect->right = *value;
			break;
		case 3:
			rect->top = *value;
			break;
	}
}


status_t
PropertyParser::ParseBorderColorSidesProperty(StyleSheetParser *parser, uint32 property)
{
	rgb_color color;

	status_t rc;
	if ((rc = ParseColor(parser,&color)) < RC_OK)
		return rc;
	
	if ((rc = CheckForEnd(parser)) < RC_OK)
		return rc;

	SetRectValue(&parser->fProperties->borderColor, &color, property - BORDER_BOTTOM_COLOR_PROPERTY);
	parser->fLastPropertyFlags.Set(property);

	return RC_OK;
}


status_t
PropertyParser::ParseBorderColorProperty(StyleSheetParser *parser, uint32 property)
{
	rgb_color color[4];
	int i;

	// check for keyword "transparent"
	char *first = parser->fCurrentChar;
	char *keyword;
	status_t rc = ParseKeyword(parser,&keyword);

	if (rc == RC_OK && strcasecmp(keyword,"transparent"))
		rc = INVALID_VALUE;
	free(keyword);
	
	if (rc == RC_OK)	// transparent keyword
	{
		for(i = 0;i < 4;i++)
			color[i].red = color[i].green = color[i].blue = color[i].alpha = 0;

		rc = CheckForEnd(parser);
	}
	else	// check for single color values
	{
		parser->fCurrentChar = first;

		for(i = 0;i < 4;)
		{
			if ((rc = ParseColor(parser,&color[i])) < RC_OK)
				return rc;
	
			i++;
			if ((rc = CheckForEnd(parser)) == RC_OK || rc == RC_IO_ERROR)
				break;
		}
	}
	if (rc != RC_OK)
		return rc;

	PropagateValuesToRect(color,&parser->fProperties->borderColor,i);

	property = BORDER_BOTTOM_COLOR_PROPERTY;
	for (i = 0;i < 4;i++)
		parser->fLastPropertyFlags.Set(property+i);

	return RC_OK;
}


status_t
PropertyParser::ParseBorderStyleSidesProperty(StyleSheetParser *parser, uint32 property)
{
	int8 style = -1;

	status_t rc;
	if ((rc = ParseKeywordOrUnit(parser,&style,keywordsBorderStyle,NUM_KEYWORDS_BORDER_STYLE,
										  NULL,UNIT_NONE)) < RC_OK)
		return rc;

	if ((rc = CheckForEnd(parser)) < RC_OK)
		return rc;

	SetRectValue(&parser->fProperties->borderStyle,&style,property - BORDER_BOTTOM_STYLE_PROPERTY);
	parser->fLastPropertyFlags.Set(property);

	return RC_OK;
}


status_t
PropertyParser::ParseBorderStyleProperty(StyleSheetParser *parser, uint32 property)
{
	int8 style[4];
	status_t rc;
	int i = 0;

	while(i < 4)
	{
		if ((rc = ParseKeywordOrUnit(parser,&style[i],keywordsBorderStyle,NUM_KEYWORDS_BORDER_STYLE,
											  NULL,UNIT_NONE)) < RC_OK)
			return rc;

		i++;
		if ((rc = CheckForEnd(parser)) == RC_OK || rc == RC_IO_ERROR)
			break;
	}

	if (rc != RC_OK)
		return rc;

	PropagateValuesToRect(style,&parser->fProperties->borderStyle,i);

	property = BORDER_BOTTOM_STYLE_PROPERTY;
	for(i = 0;i < 4;i++)
		parser->fLastPropertyFlags.Set(property+i);

	return RC_OK;
}


status_t
PropertyParser::ParseBorderWidth(StyleSheetParser *parser, UnitValue *length)
{
	int8 keyword;

	status_t rc;
	if ((rc = ParseKeywordOrUnit(parser,&keyword,keywordsBorderWidth,NUM_KEYWORDS_BORDER_WIDTH,
										  length,UNIT_LENGTH)) < RC_OK)
		return rc;

	if (keyword != -1)	// keyword found
	{
		length->value = gBorderSizes[keyword];
		length->flags = UNIT_METRIC;
	}
	return RC_OK;
}


status_t
PropertyParser::ParseBorderWidthSidesProperty(StyleSheetParser *parser, uint32 property)
{
	UnitValue length;

	status_t rc;
	if ((rc = ParseBorderWidth(parser,&length)) < RC_OK)
		return rc;

	if (CheckForEnd(parser) < RC_OK)
		return INVALID_VALUE;

	SetRectValue(&parser->fProperties->border,&length,property - BORDER_BOTTOM_WIDTH_PROPERTY);
	parser->fLastPropertyFlags.Set(property);

	return RC_OK;
}


status_t
PropertyParser::ParseBorderWidthProperty(StyleSheetParser *parser, uint32 property)
{
	UnitValue value[4];
	status_t rc = RC_OK;
	int i = 0;
	
	while(i < 4)
	{
		if ((rc = ParseBorderWidth(parser,&value[i])) < RC_OK)
			return rc;

		i++;
		if ((rc = CheckForEnd(parser)) == RC_OK || rc == RC_IO_ERROR)
			break;
	}
	if (rc != RC_OK)
		return rc;

	PropagateValuesToRect(value,&parser->fProperties->border,i);

	property = BORDER_BOTTOM_WIDTH_PROPERTY;
	for(i = 0;i < 4;i++)
		parser->fLastPropertyFlags.Set(property+i);

	return RC_OK;
}


status_t
PropertyParser::ParseBorderProperty(StyleSheetParser *parser, uint32 property)
{
	bool colorSet = false,widthSet = false,styleSet = false;
	UnitValue width;
	rgb_color color;
	int8 style;
	status_t rc;

	while (!(colorSet && widthSet && styleSet)) {
		if (!colorSet && (rc = ParseColor(parser,&color)) >= RC_OK)
			colorSet = true;
		else if (!widthSet && (rc = ParseBorderWidth(parser,&width)) >= RC_OK)
			widthSet = true;
		else if (!styleSet && (rc = ParseKeywordOrUnit(parser,&style,keywordsBorderStyle,NUM_KEYWORDS_BORDER_STYLE,
											  NULL,UNIT_NONE)) >= RC_OK)
			styleSet = true;
		else
			return INVALID_VALUE;

		if ((rc = CheckForEnd(parser)) == RC_OK || rc == RC_IO_ERROR)
			break;
	}
	if (rc < RC_OK || !colorSet && !widthSet && !styleSet)
		return INVALID_VALUE;

	Properties *properties = parser->fProperties;

	int side = 0;
	switch (property) {
		case BORDER_PROPERTY:
			if (colorSet)
			{
				PropagateValuesToRect(&color,&properties->borderColor,1);
				
				property = BORDER_BOTTOM_COLOR_PROPERTY;
				for(int i = 0;i < 4;i++)
					parser->fLastPropertyFlags.Set(property+i);
			}
			if (styleSet)
			{
				PropagateValuesToRect(&style,&properties->borderStyle,1);
				
				property = BORDER_BOTTOM_STYLE_PROPERTY;
				for(int i = 0;i < 4;i++)
					parser->fLastPropertyFlags.Set(property+i);
			}
			if (widthSet)
			{
				PropagateValuesToRect(&width,&properties->border,1);
				
				property = BORDER_BOTTOM_WIDTH_PROPERTY;
				for(int i = 0;i < 4;i++)
					parser->fLastPropertyFlags.Set(property+i);
			}
			break;
		case BORDER_TOP_PROPERTY:
			side++;
		case BORDER_RIGHT_PROPERTY:
			side++;
		case BORDER_LEFT_PROPERTY:
			side++;
		case BORDER_BOTTOM_PROPERTY:
			if (colorSet)
			{
				SetRectValue(&properties->borderColor,&color,side);
				parser->fLastPropertyFlags.Set(BORDER_BOTTOM_COLOR_PROPERTY + side);
			}
			if (styleSet)
			{
				SetRectValue(&properties->borderStyle,&style,side);
				parser->fLastPropertyFlags.Set(BORDER_BOTTOM_STYLE_PROPERTY + side);
			}
			if (widthSet)
			{
				SetRectValue(&properties->border,&width,side);
				parser->fLastPropertyFlags.Set(BORDER_BOTTOM_STYLE_PROPERTY + side);
			}
			break;
	}
	return RC_OK;
}


status_t
PropertyParser::ParseBorderSpacingProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	UnitValue value[2];
	status_t rc;
	int i;

	for(i = 0;i < 2;i++)
	{
		if ((rc = ParseLength(parser,&value[i])) < RC_OK)
			return INVALID_VALUE;

		if ((rc = CheckForEnd(parser)) == RC_OK || rc == RC_IO_ERROR)
			break;
	}
	if (rc < RC_OK)
		return INVALID_VALUE;

	if (i == 0)	// horizontal equals vertical
		value[1] = value[0];

	parser->fProperties->horizontalBorderSpacing = value[0];
	parser->fProperties->verticalBorderSpacing = value[1];

	return RC_OK;
}


status_t
PropertyParser::ParseAutoWidth(StyleSheetParser *parser, UnitValue *length,
	bool padding, bool percent = true)
{
	int8 keyword = -1;
	status_t rc;
	if ((rc = ParseKeywordOrUnit(parser,!padding ? &keyword : NULL,keywordsAuto,NUM_KEYWORDS_AUTO,
										  length,(percent ? UNIT_PERCENT : 0) | UNIT_LENGTH)) < RC_OK)
		return rc;

	if (keyword != -1)
		length->flags = UNIT_AUTO;
	
	return RC_OK;
}


status_t
PropertyParser::ParseMarginPaddingOffsetSizeProperty(StyleSheetParser *parser, uint32 property)
{
	bool padding = (property >= PADDING_PROPERTY && property <= PADDING_TOP_PROPERTY);
	UnitValue length;
	
	status_t rc;
	if ((rc = ParseAutoWidth(parser,&length,padding)) < RC_OK ||
		(rc = CheckForEnd(parser)) < RC_OK)
		return INVALID_VALUE;

	Properties *properties = parser->fProperties;

	switch(property)
	{
		case MARGIN_LEFT_PROPERTY:
			properties->margin.left = length;
			break;
		case MARGIN_RIGHT_PROPERTY:
			properties->margin.right = length;
			break;
		case MARGIN_TOP_PROPERTY:
			properties->margin.top = length;
			break;
		case MARGIN_BOTTOM_PROPERTY:
			properties->margin.bottom = length;
			break;

		case PADDING_LEFT_PROPERTY:
			properties->padding.left = length;
			break;
		case PADDING_RIGHT_PROPERTY:
			properties->padding.right = length;
			break;
		case PADDING_TOP_PROPERTY:
			properties->padding.top = length;
			break;
		case PADDING_BOTTOM_PROPERTY:
			properties->padding.bottom = length;
			break;

		case WIDTH_PROPERTY:
			properties->width = length;
			break;
		case HEIGHT_PROPERTY:
			properties->height = length;
			break;

		case LEFT_PROPERTY:
			properties->boxOffset.left = length;
			break;
		case RIGHT_PROPERTY:
			properties->boxOffset.right = length;
			break;
		case TOP_PROPERTY:
			properties->boxOffset.top = length;
			break;
		case BOTTOM_PROPERTY:
			properties->boxOffset.bottom = length;
			break;
	}
	return RC_OK;
}


status_t
PropertyParser::ParseMarginPaddingProperty(StyleSheetParser *parser, uint32 property)
{
	bool padding = (property >= PADDING_PROPERTY && property <= PADDING_TOP_PROPERTY);
	UnitValue value[4];
	status_t rc = RC_OK;
	int i = 0;
	
	while(i < 4)
	{
		if ((rc = ParseAutoWidth(parser,&value[i],padding)) < RC_OK)
			return rc;

		i++;
		if ((rc = CheckForEnd(parser)) == RC_OK || rc == RC_IO_ERROR)
			break;
	}
	if (rc < RC_OK)
		return rc;

	if (padding)
	{
		PropagateValuesToRect(value,&parser->fProperties->padding,i);
		property = PADDING_BOTTOM_PROPERTY;
	}
	else
	{
		PropagateValuesToRect(value,&parser->fProperties->margin,i);
		property = MARGIN_BOTTOM_PROPERTY;
	}
	for(i = 0;i < 4;i++)
		parser->fLastPropertyFlags.Set(property+i);

	return RC_OK;
}


// #pragma mark -


status_t
PropertyParser::ParseOutlineWidthProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	UnitValue length;

	status_t rc;
	if ((rc = ParseBorderWidth(parser,&length)) < RC_OK)
		return rc;

	if (CheckForEnd(parser) < RC_OK)
		return INVALID_VALUE;

	parser->fProperties->outlineWidth = length;
	return RC_OK;
}


status_t
PropertyParser::ParseOutlineColorProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	rgb_color color;

	if (!TestForKeyword(parser,"invert"))
	{
		status_t rc;
		if ((rc = ParseColor(parser,&color)) < RC_OK)
			return rc;
	}
	else	// special inverted color
		color.red = color.green = color.blue = color.alpha = 0;

	if (CheckForEnd(parser) < RC_OK)
		return INVALID_VALUE;

	parser->fProperties->outlineColor = color;
	return RC_OK;
}


status_t
PropertyParser::ParseOutlineProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	bool colorSet = false,widthSet = false,styleSet = false;
	UnitValue width;
	rgb_color color;
	int8 style;
	status_t rc;

	while(!(colorSet && widthSet && styleSet))
	{
		if (!colorSet && TestForKeyword(parser,"invert"))
		{
			// special inverted color
			color.red = color.green = color.blue = color.alpha = 0;
			colorSet = true;
		}
		else if (!colorSet && (rc = ParseColor(parser,&color)) >= RC_OK)
			colorSet = true;
		else if (!widthSet && (rc = ParseBorderWidth(parser,&width)) >= RC_OK)
			widthSet = true;
		else if (!styleSet && (rc = ParseKeywordOrUnit(parser,&style,keywordsBorderStyle,NUM_KEYWORDS_BORDER_STYLE,
											  NULL,UNIT_NONE)) >= RC_OK)
			styleSet = true;
		else
			return INVALID_VALUE;

		if ((rc = CheckForEnd(parser)) == RC_OK || rc == RC_IO_ERROR)
			break;
	}
	if (rc < RC_OK || !colorSet && !widthSet && !styleSet)
		return INVALID_VALUE;

	if (colorSet)
	{
		parser->fProperties->outlineColor = color;
		parser->fLastPropertyFlags.Set(OUTLINE_COLOR_PROPERTY);
	}
	if (styleSet)
	{
		parser->fProperties->outlineStyle = style;
		parser->fLastPropertyFlags.Set(OUTLINE_STYLE_PROPERTY);
	}
	if (widthSet)
	{
		parser->fProperties->outlineWidth = width;
		parser->fLastPropertyFlags.Set(OUTLINE_WIDTH_PROPERTY);
	}
	return RC_OK;
}


status_t
PropertyParser::ParseListStyleProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	bool typeSet = false,positionSet = false,imageSet = false;
	int8 type,position;

	while(!typeSet || !positionSet || !imageSet)
	{
		char *keyword;
		char *start = parser->fCurrentChar;

		if (!imageSet && ParseURL(parser,&keyword) == RC_OK)
		{
			imageSet = true;
			parser->fProperties->listStyleImage = keyword;
		}
		else if (ParseKeyword(parser,&keyword) == RC_OK)
		{
			if ((!imageSet || !typeSet) && (type = FindKeyword(keyword,keywordProperties[LIST_STYLE_TYPE_KEYWORDS].keywords,keywordProperties[LIST_STYLE_TYPE_KEYWORDS].numKeywords)) != -1)
			{
				if (type == 0 && !imageSet)	// none applies to images and types
				{
					if (typeSet)
						imageSet = true;
				}
				else
				{
					typeSet = true;
					parser->fProperties->listStyleType = type;
					parser->fLastPropertyFlags.Set(LIST_STYLE_TYPE_PROPERTY);
				}
			}
			else if (!positionSet && (position = FindKeyword(keyword,keywordProperties[LIST_STYLE_POSITION_KEYWORDS].keywords,keywordProperties[LIST_STYLE_POSITION_KEYWORDS].numKeywords)) != -1)
			{
				positionSet = true;
				parser->fProperties->listStylePosition = position;
				parser->fLastPropertyFlags.Set(LIST_STYLE_POSITION_PROPERTY);
			}
			else
			{
				parser->fCurrentChar = start;
				free(keyword);
				break;
			}
			free(keyword);
		}
		else
			break;

		if (parser->NextToken() < RC_OK)
			return RC_IO_ERROR;
	}
	if (CheckForEnd(parser) < RC_OK)
		return INVALID_VALUE;

	if (imageSet)	
		parser->fLastPropertyFlags.Set(LIST_STYLE_IMAGE_PROPERTY);

	return RC_OK;
}


status_t
PropertyParser::ParseMinMaxSizesProperty(StyleSheetParser *parser, uint32 property)
{
	UnitValue length;

	if (property < MIN_HEIGHT_PROPERTY && TestForKeyword(parser, "none"))
		length.flags = UNIT_NORMAL;
	else if (ParseUnit(parser,&length) < RC_OK || !(length.flags & (UNIT_LENGTH | UNIT_PERCENT)))
		return INVALID_VALUE;

	if (CheckForEnd(parser) < RC_OK)
		return INVALID_VALUE;

	switch(property)
	{
		case MIN_WIDTH_PROPERTY:
			parser->fProperties->minWidth = length;
			break;
		case MAX_WIDTH_PROPERTY:
			parser->fProperties->maxWidth = length;
			break;
		case MIN_HEIGHT_PROPERTY:
			parser->fProperties->minHeight = length;
			break;
		case MAX_HEIGHT_PROPERTY:
			parser->fProperties->maxHeight = length;
			break;
	}
	return RC_OK;

}


status_t
PropertyParser::ParseMarkerOffsetProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	UnitValue length;

	status_t rc;
	if ((rc = ParseAutoWidth(parser,&length,false,false)) < RC_OK)
		return rc;

	if ((rc = CheckForEnd(parser)) < RC_OK)
		return rc;

	parser->fProperties->markerOffset = length;
	return RC_OK;
}


status_t
PropertyParser::ParseZIndexWidowsOrphansProperty(StyleSheetParser *parser, uint32 property)
{
	float number;
	uint8 type;

	if (property == Z_INDEX_PROPERTY && TestForKeyword(parser,"auto"))
		number = -1.0f;
	else if (ParseNumber(parser,&number,&type) < RC_OK || type == FLOAT_NUMBER)
		return INVALID_VALUE;
	
	if (CheckForEnd(parser) < RC_OK)
		return INVALID_VALUE;

	switch(property)
	{
		case Z_INDEX_PROPERTY:
			parser->fProperties->zIndex = (int)number;
			break;
		case WIDOWS_PROPERTY:
			parser->fProperties->widows = (int)number;
			break;
		case ORPHANS_PROPERTY:
			parser->fProperties->orphans = (int)number;
			break;
	}
	return RC_OK;
}


status_t
PropertyParser::ParsePageProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	char *identifier;
	status_t rc;

	if ((rc = ParseKeyword(parser,&identifier)) < RC_OK)
		return rc;
	
	if (!strcasecmp(identifier,"auto"))
	{
		free(identifier);
		identifier = NULL;
	}

	parser->fProperties->pageIdentifier = identifier;
	return RC_OK;
}


status_t
PropertyParser::ParseQuotesProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	status_t rc;

	if (TestForKeyword(parser,"none"))
		return CheckForEnd(parser);

	while(true)
	{
		char *openQuote,*closeQuote;
		
		if (ParseString(parser,&openQuote) == RC_OK)
		{
			if ((rc = parser->NextToken()) < RC_OK || (rc = ParseString(parser,&closeQuote)) < RC_OK)
			{
				free(openQuote);
				return rc;
			}
			parser->fProperties->quotes.AddItem(openQuote);
			parser->fProperties->quotes.AddItem(closeQuote);
		}
		else
			return INVALID_VALUE;

		if ((rc = CheckForEnd(parser)) == RC_OK || rc == RC_IO_ERROR)
			break;
	}
	return rc;
}


status_t
PropertyParser::ParseMarksProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	if (TestForKeyword(parser,"none"))
		return CheckForEnd(parser);

	bool cropSet = false,crossSet = false;
	status_t rc;

	while(!cropSet || !crossSet)
	{
		if (TestForKeyword(parser,"cross"))
			crossSet = true;
		else if (TestForKeyword(parser,"crop"))
			cropSet = true;

		if ((rc = CheckForEnd(parser)) == RC_OK || rc == RC_IO_ERROR)
			break;
	}
	if (rc < RC_OK || !cropSet && !crossSet)
		return INVALID_VALUE;

	parser->fProperties->marks = (crossSet ? CROSS_MARKS : 0) | (cropSet ? CROP_MARKS : 0);
	return RC_OK;
}


status_t
PropertyParser::ParseCounterResetIncrementProperty(StyleSheetParser *parser, uint32 property)
{
	status_t rc;

	if (TestForKeyword(parser,"none"))
		return CheckForEnd(parser);

	while(true)
	{
		char *name;
		float value;
		uint8 type;
		
		if (ParseKeyword(parser,&name) == RC_OK)
		{
			value = 0.0f;
			if ((rc = parser->NextToken()) < RC_OK || ((rc = ParseNumber(parser,&value,&type)) == RC_OK && type == FLOAT_NUMBER))
			{
				free(name);
				return rc;
			}
			Counter *counter;

			if ((counter = (Counter *)parser->fProperties->counter.GetValue(name)) == NULL)
			{
				counter = new Counter(name);
				parser->fProperties->counter.Put(name,counter);
			}
			if (property == COUNTER_RESET_PROPERTY)
				counter->value = (int32)value;
			else
				counter->value += (int32)value;

			rc = RC_OK;
		}
		else
			return INVALID_VALUE;

		if ((rc = CheckForEnd(parser)) == RC_OK || rc == RC_IO_ERROR)
			break;
	}
	return rc;
}


status_t
PropertyParser::ParseCounter(StyleSheetParser *parser,ContentType *ct,bool counters)
{
	if (*parser->fCurrentChar != '(')
		return INVALID_VALUE;
	parser->fCurrentChar++;

	status_t rc;
	if ((rc = parser->NextToken()) < RC_OK || (rc = ParseKeyword(parser,&ct->string)) < RC_OK)
		return rc;

	if (counters)
	{
		if ((rc = parser->NextToken()) < RC_OK || *parser->fCurrentChar != ',')
			return INVALID_VALUE;
		parser->fCurrentChar++;
	
		if ((rc = parser->NextToken()) < RC_OK || (rc = ParseString(parser,&ct->delimiter)) < RC_OK)
			return rc;
	}

	if ((rc = parser->NextToken()) < RC_OK)
		return rc;

	ct->style = LIST_STYLE_DECIMAL;
	if (*parser->fCurrentChar == ',')
	{
		parser->fCurrentChar++;

		if ((rc = parser->NextToken()) < RC_OK)
			return rc;

		if ((rc = ParseKeywordOrUnit(parser,&ct->style,keywordProperties[LIST_STYLE_TYPE_KEYWORDS].keywords,keywordProperties[LIST_STYLE_TYPE_KEYWORDS].numKeywords,
										  NULL,UNIT_NONE)) < RC_OK)
			return INVALID_VALUE;
	}

	if ((rc = parser->NextToken()) < RC_OK || *parser->fCurrentChar != ')')
		return INVALID_VALUE;
	parser->fCurrentChar++;

	if (counters)
		ct->type = CONTENT_COUNTERS_TYPE;
	else
		ct->type = CONTENT_COUNTER_TYPE;
	
	return RC_OK;
}


status_t
PropertyParser::ParseContentProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	status_t rc;

	while(true)
	{
		ContentType ct;

		if (ParseString(parser,&ct.string) == RC_OK)
		{
			ct.type = CONTENT_STRING_TYPE;
		}
		else if (ParseURL(parser,&ct.string) == RC_OK)
		{
			ct.type = CONTENT_URI_TYPE;
		}
		else if (TestForKeyword(parser,"counter"))
		{
			if ((rc = ParseCounter(parser,&ct,false)) < RC_OK)
				return rc;
		}
		else if (TestForKeyword(parser,"counters"))
		{
			if ((rc = ParseCounter(parser,&ct,true)) < RC_OK)
				return rc;
		}
		else if (TestForKeyword(parser,"attr"))
		{
			if (*parser->fCurrentChar != '(')
				return INVALID_VALUE;
			parser->fCurrentChar++;

			if ((rc = parser->NextToken()) < RC_OK || (rc = ParseKeyword(parser,&ct.string)) < RC_OK)
				return rc;

			if ((rc = parser->NextToken()) < RC_OK || *parser->fCurrentChar != ')')
				return INVALID_VALUE;
			parser->fCurrentChar++;

			ct.type = CONTENT_ATTR_TYPE;
		}
		else if ((rc = ParseKeywordOrUnit(parser,&ct.type,keywordsContent,NUM_KEYWORDS_CONTENT,
										  NULL,UNIT_NONE)) < RC_OK)
			return INVALID_VALUE;

		// copy content type
		ContentType *cct = new ContentType();
		cct->string = ct.string;
		cct->delimiter = ct.delimiter;
		cct->style = ct.style;
		cct->type = ct.type;
		parser->fProperties->content.AddItem(cct);

		if ((rc = CheckForEnd(parser)) == RC_OK || rc == RC_IO_ERROR)
			break;
	}
	return rc;
}


status_t
PropertyParser::ParseClipProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	status_t rc;

	if (TestForKeyword(parser,"auto"))
	{
		parser->fProperties->clip.top.flags = UNIT_AUTO;
		parser->fProperties->clip.left.flags = UNIT_AUTO;
		parser->fProperties->clip.right.flags = UNIT_AUTO;
		parser->fProperties->clip.bottom.flags = UNIT_AUTO;
	}
	else if ((rc = ParseShape(parser,&parser->fProperties->clip)) < RC_OK)
		return rc;

	return CheckForEnd(parser);
}


status_t
PropertyParser::ParseCursorProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	status_t rc;

	while(true)
	{
		char *url;
		if ((rc = ParseURL(parser,&url)) < RC_OK)
			break;
		
		if ((rc = parser->NextToken()) < RC_OK || *parser->fCurrentChar != ',')
			return INVALID_VALUE;
		
		parser->fCurrentChar++;
		parser->fProperties->cursorURLs.AddItem(url);

		if ((rc = parser->NextToken()) < RC_OK)
			return rc;
	}
	int8 cursor = -1;
	if ((rc = ParseKeywordOrUnit(parser,&cursor,keywordsCursor,NUM_KEYWORDS_CURSOR,
										  NULL,UNIT_NONE)) < RC_OK)
		return rc;

	if ((rc = CheckForEnd(parser)) < RC_OK)
		return rc;

	parser->fProperties->cursor = cursor;
	return RC_OK;
}


status_t
PropertyParser::ParseSizeProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	status_t rc = RC_OK;
	UnitValue value[2];
	int i = 0;

	if (TestForKeyword(parser,"auto"))
		value[0].flags = UNIT_AUTO;
	else if (TestForKeyword(parser,"portrait"))
		value[0].flags = UNIT_PORTRAIT;
	else if (TestForKeyword(parser,"landscape"))
		value[0].flags = UNIT_LANDSCAPE;
	else
	{
		for(;i < 2;i++)
		{
			if ((rc = ParseLength(parser,&value[i])) < RC_OK)
				return INVALID_VALUE;
	
			if ((rc = CheckForEnd(parser)) == RC_OK || rc == RC_IO_ERROR)
				break;
		}
	}
	if (i == 0)	// horizontal equals vertical
	{
		if (value[0].flags != UNIT_NONE)	// there was a keyword
			rc = CheckForEnd(parser);
		value[1] = value[0];
	}
	if (rc < RC_OK)
		return INVALID_VALUE;

	parser->fProperties->horizontalSize = value[0];
	parser->fProperties->verticalSize = value[1];

	return RC_OK;
}


// #pragma mark -

status_t
PropertyParser::ParseFontFamily(StyleSheetParser *parser)
{
	status_t rc;

	while(true)
	{
		char *name;
		bool string = false;

		if ((rc = ParseString(parser,&name)) >= RC_OK)
			string = true;
		else if ((rc = ParseLongName(parser,&name)) < RC_OK)
			break;
	
		FontFamily *ff = new FontFamily();

		int8 generic;
		if (!string && (generic = FindKeyword(name,genericFontFamilies,NUM_GENERIC_FONT_FAMILIES)) != -1)
		{
			ff->generic = generic;
			free(name);
		}
		else
			ff->name = name;

		parser->fProperties->fontFamilies.AddItem(ff);

		if ((rc = parser->NextToken()) < RC_OK)
			break;
		
		if (*parser->fCurrentChar == ',')
		{
			parser->fCurrentChar++;
			if ((rc = parser->NextToken()) < RC_OK)
				break;
		}
		else
			break;
	}
	return rc;
}


status_t
PropertyParser::ParseFontSize(StyleSheetParser *parser)
{
	UnitValue length;
	int8 keyword;

	const char *fontSizes[] = {"xx-small","x-small","small","medium","large","x-large","xx-large",
							   "smaller","larger"};

	status_t rc;
	if ((rc = ParseKeywordOrUnit(parser,&keyword,fontSizes,9,&length,UNIT_PERCENT | UNIT_LENGTH)) < RC_OK)
		return rc;

	if (keyword != -1)	// keyword found
	{
		if (keyword < 7)	// absolute font size
		{
			length.value = pow(gFontSizeFactor,(keyword - 3)) * gMediumFontSizeBase;
			length.flags = UNIT_PT;
		}
		else if (keyword == 7)	// relative size
			length.flags = UNIT_SMALLER;
		else
			length.flags = UNIT_LARGER;
	}

	parser->fProperties->fontSize = length;
	return RC_OK;
}


status_t
PropertyParser::ParseFontWeight(StyleSheetParser *parser)
{
	int8 weight;

	status_t rc;
	char *keyword,*start = parser->fCurrentChar;
	if ((rc = ParseKeyword(parser,&keyword,false)) < RC_OK)
		return rc;

	weight = FindKeyword(keyword,keywordsFontWeight,NUM_KEYWORDS_FONT_WEIGHT);
	free(keyword);

	if (weight == -1)
	{
		parser->fCurrentChar = start;
		return UNKNOWN_KEYWORD;
	}
	parser->fProperties->fontWeight = weight;
	return RC_OK;
}


status_t
PropertyParser::ParseFontFamilyProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	status_t rc;
	if ((rc = ParseFontFamily(parser)) < RC_OK)
		return rc;

	if (CheckForEnd(parser) < RC_OK)
		return INVALID_VALUE;
	
	return RC_OK;
}


status_t
PropertyParser::ParseFontProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	int8 systemFont;
	status_t rc;
	if ((rc = ParseKeywordOrUnit(parser,&systemFont,keywordsFont,NUM_KEYWORDS_FONT,
										NULL,UNIT_NONE)) == RC_OK)
	{
		parser->fProperties->systemFont = systemFont;
		return RC_OK;
	}
	bool weightSet = false,variantSet = false,styleSet = false;
	int8 variant,style;

	while(!weightSet || !variantSet || !styleSet)
	{
		if (parser->NextToken() < RC_OK)
			return RC_IO_ERROR;

		char *keyword;
		char *start = parser->fCurrentChar;

		if (!weightSet && ParseFontWeight(parser) == RC_OK)
		{
			weightSet = true;
			parser->fLastPropertyFlags.Set(FONT_WEIGHT_PROPERTY);
		}
		else if ((!variantSet || !styleSet) && ParseKeyword(parser,&keyword) == RC_OK)
		{
			if (!variantSet && (variant = FindKeyword(keyword,keywordProperties[FONT_VARIANT_KEYWORDS].keywords,keywordProperties[FONT_VARIANT_KEYWORDS].numKeywords)) != -1)
			{
				variantSet = true;
				parser->fLastPropertyFlags.Set(FONT_VARIANT_PROPERTY);
			}
			else if (!styleSet && (style = FindKeyword(keyword,keywordProperties[FONT_STYLE_KEYWORDS].keywords,keywordProperties[FONT_STYLE_KEYWORDS].numKeywords)) != -1)
			{
				styleSet = true;
				parser->fLastPropertyFlags.Set(FONT_STYLE_PROPERTY);
			}
			else
			{
				parser->fCurrentChar = start;
				free(keyword);
				break;
			}
			free(keyword);
		}
		else
			break;
	}
	if ((rc = ParseFontSize(parser)) < RC_OK)
		return rc;
	parser->fLastPropertyFlags.Set(FONT_SIZE_PROPERTY);

	// initialize font values (according to the CSS2 spec)
	parser->fProperties->fontSizeAdjusted = false;
	parser->fProperties->fontStretch = 0;
	
	if (*parser->fCurrentChar == '/')	// parse line height
	{
		parser->fCurrentChar++;

		UnitValue length;
		int8 keyword;
	
		if ((rc = ParseKeywordOrUnit(parser,&keyword,keywordsNormal,NUM_KEYWORDS_NORMAL,
											&length,UNIT_PERCENT | UNIT_LENGTH | UNIT_NOT_SPECIFIED)) < RC_OK)
			return rc;
	
		if (keyword != -1)	// keyword found
			length.flags = UNIT_NORMAL;
	
		parser->fProperties->lineHeight = length;
		parser->fLastPropertyFlags.Set(LINE_HEIGHT_PROPERTY);
	}

	if (parser->NextToken() < RC_OK)
		return RC_IO_ERROR;

	ParseFontFamily(parser);
	parser->fLastPropertyFlags.Set(FONT_FAMILY_PROPERTY);

	if (CheckForEnd(parser) < RC_OK)
		return INVALID_VALUE;

	return RC_OK;
}


status_t
PropertyParser::ParseFontWeightProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	status_t rc;
	if ((rc = ParseFontWeight(parser)) < RC_OK)
		return rc;

	if (CheckForEnd(parser) < RC_OK)
		return INVALID_VALUE;

	return RC_OK;
}


status_t
PropertyParser::ParseFontSizeProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	status_t rc;
	if ((rc = ParseFontSize(parser)) < RC_OK)
		return rc;

	if (CheckForEnd(parser) < RC_OK)
		return INVALID_VALUE;

	return RC_OK;
}


status_t
PropertyParser::ParseFontSizeAdjustProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	bool adjust = true;
	float number;

	status_t rc;
	if ((rc = ParseNumber(parser,&number)) < RC_OK)
	{
		if (!TestForKeyword(parser,"none"))
			return INVALID_VALUE;

		adjust = false;
	}
	if (CheckForEnd(parser) < RC_OK)
		return INVALID_VALUE;

	if (adjust)
	{
		parser->fProperties->fontSizeAdjusted = true;
		parser->fProperties->fontSizeAdjust = number;
	}
	else
		parser->fProperties->fontSizeAdjusted = false;

	return RC_OK;
}


// #pragma mark -


status_t
PropertyParser::ParseWordLetterSpacingProperty(StyleSheetParser *parser, uint32 property)
{
	UnitValue length;
	int8 keyword;

	status_t rc;
	if ((rc = ParseKeywordOrUnit(parser,&keyword,keywordsNormal,NUM_KEYWORDS_NORMAL,
										  &length,UNIT_LENGTH)) < RC_OK)
		return rc;

	if (keyword != -1)	// keyword found
		length.flags = UNIT_NORMAL;

	if (CheckForEnd(parser) < RC_OK)
		return INVALID_VALUE;

	if (property == LETTER_SPACING_PROPERTY)
		parser->fProperties->letterSpacing = length;
	else	// WORD_SPACING
		parser->fProperties->wordSpacing = length;

	return RC_OK;
}


status_t
PropertyParser::ParseTextDecorationProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	int8 keyword = -1,deco = 0;
	status_t rc;
	
	do
	{
		if ((rc = ParseKeywordOrUnit(parser,&keyword,keywordsTextDeco,NUM_KEYWORDS_TEXT_DECO,
											  NULL,UNIT_NONE)) == RC_OK)
		{
			rc = parser->NextToken();

			if (keyword == 0)
			{
				if (deco != 0)
					return INVALID_VALUE;
				break;
			}
			else
				deco |= 1 << (keyword-1);
		}
	} while(rc == RC_OK);

	if (rc == UNKNOWN_KEYWORD)
		return UNKNOWN_KEYWORD;

	if (CheckForEnd(parser) < RC_OK)
		return INVALID_VALUE;

	parser->fProperties->textDecoration = deco;
	return RC_OK;
}


status_t
PropertyParser::ParseVerticalAlignProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	UnitValue length;
	int8 keyword = -1;

	status_t rc;
	if ((rc = ParseKeywordOrUnit(parser,&keyword,keywordsVerticalAlign,NUM_KEYWORDS_VERTICAL_ALIGN,
										  &length,UNIT_PERCENT)) < RC_OK)
		return rc;

	// only percent values are permitted in CSS1
	if (keyword == -1 && !(length.flags & UNIT_PERCENT))
		return INVALID_VALUE;

	if (CheckForEnd(parser) < RC_OK)
		return INVALID_VALUE;

	parser->fProperties->verticalAlignType = (uint8)keyword;
	if (keyword == -1)
		parser->fProperties->verticalAlign = length;

	return RC_OK;
}


status_t
PropertyParser::ParseTextAlignProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	char *string = NULL;
	int8 keyword = -1;

	status_t rc;
	if ((rc = ParseString(parser,&string)) < RC_OK &&
		(rc = ParseKeywordOrUnit(parser,&keyword,keywordsTextAlign,NUM_KEYWORDS_TEXT_ALIGN,
										  NULL,UNIT_NONE)) < RC_OK)
		return rc;

	if (CheckForEnd(parser) < RC_OK)
		return INVALID_VALUE;

	parser->fProperties->textAlignString = string;
	parser->fProperties->textAlign = keyword;

	return RC_OK;
}


status_t
PropertyParser::ParseTextIndentProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	UnitValue length;

	status_t rc;
	if ((rc = ParseLength(parser,&length)) < RC_OK)
		return rc;
	
	if (CheckForEnd(parser) < RC_OK)
		return INVALID_VALUE;

	parser->fProperties->textIndent = length;
	return RC_OK;
}


status_t
PropertyParser::ParseTextShadowProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	status_t rc;

	if (TestForKeyword(parser,"none"))
		return CheckForEnd(parser);

	bool colorSet = false;

	while(true)
	{
		UnitValue horizontal,vertical,radius;
		rgb_color color;
		
		if (ParseColor(parser,&color) == RC_OK)
			colorSet = true;
		else if (ParseLength(parser,&horizontal) == RC_OK)
		{
			if ((rc = parser->NextToken()) < RC_OK || (rc = ParseLength(parser, &vertical)) < RC_OK)
				return rc;

			if ((rc = parser->NextToken()) < RC_OK)
				return rc;

			ParseLength(parser,&radius);
		}
		else
			return INVALID_VALUE;

		if ((rc = CheckForEnd(parser)) == RC_OK || rc == RC_IO_ERROR)
			break;
		if (*parser->fCurrentChar == ',')
		{
			parser->fCurrentChar++;

			TextShadow *shadow = new TextShadow();
			shadow->color = color;
			shadow->colorSet = colorSet;
			colorSet = false;
			
			shadow->horizontal = horizontal;
			shadow->vertical = vertical;
			shadow->radius = radius;

			parser->fProperties->textShadow.AddItem(shadow);
		}
		if ((rc = parser->NextToken()) < RC_OK)
			return rc;
	}
	return rc;
}


status_t
PropertyParser::ParseLineHeightProperty(StyleSheetParser *parser, uint32 /*property*/)
{
	UnitValue length;
	int8 keyword;

	status_t rc;
	if ((rc = ParseKeywordOrUnit(parser,&keyword,keywordsNormal,NUM_KEYWORDS_NORMAL,
										  &length,UNIT_PERCENT | UNIT_LENGTH | UNIT_NOT_SPECIFIED)) < RC_OK)
		return rc;

	if (keyword != -1)	// keyword found
		length.flags = UNIT_NORMAL;

	if (CheckForEnd(parser) < RC_OK)
		return INVALID_VALUE;

	parser->fProperties->lineHeight = length;
	return RC_OK;
}

