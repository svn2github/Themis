/*
	Copyright (c) 2003 Olivier Milla. All Rights Reserved.
	
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
	
	Original Author: 	Olivier Milla (methedras@online.fr)
	Project Start Date: October 18, 2000
*/
#ifndef _ATTRIBUTES_H_
#define _ATTRIBUTES_H_
/*	This header defines the CSS Property list	(including values and initial values)
	as defined here:
	http://www.w3.org/TR/REC-CSS2/propidx.html
	Problems to fix:
		CSS_ATTRX_V	
		+ some IV
		+ some < > ed values like <absolute-size>, <uri>, etc.
*/

/*********************************************************************		
	Definitions of CSS Properties' values by alphabetical order
**********************************************************************/	
//--Numbers--
#define CSS_100_V					"100"
#define CSS_200_V					"200"
#define CSS_300_V					"300"
#define CSS_400_V					"400"
#define CSS_500_V					"500"
#define CSS_600_V					"600"
#define CSS_700_V					"700"
#define CSS_800_V					"800"
#define CSS_900_V					"900"

//--A--
#define CSS_ABOVE_V					"above"
#define CSS_ABSOLUTE_V				"absolute"
#define CSS_ALWAYS_V				"always"
#define CSS_ARMENIAN_V				"armenian"
#define CSS_ATTRX_V	 // <---------------------- How ? 
#define CSS_AUTO_V					"auto"
#define CSS_AVOID_V					"avoid"

//--B--
#define CSS_BASELINE_V				"baseline"
#define CSS_BEHIND_V				"behind"
#define CSS_BELOW_V					"below"
#define CSS_BIDI_OVERRIDE_V			"bidi-override"
#define CSS_BLINK_V					"blink"
#define CSS_BLOCK_V					"block"
#define CSS_BOLD_V					"bold"
#define CSS_BOLDER_V				"bolder"
#define CSS_BOTH_V					"both"
#define CSS_BOTTOM_V				"bottom"

//--C--
#define CSS_CAPITALIZE_V			"capitalize"
#define CSS_CAPTION_V				"caption"
#define CSS_CENTER_V				"center"
#define CSS_CENTER_LEFT_V			"center-left"
#define CSS_CENTER_RIGHT_V			"center-right"
#define CSS_CIRCLE_V				"circle"
#define CSS_CJK_IDEOGRAPHIC_V		"cjk-ideographic"
#define CSS_CLOSE_QUOTE_V			"close-quote"
#define CSS_CODE_V					"code"
#define CSS_COLLAPSE_V				"collapse"
#define CSS_COMPACT_V				"compact"
#define CSS_CONDENSED_V				"condensed"
#define CSS_CONTINUOUS_V			"continuous"
#define CSS_CROP_V					"crop"
#define CSS_CROSS_V					"cross"
#define CSS_CROSSHAIR_V				"crosshair"

//--D--
#define CSS_DECIMAL_V				"decimal"
#define CSS_DECIMAL_LEADING_ZERO_V	"decimal-leading-zero"
#define CSS_DEFAULT_V				"default"
#define CSS_DIGITS_V				"digits"
#define CSS_DISC_V					"disc"

//--E--
#define CSS_EMBED_V					"embed"
#define CSS_EXPANDED_V				"expanded"
#define CSS_EXTRA_CONDENSED_V		"extra-condensed"
#define CSS_EXTRA_EXPANDED_V		"extra-expanded"
#define CSS_E_RESIZE_V				"e-resize"

//--F--
#define CSS_FAR_LEFT_V				"far-left"
#define CSS_FAR_RIGHT_V				"far-right"
#define CSS_FAST_V					"fast"
#define CSS_FASTER_V				"faster"
#define CSS_FIXED_V					"fixed"

//--G--
#define CSS_GEORGIAN_V				"georgian"

//--H--
#define CSS_HEBREW_V				"hebrew"
#define CSS_HELP_V					"help"
#define CSS_HIDE_V					"hide"
#define CSS_HIDDEN_V				"hidden"
#define CSS_HIGH_V					"high"
#define CSS_HIGHER_V				"higher"
#define CSS_HIRAGANA_V				"hiragana"
#define CSS_HIRAGANA_IROHA_V		"hiragana-iroha"

//--I--
#define CSS_ICON_V					"icon"
#define CSS_INHERIT_V				"inherit"
#define CSS_INLINE_V				"inline"
#define CSS_INLINE_TABLE_V			"inline-table"
#define CSS_INSIDE_V				"inside"
#define CSS_INVERT_V				"invert"
#define CSS_ITALIC_V				"italic"

//--J--
#define CSS_JUSTIFY_V				"justify"

//--K--
#define CSS_KATAKANA_V				"katakana"
#define CSS_KATAKANA_IROHA_V		"katakana-iroha"

//--L--
#define CSS_LANDSCAPE_V				"landscape"
#define CSS_LEFT_V					"left"
#define CSS_LEFTWARDS_V				"leftwards"
#define CSS_LEFT_SIDE_V				"left-side"
#define CSS_LENGTH_V				"length"
#define CSS_LEVEL_V					"level"
#define CSS_LIGHTER_V				"lighter"
#define CSS_LINE_THROUGH_V			"line-through"
#define CSS_LIST_ITEM_V				"list-item"
#define CSS_LOUD_V					"loud"
#define CSS_LOW_V					"low"
#define CSS_LOWER_V					"lower"
#define CSS_LOWERCASE_V				"lowercase"
#define CSS_LOWER_ALPHA_V			"lower-alpha"
#define CSS_LOWER_GREEK_V			"lower-greek"
#define CSS_LOWER_LATIN_V			"lower-latin"
#define CSS_LOER_ROMAN_V			"lower-roman"
#define CSS_LTR_V					"ltr"

//--M--
#define CSS_MARKER_V				"marker"
#define CSS_MEDIUM_V				"medium"
#define CSS_MENU_V					"menu"
#define CSS_MESSAGE_BOX_V			"message-box"
#define CSS_MIDDLE_V				"middle"
#define CSS_MIX_V					"mix"
#define CSS_MOVE_V					"move"

//--N--
#define CSS_NARROWER_V				"narrower"
#define CSS_NE_RESIZE_V				"ne-resize"
#define CSS_NONE_V					"none"
#define CSS_NORMAL_V				"normal"
#define CSS_NOWRAP_V				"nowrap"
#define CSS_NO_CLOSE_QUOTE_V		"no-close-quote"
#define CSS_NO_OPEN_QUOTE_V			"no-open-quote"
#define CSS_NO_REPEAT_V				"no-repeat"
#define CSS_NW_RESIZE_V				"nw-resize"
#define CSS_N_RESIZE_V				"n-resize"

//--O--
#define CSS_OBLIQUE_V				"oblique"
#define CSS_ONCE_V					"once"
#define CSS_OPEN_QUOTE_V			"open-quote"
#define CSS_OUTSIDE_V				"outside"
#define CSS_OVERLINE_V				"overline"

//--P--
#define CSS_PERCENTAGE_V			"percentage"
#define CSS_POINTER_V				"pointer"
#define CSS_PORTRAIT_V				"portrait"
#define CSS_PRE_V					"pre"

//--R--
#define CSS_RELATIVE_V				"relative"
#define CSS_REPEAT_V				"repeat"
#define CSS_REPEAT_X_V				"repeat-x"
#define CSS_REPEAT_Y_V				"repeat-y"
#define CSS_RIGHT_V					"right"
#define CSS_RIGHTWARDS_V			"rightwards"
#define CSS_RIGHT_SIDE_V			"right-side"
#define CSS_RTL_V					"rtl"
#define CSS_RUN_IN_V				"run-in"

//--S--
#define CSS_SCROLL_V				"scroll"
#define CSS_SEMI_CONDENSED_V		"semi-condensed"
#define CSS_SEPARATE_V				"separate"
#define CSS_SE_RESIZE_V				"se-resize"
#define CSS_SHOW_V					"show"
#define CSS_SILENT_V				"silent"
#define CSS_SLOW_V					"slow"
#define CSS_SLOWER_V				"slower"
#define CSS_SMALL_CAPS_V			"small-caps"
#define CSS_SMALL_CAPTION_V			"small-caption"
#define CSS_SOFT_V					"soft"
#define CSS_SPELL_OUT_V				"spell-out"
#define CSS_SQUARE_V				"square"
#define CSS_STATIC_V				"static"
#define CSS_STATUS_BAR_V			"status-bar"
#define CSS_SUB_V					"sub"
#define CSS_SUPER_V					"super"
#define CSS_SW_RESIZE_V				"sw-resize"
#define CSS_S_RESIZE_V				"s-resize"

//--T--
#define CSS_TABLE_V					"table"
#define CSS_TABLE_CAPTION_V			"table-caption"
#define CSS_TABLE_CELL_V			"table-cell"
#define CSS_TABLE_COLUMN_V			"table-column" 
#define CSS_TABLE_COLUMN_GROUP_V	"table-column-group"
#define CSS_TABLE_FOOTER_GROUP_V	"table-footer-group"
#define CSS_TABLE_HEADER_GROUP_V	"table-header-group"
#define CSS_TABLE_ROW_V				"table-row"
#define CSS_TABLE_ROW_GROUP_V		"table-row-group"
#define CSS_TEXT_V					"text"
#define CSS_TEXT_BOTTOM_V			"text-bottom"
#define CSS_TEXT_TOP_V				"text-top"
#define CSS_TOP_V					"top"
#define CSS_TRANSPARENT_V			"transparent"

//--U--
#define CSS_ULTA_CONDENSED_V		"ultra-condensed"
#define CSS_ULTA_EXPANDED_V			"ultra-expanded"
#define CSS_UNDERLINE_V				"underline"
#define CSS_UPPERCASE_V				"uppercase"
#define CSS_UPPER_ALPHA_V			"upper-alpha"
#define CSS_UPPER_LATIN_V			"upper-latin"
#define CSS_UPPER_ROMAN_V			"upper-roman"

//--V--
#define CSS_VISIBLE_V				"visible"

//--W--
#define CSS_WAIT_V					"wait"
#define CSS_WIDER_V					"wider"
#define CSS_W_RESIZE_V				"w-resize"

//--X--
#define CSS_X_FAST_V				"x-fast"
#define CSS_X_HIGH_V				"x-high"
#define CSS_X_LOUD_V				"x-loud"
#define CSS_X_LOW_V					"x-low"
#define CSS_X_SLOW_V				"x-slow"
#define CSS_X_SOFT_V				"x-soft"



/**********************************************************************************
	Definitions of CSS Properties by alphabetical order
***********************************************************************************/	
//--A--
#define CSS_AZIMUTH_P				"azimuth"

//--B--
#define CSS_BACKGROUND_P			"background"
#define CSS_BACKGROUND_ATTACHMENT_P	"background-attachment"
#define CSS_BACKGROUND_COLOR_P		"background-color"
#define CSS_BACKGROUND_IMAGE_P		"background-image"
			
#define CSS_BACKGROUND_POSITION_P	"background-position"
#define CSS_BACKGROUND_REPEAT_P		"background-repeat"
#define CSS_BORDER_P				"border"
#define CSS_BORDER_COLLAPSE_P		"boder-collapse"
#define CSS_BORDER_COLOR_P			"border-color"
#define CSS_BORDER_SPACING_P		"border-spacing"
#define CSS_BORDER_STYLE_P			"border-style"
#define CSS_BORDER_BOTTOM_P			"border-bottom"
#define CSS_BORDER_LEFT_P			"border-left"
#define CSS_BORDER_RIGHT_P			"border-right"
#define CSS_BORDER_TOP_P			"border-top"
#define CSS_BORDER_BOTTOM_COLOR_P	"border-bottom-color"
#define CSS_BORDER_LEFT_COLOR_P		"border-left-color"
#define CSS_BORDER_RIGHT_COLOR_P	"border-right-color"
#define CSS_BORDER_TOP_COLOR_P		"border-top-color"
#define CSS_BORDER_BOTTOM_STYLE_P	"border-bottom-style"
#define CSS_BORDER_LEFT_STYLE_P		"border-left-style"
#define CSS_BORDER_RIGHT_STYLE_P	"border-right-style"
#define CSS_BORDER_TOP_STYLE_P		"border-top-style"
#define CSS_BORDER_BOTTOM_WIDTH_P	"border-bottom-width"
#define CSS_BORDER_LEFT_WIDTH_P		"border-left-width"
#define CSS_BORDER_RIGHT_WIDTH_P	"border-right-width"
#define CSS_BORDER_TOP_WIDTH_P		"border-top-width"
#define CSS_BORDER_WIDTH_P			"border-width"
#define CSS_BOTTOM_P				"bottom"

//--C--
#define CSS_CAPTION_SIDE_P			"caption-side"
#define CSS_CLEAR_P					"clear"
#define CSS_CLIP_P					"clip"
#define CSS_COLOR_P					"color"
#define CSS_CONTENT_P				"content"
#define CSS_COUNTER_INCREMENT_P		"counter-increment"
#define CSS_COUNTER_RESET_P			"counter-reset"
#define CSS_CUE_P					"cue"
#define CSS_CUE_AFTER_P				"cue-after"
#define CSS_CUE_BEFORE_P			"cue-before"
#define CSS_CURSOR_P				"cursor"

//--D--
#define CSS_DIRECTION_P				"direction"
#define CSS_DISPLAY_P				"display"

//--E--
#define CSS_ELEVATION_P				"elevation"
#define CSS_EMPTY_CELLS_P			"empty-cells"

//--F--
#define CSS_FLOAT_P					"float"
#define CSS_FONT_P					"font"
#define CSS_FONT_FAMILY_P			"font-family"
#define CSS_FONT_SIZE_P				"font-size"
#define CSS_FONT_SIZE_ADJUST_P		"font-size-adjust"
#define CSS_FONT_STRETCH_P			"font-stretch"	
#define CSS_FONT_STYLE_P			"font-style"
#define CSS_FONT_VARIANT_P			"font-variant"
#define CSS_FONT_WEIGHT_P			"font-weight"

//-H--
#define CSS_HEIGHT_P				"height"

//--L--
#define CSS_LEFT_P					"left"
#define CSS_LETTER_SPACING_P		"letter-spacing"
#define CSS_LINE_HEIGHT_P			"line-height"
#define CSS_LINE_STYLE_P			"line-style"
#define CSS_LIST_STYLE_IMAGE_P		"list-style-image"
#define CSS_LIST_STYLE_POSITION_P	"list-style-position"
#define CSS_LIST_STYLE_TYPE_P		"list-style-type"

//--M--
#define CSS_MARGIN_P				"margin"
#define CSS_MARGIN_BOTTOM_P			"margin-bottom"
#define CSS_MARGIN_LEFT_P			"margin-left"
#define CSS_MARGIN_RIGHT_P			"margin-right"
#define CSS_MARGIN_TOP_P			"margin-top"
#define CSS_MARKER_OFFSET_P			"marker-offset"
#define CSS_MARKS_P					"marks"
#define CSS_MAX_HEIGHT_P			"max-height"
#define CSS_MAX_WIDTH_P				"max-width"
#define CSS_MIN_HEIGHT_P			"min-height"
#define CSS_MIN_WIDTH_P				"min-width"

//--O--
#define CSS_ORPHANS_P				"orphans"
#define CSS_OUTLINE_P				"outline"
#define CSS_OUTLINE_COLOR_P			"outline-color"
#define CSS_OUTLINE_STYLE_P			"outline-style"
#define CSS_OUTLINE_WIDTH_P			"outline-width"
#define CSS_OVERFLOW_P				"overflow"

//--P--
#define CSS_PADDING_P				"padding"
#define CSS_PADDING_BOTTOM_P		"padding-bottom"
#define CSS_PADDING_LEFT_P			"padding-left"
#define CSS_PADDING_RIGHT_P			"padding-right"
#define CSS_PADDING_TOP_P			"padding-top"
#define CSS_PAGE_P					"page"
#define CSS_PAGE_BREAK_AFTER_P		"page-break-after"
#define CSS_PAGE_BREAK_BEFORE_P		"page-break-before"
#define CSS_PAGE_BREAK_INSIDE_P		"page-break-inside"
#define CSS_PAUSE_P					"pause"
#define CSS_PAUSE_AFTER_P			"pause-after"
#define CSS_PAUSE_BEFORE_P			"pause-before"
#define CSS_PITCH_P					"pitch"
#define CSS_PITCH_RANGE_P			"pitch-range"
#define CSS_PLAY_DURING_P			"play-during"
#define CSS_POSITION_P 				"position"

//--Q--
#define CSS_QUOTES_P				"quotes"

//--R--
#define CSS_RICHNESS_P				"richness"
#define CSS_RIGHT_P					"right"

//--S--
#define CSS_SIZE_P					"size"
#define CSS_SPEAK_P					"speak"
#define CSS_SPEAK_HEADER_P			"speak-header"
#define CSS_SPEAK_NUMERAL_P			"speak-numeral"
#define CSS_SPEAK_PUNCTUATION_P		"speak-punctuation"
#define CSS_SPEECH_RATE_P			"speech-rate"
#define CSS_STRESS_P				"stress"

//--T--
#define CSS_TABLE_LAYOUT_P			"table-layout"
#define CSS_TEXT_ALIGN_P			"text-align"
#define CSS_TEXT_DECORATION_P		"text-decoration"
#define CSS_TEXT_INDENT_P			"text-indent"
#define CSS_TEXT_SHADOW_P			"text-shadow"
#define CSS_TEXT_TRANSFORM_P		"text-transform"
#define CSS_TOP_P					"top"

//--U--
#define CSS_UNICODE_BIDI_P			"unicode-bidi"

//--V--
#define CSS_VERTICAL_ALIGN_P		"vertical-align"
#define CSS_VISIBILITY_P			"visibility"
#define CSS_VOICE_FAMILY_P			"voice-family"
#define CSS_VOLUME_P				"volume"

//--W--
#define CSS_WHITE_SPACE_P			"white-space"
#define CSS_WIDOWS_P				"widows"
#define CSS_WIDTH_P					"width"
#define CSS_WORD_SPACING_P			"word-spacing"

//--Z--
#define CSS_Z_INDEX_P				"z-index"	



/*********************************************************************
				CSS Property default values
**********************************************************************/
//--A--
#define CSS_AZIMUTH_P_IV					CSS_CENTER_V

//--B--
#define CSS_BACKGROUND_P_IV					//XX <----- How ?
#define CSS_BACKGROUND_ATTACHMENT_P_IV		CSS_SCROLL_V
#define CSS_BACKGROUND_COLOR_P_IV			CSS_TRANSPARENT_V
#define CSS_BACKGROUND_IMAGE_P_IV			CSS_NONE_V
			
#define CSS_BACKGROUND_POSITION_P_IV		//0% 0%
#define CSS_BACKGROUND_REPEAT_P_IV			CSS_REPEAT_V
#define CSS_BORDER_P_IV						// ??
#define CSS_BORDER_COLLAPSE_P_IV			CSS_COLLAPSE_V
#define CSS_BORDER_COLOR_P_IV				// ??
#define CSS_BORDER_SPACING_P_IV				0
#define CSS_BORDER_STYLE_P_IV				// ??
#define CSS_BORDER_BOTTOM_P_IV				// ??
#define CSS_BORDER_LEFT_P_IV				// ??
#define CSS_BORDER_RIGHT_P_IV				// ??
#define CSS_BORDER_TOP_P_IV				 	// ??
#define CSS_BORDER_BOTTOM_COLOR_P_IV		// ??
#define CSS_BORDER_LEFT_COLOR_P_IV			// ??
#define CSS_BORDER_RIGHT_COLOR_P_IV			// ??
#define CSS_BORDER_TOP_COLOR_P_IV			// ??
#define CSS_BORDER_BOTTOM_STYLE_P_IV		CSS_NONE_V
#define CSS_BORDER_LEFT_STYLE_P_IV			CSS_NONE_V
#define CSS_BORDER_RIGHT_STYLE_P_IV			CSS_NONE_V
#define CSS_BORDER_TOP_STYLE_P_IV			CSS_NONE_V
#define CSS_BORDER_BOTTOM_WIDTH_P_IV		CSS_MEDIUM_V
#define CSS_BORDER_LEFT_WIDTH_P_IV			CSS_MEDIUM_V
#define CSS_BORDER_RIGHT_WIDTH_P_IV			CSS_MEDIUM_V
#define CSS_BORDER_TOP_WIDTH_P_IV			CSS_MEDIUM_V
#define CSS_BORDER_WIDTH_P_IV				// ??
#define CSS_BOTTOM_P_IV						CSS_AUTO_V

//--C--
#define CSS_CAPTION_SIDE_P_IV				CSS_TOP_V		
#define CSS_CLEAR_P_IV						CSS_NONE_V
#define CSS_CLIP_P_IV						CSS_AUTO_V
#define CSS_COLOR_P_IV						//depends on user agent
#define CSS_CONTENT_P_IV					NULL //Empty string
#define CSS_COUNTER_INCREMENT_P_IV			CSS_NONE_V
#define CSS_COUNTER_RESET_P_IV				CSS_NONE_V
#define CSS_CUE_P_IV						//XX
#define CSS_CUE_AFTER_P_IV					CSS_NONE_V
#define CSS_CUE_BEFORE_P_IV					CSS_NONE_V
#define CSS_CURSOR_P_IV						CSS_AUTO_V

//--D--
#define CSS_DIRECTION_P_IV					CSS_LTR_V
#define CSS_DISPLAY_P_IV					CSS_INLINE_V

//--E--
#define CSS_ELEVATION_P_IV					CSS_LEVEL_V
#define CSS_EMPTY_CELLS_P_IV				CSS_SHOW_V

//--F--
#define CSS_FLOAT_P_IV						CSS_NONE_V
#define CSS_FONT_P_IV						// ??
#define CSS_FONT_FAMILY_P_IV				//depends on user agent
#define CSS_FONT_SIZE_P_IV					CSS_MEDIUM_V
#define CSS_FONT_SIZE_ADJUST_P_IV			CSS_NONE_V
#define CSS_FONT_STRETCH_P_IV				CSS_NORMAL_V	
#define CSS_FONT_STYLE_P_IV					CSS_NORMAL_V
#define CSS_FONT_VARIANT_P_IV				CSS_NORMAL_V
#define CSS_FONT_WEIGHT_P_IV				CSS_NORMAL_V

//-H--
#define CSS_HEIGHT_P_IV						CSS_AUTO_V

//--L--
#define CSS_LEFT_P_IV						CSS_AUTO_V
#define CSS_LETTER_SPACING_P_IV				CSS_NORMAL_V
#define CSS_LINE_HEIGHT_P_IV				CSS_NORMAL_V
#define CSS_LINE_STYLE_P_IV					// XX
#define CSS_LIST_STYLE_IMAGE_P_IV			CSS_NONE_V
#define CSS_LIST_STYLE_POSITION_P_IV		CSS_OUTSIDE_V
#define CSS_LIST_STYLE_TYPE_P_IV			CSS_DISC_V

//--M--
#define CSS_MARGIN_P_IV						// XX
#define CSS_MARGIN_BOTTOM_P_IV				0
#define CSS_MARGIN_LEFT_P_IV				0
#define CSS_MARGIN_RIGHT_P_IV				0
#define CSS_MARGIN_TOP_P_IV					0
#define CSS_MARKER_OFFSET_P_IV				CSS_AUTO_V
#define CSS_MARKS_P_IV						CSS_NONE_V
#define CSS_MAX_HEIGHT_P_IV					CSS_NONE_V
#define CSS_MAX_WIDTH_P_IV					CSS_NONE_V
#define CSS_MIN_HEIGHT_P_IV					0
#define CSS_MIN_WIDTH_P_IV					//UA dependant

//--O--
#define CSS_ORPHANS_P_IV					2
#define CSS_OUTLINE_P_IV					// ??
#define CSS_OUTLINE_COLOR_P_IV				CSS_INVERT_V
#define CSS_OUTLINE_STYLE_P_IV				CSS_NONE_V
#define CSS_OUTLINE_WIDTH_P_IV				CSS_MEDIUM_V
#define CSS_OVERFLOW_P_IV					CSS_VISIBLE_V

//--P--
#define CSS_PADDING_P_IV					// XX
#define CSS_PADDING_BOTTOM_P_IV				0
#define CSS_PADDING_LEFT_P_IV				0
#define CSS_PADDING_RIGHT_P_IV				0
#define CSS_PADDING_TOP_P_IV				0
#define CSS_PAGE_P_IV						CSS_AUTO_V
#define CSS_PAGE_BREAK_AFTER_P_IV			CSS_AUTO_V
#define CSS_PAGE_BREAK_BEFORE_P_IV			CSS_AUTO_V
#define CSS_PAGE_BREAK_INSIDE_P_IV			CSS_AUTO_V
#define CSS_PAUSE_P_IV						//depends on User Agent
#define CSS_PAUSE_AFTER_P_IV				//depends on User Agent
#define CSS_PAUSE_BEFORE_P_IV				//depends on User Agent
#define CSS_PITCH_P_IV						CSS_MEDIUM_V
#define CSS_PITCH_RANGE_P_IV				50
#define CSS_PLAY_DURING_P_IV				CSS_AUTO_V
#define CSS_POSITION_P_IV 					CSS_STATIC_V

//--Q--
#define CSS_QUOTES_P_IV						//depends on User Agent

//--R--
#define CSS_RICHNESS_P_IV					50
#define CSS_RIGHT_P_IV						CSS_AUTO_V

//--S--
#define CSS_SIZE_P_IV						CSS_AUTO_V
#define CSS_SPEAK_P_IV						CSS_NORMAL_V
#define CSS_SPEAK_HEADER_P_IV				CSS_ONCE_V
#define CSS_SPEAK_NUMERAL_P_IV				CSS_CONTINUOUS_V
#define CSS_SPEAK_PUNCTUATION_P_IV			CSS_NONE_V
#define CSS_SPEECH_RATE_P_IV				CSS_MEDIUM_V
#define CSS_STESS_P_IV						50

//--T--
#define CSS_TABLE_LAYOUT_P_IV				CSS_AUTO_V
#define CSS_TEXT_ALIGN_P_IV					//depends on user Agent and writting direction
#define CSS_TEXT_DECORATION_P_IV			CSS_NONE_V
#define CSS_TEXT_INDENT_P_IV				0
#define CSS_TEXT_SHADOW_P_IV				CSS_NONE_V
#define CSS_TEXT_TRANSFORM_P_IV				CSS_NONE_V
#define CSS_TOP_P_IV						CSS_AUTO_V

//--U--
#define CSS_UNICODE_BIDI_P_IV				CSS_NORMAL_V

//--V--
#define CSS_VERTICAL_ALIGN_P_IV				CSS_BASELINE_V
#define CSS_VISIBILITY_P_IV					CSS_INHERIT_V
#define CSS_VOICE_FAMILY_P_IV				//depends on User Agent
#define CSS_VOLUME_P_IV						CSS_MEDIUM_V

//--W--
#define CSS_WHITE_SPACE_P_IV				CSS_NORMAL_V			
#define CSS_WIDOWS_P_IV						2
#define CSS_WIDTH_P_IV						CSS_AUTO_V
#define CSS_WORD_SPACING_P_IV				CSS_NORMAL_V

//--Z--
#define CSS_Z_INDEX_P_IV					CSS_AUTO_V

#endif


