#include "TextControlElement.h"

TextControlElement::TextControlElement(BRect frame, const char *name, const char *text, 
									   BMessage *message, uint32 resizingMode,
									   uint32 flags) 
				   : BTextControl(frame,name,NULL,text,message,resizingMode,flags) 
				   , UIElement(frame)
{
	//General options
	isZoomable = false;
}

TextControlElement::~TextControlElement()
{}

void TextControlElement::AttachedToWindow()
{
	SetDivider(0);
	
	//Set Message Target
	SetTarget(parentView);
	
	BTextControl::AttachedToWindow();	
}