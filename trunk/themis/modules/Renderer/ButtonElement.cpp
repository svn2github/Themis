/* See header for more info */

#include "ButtonElement.h"

ButtonElement::ButtonElement(BRect frame, const char *name, const char *label, 
							 BMessage *message, uint32 resizingMode, uint32 flags) 
			  : BButton(frame,name,label,message,resizingMode,flags) , UIElement(frame)
{
	//General options
	isZoomable = false;
}

ButtonElement::~ButtonElement()
{}

void ButtonElement::AttachedToWindow()
{
	//Set Message Target
	SetTarget(parentView);
	
	BButton::AttachedToWindow();
}

