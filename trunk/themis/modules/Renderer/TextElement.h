#include <Font.h>
#include <String.h>

#include "UIElement.h"
#include "Globals.h"

#ifndef _TEXT_ELEMENT_H_
#define _TEXT_ELEMENT_H_

class TextElement : public UIElement
{
	public:
						TextElement(BRect frame, const char *text, BFont *font,
									rgb_color high, rgb_color low, float size = 0);
									//Maybe TRenderView *parent should be passed too
									
		virtual 		~TextElement();
				
	protected:
				BString 	text;
				BFont   	font;
				rgb_color	low;
				rgb_color	high;
};

#endif
