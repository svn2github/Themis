/* See header for more info */

#include <GraphicsDefs.h>

#include "Utils.h"

rgb_color MakeRgbFromHexa(const char *string)
{
	rgb_color color;

	color.red   = 16*HexaToInt(string[1]) + HexaToInt(string[2]);
	color.green = 16*HexaToInt(string[3]) + HexaToInt(string[4]);
	color.blue  = 16*HexaToInt(string[5]) + HexaToInt(string[6]);
		
	return color;	
}	

char HexaToInt(char c)
{
	if (c > 64 && c < 71)
		return c - 55;
	else if (c > 96 && c < 103)
		return c - 87;
		
	return c - '0';
}

BPoint GetRectCenter(BRect frame)
{
	BPoint center;
	
	center.x = frame.Width()/2;
	center.y = frame.Height()/2;
	
	return center;
}

void CenterRectInRect(BRect *frame, BRect into, bool alignW, bool alignH)
{	
	BPoint center = GetRectCenter(into);
	
	float width  = frame->Width();
	float height = frame->Height();
	
	if (alignW){
		frame->left  	= center.x - width/2;
		frame->right 	= center.x + width/2; 
	}
	if (alignH){
		frame->top  	= center.y - height/2;
		frame->bottom	= center.y + height/2;
	}
}


