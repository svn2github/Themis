/*	Themis Renderer

	Olivier MILLA (Methedras) methedras@online.fr
	
	This code falls under the General Themis License.
*/
#include <Bitmap.h>

#include "UIElement.h"

#ifndef _BITMAP_ELEMENT_H_
#define _BITMAP_ELEMENT_H_

//Temporary Bitmap Element: should be changed regarding how the cache
//manages the bitmap, if it keeps it or dierctly gives it to us
//TODO: Should provide a management for animated Gifs in it, without any
//new class inherit
class BitmapElement : public UIElement
{
	public:
						BitmapElement(UIBox frame,  TNodePtr node, BBitmap *bitmap);
		virtual 		~BitmapElement();
		
		virtual	void	EDraw();
		
	protected:
				BBitmap *bitmap;		
				BRect	bmpFrame;
};
#endif

