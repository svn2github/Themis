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
#include <stdio.h>

#include "UIBox.h"

UIBox::UIBox(float width, float height) : BRect(0,0,width,height)
{
	marginLeft = marginTop = marginRight = marginBottom = 0;
	
	borderHeight = borderWidth = 0;
	
	paddingLeft = paddingRight = paddingTop = paddingBottom = 0; 
}

UIBox::~UIBox()
{}

void UIBox::PrintUIBoxToStream()
{
	printf("The UIBox lies in the frame ");
	PrintToStream();
	printf("with the following datas:\n");
	printf("	*"); 
	printf("Margins(l:%f ,t:%f ,r:%f ,b%f) => BorderRect() = ",marginLeft,marginTop,marginRight,marginBottom);
	BorderRect().PrintToStream();
	printf("	*"); 
	printf("Borders(t:%f ,b%f) => PaddingRect() = ",borderWidth,borderHeight);
	PaddingRect().PrintToStream();
	printf("	*"); 
	printf("Paddings(l:%f ,t:%f ,r:%f ,b%f) => ContentRect() = ",paddingLeft,paddingTop,paddingRight,paddingBottom);
	ContentRect().PrintToStream();

}
		

