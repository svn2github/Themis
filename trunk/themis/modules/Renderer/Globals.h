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

//Some globals BMessages what
#define	G_MOUSE_MOVED				'gmmd'
	#define	TRENDERVIEW_POINTER		"TRenderViewPointer"
	#define G_CURSOR_POSITION		"Position"
	#define	G_CURSOR_TRANSFER		"transfer" /*true when message is sent to 
												 the Element that has the 
												 cursor entering it's frame, 
												 false to the one which 
												 frame is being left by the 
												 cursor*/
#define G_MOUSE_DOWN				'gmdn'
	//G_CURSOR_POSITION can be used here too
	
//TextElement Specific
#define TE_COLOR_PATTERNS 			'crps'
	#define	LOW_RED					"LR"
	#define LOW_GREEN				"LG"
	#define LOW_BLUE				"LB"
	#define HIGH_RED				"HR"
	#define HIGH_GREEN				"HG"
	#define HIGH_BLUE				"HB"

//Some very useful functions
inline void SetColor(rgb_color *to, rgb_color *from)
{
	to->red   = from->red;
	to->green = from->green;
	to->blue  = from->blue;
}

inline void SetColor(rgb_color *to, uchar red, uchar green, uchar blue)
{
	to->red   = red;
	to->green = green;
	to->blue  = blue;
}

#endif
