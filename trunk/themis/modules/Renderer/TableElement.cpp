/* See header for more info */

#include "TableElement.h"

TableElement::TableElement(BRect frame, int cellpadding, int cellspacing, 
						   rgb_color bgcolor, rgb_color bordercolor) 
			 : UIElement(frame)
{
	isZoomable = true;
	
	TableElement::cellpadding = cellpadding;
	TableElement::cellspacing = cellspacing;
	TableElement::bordercolor = bordercolor;

	UIElement::lowcolor		  = bgcolor;
	
	rows = columns = 1;
}

TableElement::~TableElement()
{}

void TableElement::EDraw()
{
	int32 i=0;
	
	parentView->BeginLineArray(2+rows+columns);
	
	//Draw borders (basic by now: no options, etc. just basic lines)
	parentView->AddLine(frame.LeftTop(),frame.RightTop(),bordercolor);
	parentView->AddLine(frame.RightTop(),frame.RightBottom(),bordercolor);
	parentView->AddLine(frame.RightBottom(),frame.LeftBottom(),bordercolor);
	parentView->AddLine(frame.LeftBottom(),frame.LeftTop(),bordercolor);	
	
	//Draw lines (basic)
	for (i=1; i < rows; i++)
		parentView->AddLine(BPoint(frame.LeftTop().x,
								   frame.LeftTop().y+i*frame.Height()/rows),
							BPoint(frame.RightTop().x,
								   frame.RightTop().y+i*frame.Height()/rows),
							bordercolor);
	
	//Draw Columns (basic)
	for (i=1; i < columns; i++)
		parentView->AddLine(BPoint(frame.LeftTop().x+i*frame.Width()/columns,
								   frame.LeftTop().y),
							BPoint(frame.LeftBottom().x+i*frame.Width()/columns,
								   frame.RightBottom().y),
							bordercolor);
								   	
	parentView->EndLineArray();
		
	UIElement::EDraw();
}

