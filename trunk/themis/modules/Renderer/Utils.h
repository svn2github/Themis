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

inline rgb_color SetColorSelf(uchar red, uchar green, uchar blue)
{
	rgb_color color = {red,green,blue};
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





