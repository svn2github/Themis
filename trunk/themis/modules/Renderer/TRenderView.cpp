/* See header for more info */

#include <stdio.h>

#include <List.h>
#include <Message.h>

#include "Elements.h"
#include "Globals.h"
#include "TRenderView.h"
#include "Utils.h"

#define TEXT "Heya girls ! As you can see if you type enough text"

TRenderView::TRenderView(BRect frame) : BView(frame,"God",B_FOLLOW_ALL_SIDES,B_WILL_DRAW | B_FRAME_EVENTS) , UIElement(frame)
{	
	SetViewColor(255,255,255);
	SetLowColor(RgbFor(255,255,255));
	SetHighColor(0,0,0);
	
	currentMouseOver = this;
	parentView 		 = this;
	
	UIElement::frame = Bounds();
	
	//====================== ADD WHAT YOU WANT FOR TESTING=========================
//	rgb_color color = MakeRgbFromHexa("#EB2323");
//	ColorRectElement *rect = new ColorRectElement(BRect(25,25,220,90),color);
//	EAddChild(rect);
//	TextLinkElement *el = new TextLinkElement(BRect(30,30,190,60),TEXT,(BFont *)be_plain_font,HighColor());
//	rect->EAddChild(el);
	//=============================================================================
}

TRenderView::~TRenderView()
{}

void TRenderView::SetLowColor( rgb_color color)
{
	UIElement::lowcolor = color;
	
	BView::SetLowColor(color);
}

void TRenderView::Draw(BRect updateRect)
{
	//Let's update the layers that need to
	if (nextLayer)
		for (int32 i=0; i<nextLayer->CountItems(); i++)
			if (((UIElement *)nextLayer->ItemAt(i))->frame.Intersects(updateRect))
				((UIElement *)nextLayer->ItemAt(i))->EDraw();

	/*Many drawing above (mostly when drawing BBitmaps) are done asynchronously for speed
	 (DrawBitmapAsync() for instance) so we got to Sync() for safety. */
	Sync();
	
	BView::Draw(updateRect);
}

void TRenderView::FrameResized(float width, float height)
{
	UIElement::frame = Bounds();
	
	EFrameResized(width,height);
}

void TRenderView::MouseDown(BPoint point)
{
	FindElementFor(point)->EMouseDown(point);
}

void TRenderView::MouseUp(BPoint point)
{
	FindElementFor(point)->EMouseUp(point);
}

void TRenderView::MouseMoved(BPoint point, uint32 transit, const BMessage *message)
{
	UIElement *onTop = FindElementFor(point);
	
	if (onTop == currentMouseOver)
		onTop->EMouseMoved(point,B_INSIDE_VIEW,message);
	else {
		onTop->EMouseMoved(point,B_ENTERED_VIEW,message);
		currentMouseOver->EMouseMoved(point,B_EXITED_VIEW,message);
		currentMouseOver = onTop;
	}
}

void TRenderView::MessageReceived(BMessage *message)
{
	switch(message->what){
		case R_WELCOME:{
			BRect rect;
			message->FindRect("rect",&rect);
			FrameResized(rect.Width(),rect.Height());
			}break;
		default:
			BView::MessageReceived(message);
	}
}

