#include <stdio.h>

#include <List.h>
#include <Message.h>

#include "Globals.h"
#include "TRenderView.h"

TRenderView::TRenderView(BRect frame) : BView(frame,"God",B_FOLLOW_ALL_SIDES,B_WILL_DRAW) , UIElement(frame)
{	
	SetViewColor(255,255,255);
	SetLowColor(255,255,255);
	SetHighColor(0,0,0);
	
	currentMouseOver = this;
	
	UIElement::frame = Bounds();
	
	//====================== ADD WHAT YOU WANT FOR TESTING=========================
//	ColorRect *rect = new ColorRect(BRect(30,30,190,60),17,204,179);
//	AddChildElement(rect);
//	rect->AddChildElement(new TextElement(BRect(40,40,120,50),"This is a TextElement",(BFont *)be_plain_font));
	//=============================================================================
}

TRenderView::~TRenderView()
{}

void TRenderView::Draw(BRect updateRect)
{
	//Let's update the layers that need to
	if (nextLayer)
		for (int32 i=0; i<nextLayer->CountItems(); i++)
			if (((UIElement *)nextLayer->ItemAt(i))->frame.Intersects(updateRect))
				((UIElement *)nextLayer->ItemAt(i))->DrawElement(this);

	/*Many drawing there (mostly when drawing BBitmaps) are done asynchronously for speed
	 (DrawBitmapAsync() for instance) so we got to Sync(). */
	Sync();
	
	BView::Draw(updateRect);
}

void TRenderView::FrameResized(float width, float height)
{
	UIElement::frame = Bounds();
}

void TRenderView::MouseDown(BPoint point)
{
	//Forward message to element
	BMessage message(G_MOUSE_DOWN);
	message.AddPoint(G_CURSOR_POSITION,point);
	FindElementFor(point)->PostTMessage(&message);	
	BView::MouseDown(point);
}

void TRenderView::MouseMoved(BPoint point, uint32 transit, const BMessage *message)
{
	UIElement *onTop = FindElementFor(point);

	/*We need to maintain the currentMouseOver var and to tell UIElements when Mouse
	  enters and leave their frame. We Update the frontmost element if it needs to. (if it's bool  
	  needUpdateOnMouseOver is true) */
	if (currentMouseOver != onTop){
		if (currentMouseOver->needUpdateOnMouseOver){
			BMessage msg(G_MOUSE_MOVED);
			msg.AddPointer(TRENDERVIEW_POINTER,this);
			msg.AddPoint(G_CURSOR_POSITION,point);
			msg.AddBool(G_CURSOR_TRANSFER,false);			
			currentMouseOver->PostTMessage(&msg);
		}	
		if (onTop->needUpdateOnMouseOver){
			BMessage msg(G_MOUSE_MOVED);
			msg.AddPointer(TRENDERVIEW_POINTER,this);
			msg.AddPoint(G_CURSOR_POSITION,point);
			msg.AddBool(G_CURSOR_TRANSFER,true);		
			onTop->PostTMessage(&msg);	
		}			
		currentMouseOver = onTop;
	}
	
	BView::MouseMoved(point,transit,message);
}
