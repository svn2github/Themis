/*	Themis Renderer

	Olivier MILLA (Methedras) methedras@online.fr
	
	This code falls under the General Themis License.
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
	borderHeight = height;
	borderWidth   = width;
}

inline void UIBox::SetPadding(float left, float top, float right, float bottom)
{
	paddingLeft   = left;
	paddingRight  = right;
	paddingTop	 = top;
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
