#include <stdio.h>

#include "Globals.h"
#include "TextLinkElement.h"
#include "Utils.h"

TextLinkElement::TextLinkElement(TNodePtr node, const char *text, BFont *font, rgb_color high, bool underlinedOnMouseOver = true, float size = 0)
				   : TextElement(node,text,font,high,size)
{
	//Temporary
	SetColor(&over,RGB_BLUE);
	
	TextLinkElement::underlinedOnMouseOver = underlinedOnMouseOver;
}

TextLinkElement::~TextLinkElement()
{}

void TextLinkElement::EMouseMoved(BPoint point, uint32 transit, const BMessage *message)
{
	if (transit == B_INSIDE_VIEW)
		return;
	
	// transit == B_ENTERED_VIEW or transit == B_EXITED_VIEW
	rgb_color temp = highcolor;
	highcolor = over;
	over = temp;			

	//Not working yet cause SetFace() does nothing !!!
	printf("face here == %d\n",font.Face());
	uint16 var = font.Face() ^ B_UNDERSCORE_FACE;
	printf("Let us SetFace(%d)\n",var);
	if (underlinedOnMouseOver)
		font.SetFace(var);		
	printf("face now == %d\n\n",font.Face());	
	
	
	EDraw();
}

