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
#include <Rect.h>

#ifndef _UIBOX_H_
#define _UIBOX_H_

//An attempt to describe: http://www.w3.org/TR/REC-CSS2/box.html
class UIBox : public BRect
{
	public:
						UIBox(float width = 0, float height = 0);
		virtual 		~UIBox();
		
		//To set different frame height and width
				void	SetMargin(float left, float top, float right, float bottom);
				void 	SetBorder(float width, float height);
				void	SetPadding(float left, float top, float right, float bottom);	
				void	SetContent(float width, float height); 
		
		//To retrieve needed frames
				BRect 	ContentRect();
				BRect 	PaddingRect();
				BRect 	BorderRect();
				BRect 	MarginRect();

				BPoint	GetCenter();
		
				UIBox	&operator = (BRect rect);
				
				void	PrintUIBoxToStream();
				
	private:
				void	CenterInBox(UIBox box, bool alignW, bool alignH);
						
				float	marginLeft;
				float	marginTop;
				float 	marginRight;
				float	marginBottom;
			
				float 	borderHeight;
				float  	borderWidth;
		
				float	paddingLeft;
				float	paddingTop;
				float	paddingRight;
				float	paddingBottom;
};

//inline methods
inline void UIBox::SetMargin(float left, float top, float right, float bottom)
{
	marginLeft   = left;
	marginRight  = right;
	marginTop	 = top;
	marginBottom = bottom;
}

inline void UIBox::SetBorder(float width, float height)
{
	borderHeight  = height;
	borderWidth   = width;
}

inline void UIBox::SetPadding(float left, float top, float right, float bottom)
{
	paddingLeft   = left;
	paddingRight  = right;
	paddingTop	  = top;
	paddingBottom = bottom;
}

inline void UIBox::SetContent(float width, float height)		
{
	BPoint leftTop = LeftTop();
	
	SetRightBottom(BPoint(leftTop.x + width, leftTop.y + height));
}

inline BRect UIBox::ContentRect()
{
	return BRect(left   + marginLeft   + borderWidth  + paddingLeft,
				 top    + marginTop    + borderHeight + paddingTop,
				 right  - marginRight  - borderWidth  - paddingRight,
				 bottom - marginBottom - borderHeight - paddingBottom);
}

inline BRect UIBox::PaddingRect()
{
	return BRect(left   + marginLeft   + borderWidth,
				 top    + marginTop    + borderHeight,
				 right  - marginRight  - borderWidth,
				 bottom - marginBottom - borderHeight);
}

inline BRect UIBox::BorderRect()
{
	return BRect(left   + marginLeft,
				 top    + marginTop,
				 right  - marginRight,
				 bottom - marginBottom);
}

inline BRect UIBox::MarginRect()
{
	return *this;				
}

inline BPoint UIBox::GetCenter()
{
	BPoint center;
	
	center.x = left + Width()/2;
	center.y = top  + Height()/2;
	
	return center;
}

inline void UIBox::CenterInBox(UIBox box, bool alignW, bool alignH)
{	
	BPoint center = box.GetCenter();
	
	float width  = box.Width();
	float height = box.Height();
	
	if (alignW){
		left  	= center.x - width/2;
		right 	= center.x + width/2; 
	}
	if (alignH){
		top  	= center.y - height/2;
		bottom	= center.y + height/2;
	}
}

inline UIBox &UIBox::operator = (BRect rect)
{
	*(dynamic_cast <BRect *> (this)) = rect;
	return *this;
}

#endif
