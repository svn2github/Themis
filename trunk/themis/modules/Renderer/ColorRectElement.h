/*	Themis Renderer

	Olivier MILLA (Methedras) methedras@online.fr
	
	This code falls under the General Themis License.
*/
#include "UIElement.h"

#ifndef _COLOR_RECT_ELEMENT_H_
#define _COLOR_RECT_ELEMENT_H_

class ColorRectElement : public UIElement
{
	public:
							ColorRectElement(BRect frame, rgb_color color);
		virtual				~ColorRectElement();
		virtual	void		EDraw();
};

#endif

