#include <stdio.h>

#include <Message.h>
#include <SupportDefs.h>
#include <View.h>

#include "Globals.h"
#include "TextElement.h"

TextElement::TextElement(BRect frame, const char *text, BFont *font, rgb_color high,
						 rgb_color low, float size) : UIElement(frame)
{
	//General options
	isZoomable 			  = false;
	needUpdateOnMouseOver = false;
	
	//Member datas
	TextElement::text << text;
	
	TextElement::font = *font;
	if (size > 0)
		TextElement::font.SetSize(size);
	
	TextElement::low  = low;
	TextElement::high = high;
}

TextElement::~TextElement()
{}