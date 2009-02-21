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
#include <Point.h>
#include <Rect.h>
#include <Screen.h>

#ifndef _UTILS_H_
#define _UTILS_H_

//Some very useful functions
inline void SetColor(rgb_color *to, uchar red, uchar green, uchar blue)
{
	to->red   = red;
	to->green = green;
	to->blue  = blue;
}

inline rgb_color SetColorSelf(uchar red, uchar green, uchar blue)
{
	rgb_color color = {red,green,blue};
	return color;
}

inline char HexaToInt(char c)
{
	if (c > 64 && c < 71)
		return c - 55;
	else if (c > 96 && c < 103)
		return c - 87;
		
	return c - '0';
}

rgb_color 	MakeRgbFromHexa(const char *string);

inline bool SameColor(rgb_color one, rgb_color two)
{
	return (one.red == two.red && 
			one.green == two.green && 
			one.blue == two.blue);
	
}

inline bool SameColor(rgb_color one, uchar red, uchar green, uchar blue)
{
	return (one.red == red && 
			one.green == green && 
			one.blue == blue);
}

/*frame is the BRect you wish to center into the into rect according to align[W|H].*/
void CenterRectInRect(BRect *frame, BRect into, bool alignW, bool alignH); 

/*returns the center BPoint of the frame*/												
BPoint GetRectCenter(BRect frame);		

//DefaultIfNull that doesn't apply any function
inline const char *DefaultIfNull(const char *string, const char *ifNull)
{
	return (string) ? string : ifNull;
}

//DefaultIfNull that applies a function
template <class T, class P> T DefaultIfNull(P (*func)(const char *string), const char *string, T ifNull)
{		
	return (string) ? (T)(*func)(string) : ifNull;
}

#endif





