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
