/*	Themis Renderer

	Olivier MILLA (Methedras) methedras@online.fr
	
	This code falls under the General Themis License.
*/
#include <GraphicsDefs.h>
#include <SupportDefs.h>

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

//Some basic colors
#define RGB_WHITE 					255,255,255
#define RGB_BLACK 					0,0,0
#define RGB_RED						255,0,0
#define RGB_GREEN					0,255,0
#define RGB_BLUE					0,0,255
#define RGB_YELLOW_WHITE			255,255,238

//Defaults colors
#define T_DEFAULT_BACKGROUND   		RGB_YELLOW_WHITE
#define T_DEFAULT_HIGH_COLOR	 	RGB_BLACK

//Communication with the UI
#define	R_WELCOME					'rwoe'
#define RENDERVIEW_POINTER 			'rwpr'
#define SITE_TITLE		   			'sete'

//TextElement Specific
#define TE_COLOR_PATTERNS 			'crps'
	#define	LOW_RED					"LR"
	#define LOW_GREEN				"LG"
	#define LOW_BLUE				"LB"
	#define HIGH_RED				"HR"
	#define HIGH_GREEN				"HG"
	#define HIGH_BLUE				"HB"

#endif
