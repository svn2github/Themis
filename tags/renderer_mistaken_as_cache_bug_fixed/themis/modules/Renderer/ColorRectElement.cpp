/*See header for more details*/

#include "ColorRectElement.h"
#include "TRenderView.h"

ColorRectElement::ColorRectElement(UIBox frame,  TNodePtr node, rgb_color color) : UIElement(frame,node)
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

