#include "UIBox.h"

UIBox::UIBox(float width, float height) : BRect(0,0,width,height)
{
	marginLeft = marginTop = marginRight = marginBottom = 0;
	
	borderHeight = borderWidth = 0;
	
	paddingLeft = paddingRight = paddingTop = paddingBottom = 0; 
}

UIBox::~UIBox()
{}
		

