#include "TextElement.h"

#ifndef _TEXT_LINK_ELEMENT_H_
#define _TEXT_LINK_ELEMENT_H_

class TextLinkElement : public TextElement
{
	public:
							TextLinkElement(TNodePtr node, const char *text, BFont *font,
											rgb_color high, bool underlinedOnMouseOver = true, float size = 0);		
		virtual				~TextLinkElement();
		
		virtual	void		EMouseMoved(BPoint point, uint32 transit, const BMessage *message);

	protected:
				rgb_color	over;
				bool		underlinedOnMouseOver;
		
		//TODO: Add active, etc. and constructor args
};

#endif


