/*	Themis Renderer

	Olivier MILLA (Methedras) methedras@online.fr
	
	This code falls under the General Themis License.
*/
#include "BitmapElement.h"

#ifndef _BITMAP_LINK_ELEMENT_H_
#define _BITMAP_LINK_ELEMENT_H_

class BitmapLinkElement : public BitmapElement
{
	public:
						BitmapLinkElement(BRect frame, BBitmap *bitmap, BBitmap *onMouseOver);
		
		virtual			~BitmapLinkElement();
		
		virtual	void	EMouseMoved(BPoint point, uint32 transit, const BMessage *message);
	
	protected:
				BBitmap *onMouseOver;
};

#endif
