/*
	Copyright (c) 2003 Olivier Milla. All Rights Reserved.
	
	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or
	sell copies of the Software, and to permit persons to whom
	the Software is furnished to do so, subject to the following
	conditions:
	
	   The above copyright notice and this permission notice
	   shall be included in all copies or substantial portions
	   of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
	KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
	WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
	OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
	OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
	OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
	
	Original Author: 	Olivier Milla (methedras@online.fr)
	Project Start Date: October 18, 2000
*/
#include "TableElement.h"
#include "TRenderView.h"
#include "Utils.h"

#include <String.h> //temporary
#include <stdio.h>  //temporary

TableElement::TableElement(UIBox frame,  TNodePtr node, int cellpadding, int cellspacing, 
						   rgb_color bgcolor, rgb_color bordercolor) 
			 : UIElement(frame,node)
{
	TableElement::cellpadding = cellpadding;
	TableElement::cellspacing = cellspacing;
	TableElement::bordercolor = bordercolor;

	UIElement::lowColor		  = bgcolor;
	
	rows = columns = 0;
}

TableElement::~TableElement()
{}

void TableElement::EDraw()
{
	int32 i=0;

	BString s = "TABLE: ";
	s << columns  << " columns, " << rows << "rows";
	printf("%s\n",s.String());
	printf("border_color = {%d,%d,%d}\n",bordercolor.red,bordercolor.green,bordercolor.blue);

	//FillRect with the Background color and SetLowColor
	parentView->SetLowColor(lowColor);
	if (!SameColor(lowColor,T_DEFAULT_BACKGROUND))	
		parentView->FillRect(frame);
		
	parentView->BeginLineArray(4+rows+columns);
	
	//Draw borders (basic by now: no options, etc. just basic lines)
	parentView->AddLine(frame.LeftTop(),frame.RightTop(),bordercolor);
	parentView->AddLine(frame.RightTop(),frame.RightBottom(),bordercolor);
	parentView->AddLine(frame.RightBottom(),frame.LeftBottom(),bordercolor);
	parentView->AddLine(frame.LeftBottom(),frame.LeftTop(),bordercolor);	
	
	//Draw rows (basic)
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

void TableElement::EFrameResized(float deltaWidth, float deltaHeight)
{
	printf("TABLEELEMENT: Calling FrameResized(%f,%f)\n",deltaWidth,deltaHeight);
	printf("TABLEELEMENT: Frame changed from: ");
	frame.PrintToStream();		
	
	//TODO: Should be enhanced later
	//ProportionalResizingAndMoving(deltaWidth,deltaHeight);
	
	printf(" to: ");
	frame.PrintToStream();
	
	UIElement::EFrameResized(deltaWidth,deltaHeight);
}

void TableElement::AddColumn()
{
	columns++;
}

void TableElement::AddRow()
{
	rows++;
}
