/* See header for more info */

#include "BitmapElement.h"

BitmapElement::BitmapElement(UIBox frame,  TNodePtr node, BBitmap *bitmap) : UIElement(frame,node)
{
	isZoomable = false;
	BitmapElement::bitmap = bitmap;
	
	bmpFrame = BRect(0,0,frame.Width(),frame.Height());
}

BitmapElement::~BitmapElement()
{
	//delete bitmap ?
}

void BitmapElement::EDraw()
{
	if (bitmap)
		parentView->DrawBitmapAsync(bitmap,bmpFrame,frame);
}