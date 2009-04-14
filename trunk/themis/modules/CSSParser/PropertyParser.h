#ifndef PROPERTY_PARSER_H
#define PROPERTY_PARSER_H
/* PropertyParser.h - reads and parses CSS properties
**
** Copyright 2001, pinc Software. All Rights Reserved.
*/

#include "Types.h"
#include "StyleSheet.h"
#include "Properties.h"

class StyleSheetParser;
class AtRuleParser;


enum known_properties
{
	AZIMUTH_PROPERTY = 0,
	BACKGROUND_PROPERTY, BACKGROUND_ATTACHMENT_PROPERTY, BACKGROUND_COLOR_PROPERTY,
	BACKGROUND_IMAGE_PROPERTY, BACKGROUND_POSITION_PROPERTY,
	BACKGROUND_REPEAT_PROPERTY,

	BORDER_PROPERTY,
	BORDER_BOTTOM_PROPERTY, BORDER_BOTTOM_COLOR_PROPERTY, BORDER_BOTTOM_STYLE_PROPERTY,
	BORDER_BOTTOM_WIDTH_PROPERTY,
	BORDER_COLLAPSE_PROPERTY,
	BORDER_COLOR_PROPERTY,
	BORDER_LEFT_PROPERTY, BORDER_LEFT_COLOR_PROPERTY, BORDER_LEFT_STYLE_PROPERTY,
	BORDER_LEFT_WIDTH_PROPERTY,
	BORDER_RIGHT_PROPERTY, BORDER_RIGHT_COLOR_PROPERTY, BORDER_RIGHT_STYLE_PROPERTY,
	BORDER_RIGHT_WIDTH_PROPERTY,
	BORDER_SPACING_PROPERTY,
	BORDER_STYLE_PROPERTY,
	BORDER_TOP_PROPERTY, BORDER_TOP_COLOR_PROPERTY, BORDER_TOP_STYLE_PROPERTY,
	BORDER_TOP_WIDTH_PROPERTY,
	BORDER_WIDTH_PROPERTY,BOTTOM_PROPERTY,

	CAPTION_SIDE_PROPERTY, CLEAR_PROPERTY,
	CLIP_PROPERTY, COLOR_PROPERTY, CONTENT_PROPERTY,
	COUNTER_INCREMENT_PROPERTY, COUNTER_RESET_PROPERTY,
	CUE_PROPERTY, CUE_AFTER_PROPERTY, CUE_BEFORE_PROPERTY, CURSOR_PROPERTY,

	DIRECTION_PROPERTY, DISPLAY_PROPERTY,
	ELEVATION_PROPERTY,
	EMPTY_CELLS_PROPERTY,
	
	FLOAT_PROPERTY,
	FONT_PROPERTY, FONT_FAMILY_PROPERTY, FONT_SIZE_PROPERTY, FONT_SIZE_ADJUST_PROPERTY,
	FONT_STRETCH_PROPERTY, FONT_STYLE_PROPERTY, FONT_VARIANT_PROPERTY,
	FONT_WEIGHT_PROPERTY,

	HEIGHT_PROPERTY,
	LEFT_PROPERTY, LETTER_SPACING_PROPERTY,
	LINE_HEIGHT_PROPERTY,

	LIST_STYLE_PROPERTY, LIST_STYLE_IMAGE_PROPERTY,
	LIST_STYLE_POSITION_PROPERTY, LIST_STYLE_TYPE_PROPERTY,

	MARGIN_PROPERTY, MARGIN_BOTTOM_PROPERTY, MARGIN_LEFT_PROPERTY, MARGIN_RIGHT_PROPERTY,
	MARGIN_TOP_PROPERTY,
	MARKER_OFFSET_PROPERTY, MARKS_PROPERTY, MAX_HEIGHT_PROPERTY, MAX_WIDTH_PROPERTY,
	MIN_HEIGHT_PROPERTY, MIN_WIDTH_PROPERTY,

	ORPHANS_PROPERTY, OUTLINE_PROPERTY, OUTLINE_COLOR_PROPERTY, OUTLINE_STYLE_PROPERTY,
	OUTLINE_WIDTH_PROPERTY,
	OVERFLOW_PROPERTY,

	PADDING_PROPERTY, PADDING_BOTTOM_PROPERTY, PADDING_LEFT_PROPERTY, PADDING_RIGHT_PROPERTY,
	PADDING_TOP_PROPERTY,
	PAGE_PROPERTY,
	PAGE_BREAK_AFTER_PROPERTY, PAGE_BREAK_BEFORE_PROPERTY, PAGE_BREAK_INSIDE_PROPERTY,
	PAUSE_PROPERTY, PAUSE_AFTER_PROPERTY, PAUSE_BEFORE_PROPERTY, PITCH_PROPERTY,
	PITCH_RANGE_PROPERTY, PLAY_DURING_PROPERTY,
	POSITION_PROPERTY,	
	
	QUOTES_PROPERTY,	
	RICHNESS_PROPERTY, RIGHT_PROPERTY,

	SIZE_PROPERTY, 	SPEAK_PROPERTY, SPEAK_HEADER_PROPERTY, SPEAK_NUMERAL_PROPERTY,
	SPEAK_PUNCTUATION_PROPERTY, SPEECH_RATE_PROPERTY, STRESS_PROPERTY,

	TABLE_LAYOUT_PROPERTY,
	TEXT_ALIGN_PROPERTY, TEXT_DECORATION_PROPERTY, TEXT_INDENT_PROPERTY,
	TEXT_SHADOW_PROPERTY, TEXT_TRANSFORM_PROPERTY, TOP_PROPERTY,

	UNICODE_BIDI_PROPERTY,
	VERTICAL_ALIGN_PROPERTY, VISIBILITY_PROPERTY,
	VOICE_FAMILY_PROPERTY, VOLUME_PROPERTY,

	WIDOWS_PROPERTY, WIDTH_PROPERTY,
	WHITE_SPACE_PROPERTY, WORD_SPACING_PROPERTY,
	Z_INDEX_PROPERTY,

	NUM_PROPERTIES
};

enum	// number types
{
	INTEGER_NUMBER	= 1,
	FLOAT_NUMBER	= 2
};

class PropertyParser
{
	public:
		// some standard properties
		static status_t ParseURINoneProperty(StyleSheetParser *parser,uint32 property);

		// color & background properties
		static status_t ParseColorProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseBackgroundColorProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseBackgroundPositionProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseBackgroundProperty(StyleSheetParser *parser,uint32 property);
		
		// box properties
		static status_t ParseBorderColorSidesProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseBorderColorProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseBorderStyleSidesProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseBorderStyleProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseBorderWidthSidesProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseBorderWidthProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseBorderProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseBorderSpacingProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseMarginPaddingOffsetSizeProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseMarginPaddingProperty(StyleSheetParser *parser,uint32 property);

		// misc properties
		static status_t ParseOutlineWidthProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseOutlineColorProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseOutlineProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseListStyleProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseMinMaxSizesProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseMarkerOffsetProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseZIndexWidowsOrphansProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParsePageProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseQuotesProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseMarksProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseCounterResetIncrementProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseContentProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseClipProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseCursorProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseSizeProperty(StyleSheetParser *parser,uint32 property);

		// font properties
		static status_t ParseFontFamilyProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseFontProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseFontWeightProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseFontSizeProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseFontSizeAdjustProperty(StyleSheetParser *parser,uint32 property);

		// text properties
		static status_t ParseWordLetterSpacingProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseTextDecorationProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseVerticalAlignProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseTextTransformProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseTextAlignProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseTextIndentProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseTextShadowProperty(StyleSheetParser *parser,uint32 property);
		static status_t ParseLineHeightProperty(StyleSheetParser *parser,uint32 property);

		// classification properties

		// misc parsing
		static status_t ParseString(StyleSheetParser *parser,char **string);
		static status_t ParseURL(StyleSheetParser *parser,char **url);
		static status_t ParseKeywordOrUnit(StyleSheetParser *parser,int8 *keyword,const char **keywords,int numKeywords,UnitValue *value,uint32 flags);

	private:
		friend class AtRuleParser;

		static status_t ParseUnit(StyleSheetParser *parser,UnitValue *length);
		static status_t ParseLength(StyleSheetParser *parser,UnitValue *length);
		static status_t ParseNumber(StyleSheetParser *parser,float *value,uint8 *type = NULL);
		static status_t ParseKeyword(StyleSheetParser *parser,char **keyword,bool firstChar = true);
		static status_t ParseColor(StyleSheetParser *parser,rgb_color *color);
		static status_t ParseLongName(StyleSheetParser *parser,char **name);
		static status_t ParseShape(StyleSheetParser *parser,Rect<UnitValue> *rect);
		static bool TestForKeyword(StyleSheetParser *parser,char *keyword);
		static int8 FindKeyword(const char *keyword,const char **keywords,int numKeywords);

		template<class T> static void PropagateValuesToRect(T *value,Rect<T> *target,int count);
		template<class T> static void SetRectValue(Rect<T> *rect,T *value,int index);
		static status_t ParseBorderWidth(StyleSheetParser *parser,UnitValue *length);
		static status_t ParseAutoWidth(StyleSheetParser *parser,UnitValue *value,bool padding,bool percent = true);
		static status_t ParseBackgroundPosition(StyleSheetParser *parser);
		static status_t ParseFontFamily(StyleSheetParser *parser);
		static status_t ParseFontWeight(StyleSheetParser *parser);
		static status_t ParseFontSize(StyleSheetParser *parser);
		static status_t ParseCounter(StyleSheetParser *parser,ContentType *ct,bool contents);

		static status_t CheckForEnd(StyleSheetParser *parser);
		static void GetSystemColor(uint32 num,rgb_color *color);
		static status_t GetColorValue(StyleSheetParser *parser,uint8 *color,char delimiter);
};

#endif	/* PROPERTY_PARSER_H */
