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

#include "Globals.h"
#include "Utils.h"

rgb_color MakeRgbFromHexa(const char *string)
{
	rgb_color color;

	if (string[0] == '#'){
		color.red   = 16*HexaToInt(string[1]) + HexaToInt(string[2]);
		color.green = 16*HexaToInt(string[3]) + HexaToInt(string[4]);
		color.blue  = 16*HexaToInt(string[5]) + HexaToInt(string[6]);
	}
	else { //It is a text like "Black" or "Purple", etc. See Specs
		switch(string[0]){
			case 'S': SetColor(&color,RGB_SILVER);  break; //Silver
			case 'W': SetColor(&color,RGB_WHITE);   break; //White
			case 'M': SetColor(&color,RGB_MAROON);  break; //Maroon
			case 'R': SetColor(&color,RGB_RED);     break; //Red
			case 'P': SetColor(&color,RGB_PURPLE);  break; //Purple
			case 'F': SetColor(&color,RGB_FUCHSIA); break; //Fuchsia
			case 'L': SetColor(&color,RGB_LIME);    break; //Lime
			case 'O': SetColor(&color,RGB_OLIVE);   break; //Oliver
			case 'Y': SetColor(&color,RGB_YELLOW);  break; //Yellow
			case 'N': SetColor(&color,RGB_NAVY);    break; //Navy
			case 'T': SetColor(&color,RGB_TEAL);    break; //Teal
			case 'A': SetColor(&color,RGB_AQUA);    break; //Aqua
			case 'B': (string[2] == 'a') ? SetColor(&color,RGB_BLACK) 
										 : SetColor(&color,RGB_BLUE); break;
			case 'G': (string[2] == 'a') ? SetColor(&color,RGB_GRAY)
									     : SetColor(&color,RGB_GREEN); break;
			default: break;
		}
	}	
	return color;	
}	






