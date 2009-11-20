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
#include <GraphicsDefs.h>
#include <SupportDefs.h>

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

//Some basic colors (from HTML Specs)
#define RGB_AQUA					0,255,255
#define RGB_BLACK 					0,0,0
#define RGB_BLUE					0,0,255
#define RGB_FUCHSIA					255,0,255
#define RGB_GRAY					128,128,128
#define RGB_GREEN					0,128,0
#define RGB_LIME					0,255,0
#define RGB_MAROON					128,0,0
#define RGB_NAVY					0,0,128
#define RGB_OLIVE					128,128,0
#define RGB_PURPLE					128,0,128
#define RGB_RED						255,0,0
#define RGB_SILVER					192,129,192
#define RGB_TEAL					0,128,128
#define RGB_WHITE 					255,255,255
#define RGB_YELLOW					255,255,0

//Others colors
#define RGB_YELLOW_WHITE			255,255,230

//Defaults colors
#define T_DEFAULT_BACKGROUND   		RGB_YELLOW_WHITE
#define T_DEFAULT_HIGH_COLOR	 	RGB_BLACK

//Communication with the UI
#define	R_WELCOME					'rwoe'
#define SITE_TITLE		   			'sete'

//TextElement Specific
#define TE_COLOR_PATTERNS 			'crps'
	#define	LOW_RED					"LR"
	#define LOW_GREEN				"LG"
	#define LOW_BLUE				"LB"
	#define HIGH_RED				"HR"
	#define HIGH_GREEN				"HG"
	#define HIGH_BLUE				"HB"
	
//Alignments
typedef enum t_alignment {
	T_ALIGN_LEFT				=0,
	T_ALIGN_RIGHT				=1,
	T_ALIGN_CENTER 				=2	
};

#define BUTTON_ELEMENT_MESSAGE		'bnem'
#define CHECKBOX_ELEMENT_MESSAGE	'cbem'
#define FILE_ELEMENT_PANEL_MESSAGE	'feem'
#define FILE_ELEMENT_BUTTON_MESSAGE	'fbem'
#define RADIOBUTTON_ELEMENT_MESSAGE	'rbem'
#define TEXTAREA_ELEMENT_MESSAGE	'taem'
#define TEXTCONTROL_ELEMENT_MESSAGE	'tcem'

#endif
