/* See header for more info */

#include "BitmapLinkElement.h"

BitmapLinkElement::BitmapLinkElement(BRect frame, BBitmap *bitmap, BBitmap *onMouseOver) 
				  :BitmapElement(frame,bitmap)
{
	BitmapLinkElement::onMouseOver = onMouseOver;
}

BitmapLinkElement::~BitmapLinkElement()
{
	//delete onMouseOver ?
}

void BitmapLinkElement::EMouseMoved(BPoint point, uint32 transit, const BMessage *message)
{
	if (transit == B_ENTERED_VIEW || transit == B_EXITED_VIEW){
		BBitmap *transit = bitmap;
		bitmap = onMouseOver;
		onMouseOver = transit;		
		EDraw();
	}	
}