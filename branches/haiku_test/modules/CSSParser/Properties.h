#ifndef PROPERTIES_H
#define PROPERTIES_H
/* Properties.h - CSS properties related stuff
**
** Copyright 2001, pinc Software. All Rights Reserved.
*/

#include "Types.h"

#ifdef __BEOS__
#	include <GraphicsDefs.h>
#	include <List.h>
#	ifndef List
#		define List BList	// to be replaced by portable list
#	endif
#else
	typedef struct rgb_color
	{
		uint8 red;
		uint8 green;
		uint8 blue;
		uint8 alpha;
	} rgb_color;
#	error "no BeOS, no list"
#endif
#include "Hashtable.h"

/***************************** UnitValue *****************************/

enum	// flags for UnitValue
{
	UNIT_NONE		= 0x0000,
	UNIT_METRIC		= 0x0001,
	UNIT_PT			= 0x0002,
	UNIT_EM			= 0x0004,
	UNIT_EX			= 0x0008,
	UNIT_PIXEL		= 0x0010,
	UNIT_PERCENT	= 0x0020,

	UNIT_MILLI		= 0x0040,
	UNIT_RADIAN		= 0x0080,
	UNIT_HERTZ		= 0x0100,

	// special flags for different properties
	UNIT_AUTO		= 0x0400,
	UNIT_SMALLER	= 0x0800,
	UNIT_LIGHTER	= 0x1000,
	UNIT_LARGER		= 0x2000,
	UNIT_BOLDER		= 0x4000,
	UNIT_NORMAL		= 0x8000,
	UNIT_PORTRAIT	= 0x0800,
	UNIT_LANDSCAPE	= 0x1000,

	UNIT_NOT_SPECIFIED = 0x8000000
};

#define UNIT_RELATIVE (UNIT_EM | UNIT_EX | UNIT_PIXEL | UNIT_PERCENT)
#define UNIT_LENGTH (UNIT_METRIC | UNIT_PT | UNIT_EM | UNIT_EX | UNIT_PIXEL)
#define UNIT_TIME (UNIT_MILLI)
#define UNIT_ANGLE (UNIT_RADIAN)
#define UNIT_FREQUENCY (UNIT_HERTZ)

class UnitValue
{
	public:
		UnitValue();

		float value;
		uint32 flags;
};


/***************************** misc stuff *****************************/

template<class T> class Rect
{
	public:
		T left,right,top,bottom;
};


/***************************** FontFamily *****************************/

class FontFamily
{
	public:
		~FontFamily();

		char *name;
		int8 generic;
};


/***************************** TextShadow *****************************/

class TextShadow
{
	public:
		rgb_color color;
		UnitValue horizontal,vertical,radius;
		bool colorSet;
};


/***************************** Counter *****************************/

class Counter
{
	public:
		Counter(char *n) { name = n; value = 0;};
		
		int32 value;
		char *name;
};

/***************************** Flags *****************************/

class Flags
{
	public:
		Flags();

		void Reset();
		void Set(uint32 flag,bool set = true);
		bool IsSet(uint32 flag);
		
		Flags &operator|=(const Flags &);
		bool operator!=(int);

	private:
		uint32 flags[3];
};

/***************************** ContentType *****************************/

class ContentType
{
	public:
		ContentType() { string = delimiter = NULL; };
		char *string;
		char *delimiter;
		int8 style;
		int8 type;
};

enum
{
	CONTENT_OPEN_QUOTE_TYPE = 0,
	CONTENT_CLOSE_QUOTE_TYPE,
	CONTENT_NO_OPEN_QUOTE_TYPE,
	CONTENT_NO_CLOSE_QUOTE_TYPE,

	CONTENT_URI_TYPE,
	CONTENT_STRING_TYPE,
	CONTENT_COUNTERS_TYPE,
	CONTENT_COUNTER_TYPE,
	CONTENT_ATTR_TYPE
};

/***************************** Properties *****************************/

class Properties
{
	public:
		Properties();
		~Properties();
		void Clear();

		Flags specified,important,inherit;

		// box properties
		Rect<UnitValue> margin,padding,border;
		Rect<rgb_color> borderColor;
		Rect<int8> borderStyle;
		UnitValue verticalBorderSpacing,horizontalBorderSpacing;
		UnitValue width,height;

		// background properties
		rgb_color backgroundColor;
		char *backgroundImage;
		UnitValue backgroundLeft,backgroundTop;

		// text properties
		UnitValue wordSpacing,letterSpacing;
		uint8 textDecoration;
		uint8 verticalAlignType;
		UnitValue verticalAlign;
		char *textAlignString;
		uint8 textAlign;
		List textShadow;
		UnitValue textIndent;

		// font properties
		List fontFamilies;	// list of FontFamily
		int8 systemFont;
		int8 fontWeight;
		UnitValue fontSize;
	
		UnitValue lineHeight,markerOffset,horizontalSize,verticalSize;
		UnitValue maxWidth,maxHeight,minWidth,minHeight;
		UnitValue outlineWidth;

		Rect<UnitValue> boxOffset;
		Rect<UnitValue> clip;

		Hashtable counter;
		List content;
		List quotes;
		List cursorURLs;

		uint8 cursor;
		uint8 marks;

		rgb_color color;
		char *pageIdentifier;

		rgb_color outlineColor;
		char *listStyleImage;
		int32 zIndex,widows,orphans;

		float fontSizeAdjust;
		bool fontSizeAdjusted;

		// these are the "keyword properties", ordered
		int8	backgroundAttach,backgroundRepeat,borderCollapse,
				captionSide,clear,direction,display,emptyCells,
				floating,fontStretch,fontStyle,fontVariant,
				listStylePosition,listStyleType,outlineStyle,overflow,
				pageBreak,pageBreakInside,position,
				speak,speakHeader,speakNumeral,speakPunctuation,
				tableLayout,textTransform,
				unicodeBidi,visibility,whiteSpace;

		uint32 ownerCount;		
};

enum display_types
{
	DISPLAY_NONE = 0,
	DISPLAY_INLINE,
	DISPLAY_BLOCK,
	DISPLAY_LIST_ITEM,
	DISPLAY_RUN_IN,
	DISPLAY_COMPACT,
	DISPLAY_MARKER,
	DISPLAY_TABLE,
	DISPLAY_INLINE_TABLE,
	DISPLAY_TABLE_ROW_GROUP,
	DISPLAY_TABLE_HEADER_GROUP,
	DISPLAY_TABLE_FOOTER_GROUP,
	DISPLAY_TABLE_ROW,
	DISPLAY_TABLE_COLUMN_GROUP,
	DISPLAY_TABLE_COLUMN,
	DISPLAY_TABLE_CELL,
	DISPLAY_TABLE_CAPTION
};

enum cursor_types
{
	CURSOR_AUTO = 0,
	CURSOR_CROSSHAIR,
	CURSOR_DEFAULT,
	CURSOR_POINTER,
	CURSOR_MOVE,
	CURSOR_E_RESIZE,
	CURSOR_NERESIZE,
	CURSOR_NW_RESIZE,
	CURSOR_N_RESIZE,
	CURSOR_SE_RESIZE,
	CURSOR_SW_RESIZE,
	CURSOR_S_RESIZE,
	CURSOR_W_RESIZE,
	CURSOR_TEXT,
	CURSOR_WAIT,
	CURSOR_HELP
};

enum mark_types
{
	CROSS_MARKS = 1,
	CROP_MARKS
};

enum white_space
{
	WHITE_SPACE_NORMAL	= 0,
	WHITE_SPACE_PRE,
	WHITE_SPACE_NOWRAP
};

enum list_style_types
{
	LIST_STYLE_NONE = 0,
	LIST_STYLE_DISC,
	LIST_STYLE_CIRCLE,
	LIST_STYLE_SQUARE,
	LIST_STYLE_DECIMAL,
	LIST_STYLE_DECIMAL_LEADING_ZERO,
	LIST_STYLE_LOWER_ROMAN,
	LIST_STYLE_UPPER_ROMAN,
	LIST_STYLE_LOWER_GREEK,
	LIST_STYLE_LOWER_ALPHA,
	LIST_STYLE_LOWER_LATIN,
	LIST_STYLE_UPPER_ALPHA,
	LIST_STYLE_UPPER_LATIN,
	LIST_STYLE_HEBREW,
	LIST_STYLE_ARMENIAN,
	LIST_STYLE_GEORGIAN,
	LIST_STYLE_CJK_IDEOGRAPHIC,
	LIST_STYLE_HIRAGANA,
	LIST_STYLE_KATAKANA,
	LIST_STYLE_HIRAGANA_IROHA,
	LIST_STYLE_KATAKANA_IROHA
};


/***************************** background properties *****************************/

#define BACKGROUND_REPEAT_X 1
#define BACKGROUND_REPEAT_Y 2

#define BACKGROUND_SCROLL 0
#define BACKGROUND_FIXED 1


/***************************** box properties *****************************/

enum border_styles
{
	BORDER_STYLE_NONE = 0,
	BORDER_STYLE_HIDDEN,
	BORDER_STYLE_DOTTED,
	BORDER_STYLE_DASHED,
	BORDER_STYLE_SOLID,
	BORDER_STYLE_DOUBLE,
	BORDER_STYLE_GROOVE,
	BORDER_STYLE_RIDGE,
	BORDER_STYLE_INSET,
	BORDER_STYLE_OUTSET,
};

enum floating_types
{
	FLOAT_NONE = 0,
	FLOAT_LEFT,
	FLOAT_RIGHT
};

enum clear_type
{
	CLEAR_NONE = 0,
	CLEAR_LEFT,
	CLEAR_RIGHT,
	CLEAR_BOTH
};


/***************************** font properties *****************************/

enum font_style
{
	FONT_STYLE_NORMAL = 0,
	FONT_STYLE_ITALIC,
	FONT_STYLE_OBLIQUE
};

enum font_variant
{
	FONT_VARIANT_NORMAL	= 0,
	FONT_VARIANT_SMALL_CAPS
};

enum font_weight
{
	FONT_WEIGHT_NORMAL = 0,
	FONT_WEIGHT_BOLD,
	FONT_WEIGHT_BOLDER,
	FONT_WEIGHT_LIGHTER,
	FONT_WEIGHT_100,
	FONT_WEIGHT_200,
	FONT_WEIGHT_300,
	FONT_WEIGHT_400,
	FONT_WEIGHT_500,
	FONT_WEIGHT_600,
	FONT_WEIGHT_700,
	FONT_WEIGHT_800,
	FONT_WEIGHT_900
};

enum system_fonts
{
	CAPTION_FONT = 0,
	ICON_FONT,
	MENU_FONT,
	MESSAGE_BOX_FONT,
	SMALL_CAPTION_FONT,
	STATUS_BAR_FONT
};


/***************************** text properties *****************************/

enum text_deco
{
	TEXT_DECO_NONE		= 0,
	TEXT_DECO_UNDERLINE	= 1,
	TEXT_DECO_OVERLINE	= 2,
	TEXT_DECO_LINE_THROUGH = 4,
	TEXT_DECO_BLINK = 8
};

enum text_verticalAlign
{
	TEXT_ALIGN_BASELINE = 0,
	TEXT_ALIGN_SUB,
	TEXT_ALIGN_MIDDLE,
	TEXT_ALIGN_SUPER,
	TEXT_ALIGN_TEXT_TOP,
	TEXT_ALIGN_TEXT_BOTTOM,
	TEXT_ALIGN_TOP,
	TEXT_ALIGN_BOTTOM,
	TEXT_ALIGN_SPECIAL = 255
};

enum text_transform
{
	TEXT_TRANS_NONE = 0,
	TEXT_TRANS_CAPITALIZE,
	TEXT_TRANS_LOWERCASE,
	TEXT_TRANS_UPPERCASE
};

enum text_align
{
	TEXT_ALIGN_LEFT = 0,
	TEXT_ALIGN_RIGHT,
	TEXT_ALIGN_CENTER,
	TEXT_ALIGN_JUSTIFY
};


#endif	/* PROPERTIES_H */
