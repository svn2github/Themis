/*	Themis Renderer

	Olivier MILLA (Methedras) methedras@online.fr
	
	This code falls under the General Themis License.
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

inline rgb_color RgbFor(uchar red, uchar green, uchar blue)
{
	rgb_color color = { red, green, blue };
	return color;
}

inline char HexaToInt(char c);
rgb_color 	MakeRgbFromHexa(const char *string);

inline bool SameColor(rgb_color one, rgb_color two)
{
	return (one.red == two.red && 
			one.green == two.green && 
			one.blue == two.blue);
	
}
/*frame is the BRect you wish to center into the into rect according to align[W|H].*/
void CenterRectInRect(BRect *frame, BRect into, bool alignW, bool alignH); 

/*returns the center BPoint of the frame*/												
BPoint GetRectCenter(BRect frame);		

#endif





