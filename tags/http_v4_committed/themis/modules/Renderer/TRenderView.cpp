/* See header for more info */

#include <stdio.h>

#include <List.h>
#include <Message.h>

#include "Elements.h"
#include "Globals.h"
#include "TRenderView.h"
#include "Utils.h"

//An example of text
char *TEXT = "azkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijez azkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijezazkoep dlksqp dsqdjlksqjdidqhjfezio ijfoezvklc vkozkkl ijez";

TRenderView::TRenderView(UIBox frame, TNodePtr node) : BView(frame.MarginRect(),"TRenderView P0waa !!",B_FOLLOW_ALL_SIDES,B_WILL_DRAW | B_FRAME_EVENTS) , UIElement(UIBox(frame.Width(),frame.Height()),node)
{	
	//The default High and Low Color for Themis
	SetViewColor(T_DEFAULT_BACKGROUND);
	SetLowColor(SetColorSelf(T_DEFAULT_BACKGROUND));
	SetHighColor(SetColorSelf(T_DEFAULT_HIGH_COLOR));
	
	currentMouseOver = this;
	parentView 		 = this;
	siteID			 = 0;
	urlID			 = 0;
	
	UIElement::frame = frame;
	
	printf("TRENDERVIEW: Original frame = ");
	frame.PrintToStream();
	printf("\n");
	
	//====================== ADD WHAT YOU WANT FOR TESTING=========================
//	rgb_color color = MakeRgbFromHexa("#EB2323");
//	ColorRectElement *rect = new ColorRectElement(BRect(25,25,220,90),color);
//	EAddChild(rect);
//	TextLinkElement *el = new TextLinkElement(BRect(30,30,190,60),TEXT,(BFont *)be_plain_font,HighColor());
//	rect->EAddChild(el);
	TNodePtr node;
	TextLinkElement *text = new TextLinkElement(node,TEXT,(BFont *)be_plain_font,HighColor());
	EAddChild(text);
	//=============================================================================
	FrameResized(frame.ContentRect().Width(),frame.ContentRect().Height());
}

TRenderView::~TRenderView()
{}

void TRenderView::SetLowColor(rgb_color color)
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
}

void TRenderView::FrameResized(float width, float height)
{
	/*BRect oldRect = UIElement::frame;
	
	printf("TRENDERVIEW: Calling FrameResized(%f,%f)\n",width,height);
	printf("TRENDERVIEW: Frame changed from: ");
	frame.PrintToStream();		
	UIElement::frame = Bounds();
	printf(" to: ");
	frame.PrintToStream();
	printf("\n");
	
	//Calling EFrameResized with RELATIVES VALUES : THE RELATIVE VARIATION ! ! ! !
	EFrameResized(frame.Width()/oldRect.Width(),frame.Height()/oldRect.Height());
	*/
	UIElement::frame = Bounds();
	EFrameResized(width,height);
	
	//Complete redraw after frame resized
	Invalidate();
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
	
	printf ("--------\n");
	printf("%d elements in list\n",nextLayer->CountItems());
	printf("address: of the element: %d, onTop: %d\n",nextLayer->FirstItem(),onTop);
	printf("address of the view: %d\n",this);
	
	printf("onTop = %d, currentMouseOver = %d\n",onTop,currentMouseOver);
	
	if (onTop == currentMouseOver){
		onTop->EMouseMoved(point,B_INSIDE_VIEW,message);
		printf("Not sending cause no need\n");
	}
	else {
		onTop->EMouseMoved(point,B_ENTERED_VIEW,message);
		currentMouseOver->EMouseMoved(point,B_EXITED_VIEW,message);
		if (onTop->cursor != currentMouseOver->cursor)
			SetViewCursor(onTop->cursor,true);
		currentMouseOver = onTop;
		printf("sending to %d an event message\n");
	}
}

void TRenderView::MessageReceived(BMessage *message)
{
	switch(message->what){
		default:
			BView::MessageReceived(message);
	}
}

