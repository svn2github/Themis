#include "ColorRectElement.h"

ColorRectElement::ColorRectElement(BRect frame, rgb_color color) : UIElement(frame)
{
	UIElement::lowcolor = color;

	//General options
	isZoomable = true;
}

ColorRectElement::~ColorRectElement()
{}

void ColorRectElement::EDraw()
{
	parentView->SetHighColor(lowcolor);
	
	parentView->FillRect(frame);
	
	UIElement::EDraw();
}

