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

#include <List.h>
#include <Message.h>
#include <Window.h>

#include "Elements.h"
#include "Globals.h"
#include "TRenderView.h"
#include "Utils.h"

/*
//An example of text
char *TEXT1 = "There was a boy!!!!!!!!! A scary boy downtown!! he was playing the Grim Reaper !!!!!!!!";
char *TEXT2 = "I never thought that would be sooooooooooo scary!!!!!! Billy & Mandy must think the same!!!!!!!!!!!!!!";
char *TEXT3 = "uh! uh! uh! I know i'll call it the BAD GUY from DOWNTOWN!!!";
char *TEXT4 = "Great Billy! Great! Try to find a new name for your stupidity too!!!!";
char *TEXT5 = "Mandy !!!!!!!! u're too bad with that Billy!!! He will probably never like u anymore!!!!!!!!!!!!!!!!";
char *TEXT6 = "uh! Oh! Don't worry Grim!!!!!!!! Mandy's my best friend!!!!!!! And u too bonheah !!!!!!!!!!!!!!!!!!  And Trykie too!!!!!!!!!";
*/

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
	
//	printf("TRENDERVIEW: Original frame = ");
//	frame.PrintToStream();
//	printf("\n");
	
	//====================== ADD WHAT YOU WANT FOR TESTING=========================
//	rgb_color color = MakeRgbFromHexa("#EB2323");
//	ColorRectElement *rect = new ColorRectElement(BRect(25,25,220,90),color);
//	EAddChild(rect);
//	TextLinkElement *el = new TextLinkElement(BRect(30,30,190,60),TEXT,(BFont *)be_plain_font,HighColor());
//	rect->EAddChild(el);
/*	BFont		*font1 = new BFont(be_plain_font);
	rgb_color	*high1 = new rgb_color;
	rgb_color	*low1  = new rgb_color;	
	BFont		*font2 = new BFont(be_bold_font);
	rgb_color	*high2 = new rgb_color;
	rgb_color	*low2  = new rgb_color;		
	BFont		*font3 = new BFont(be_plain_font);
	rgb_color	*high3 = new rgb_color;	
	rgb_color	*low3  = new rgb_color;		
	BFont		*font4 = new BFont(be_plain_font);
	rgb_color	*high4 = new rgb_color;	
	rgb_color	*low4  = new rgb_color;		
	BFont		*font5 = new BFont(be_plain_font);
	rgb_color	*high5 = new rgb_color;	
	rgb_color	*low5  = new rgb_color;					
	BFont		*font6 = new BFont(be_plain_font);
	rgb_color	*high6 = new rgb_color;		
	rgb_color	*low6  = new rgb_color;		
	
	*high1 = *high3 = *high6 = HighColor();	
	*low4 = *low5 = *low6 = LowColor();		
	SetColor(high2,RGB_BLUE);
	SetColor(high4,RGB_GREEN);
	SetColor(high5,RGB_RED);	
	SetColor(low1,RGB_BLUE);
	SetColor(low3,0,1,0);
	SetColor(low2,RGB_RED);	
	font1->SetSize(18.0);
	font2->SetSize(14.0);
	font3->SetSize(26.0);
	font4->SetSize(15.0);
	font5->SetSize(12.0);		
	font6->SetSize(22.0);
	
	TNodePtr node;
	TextElement *text = new TextElement(node,TEXT1,font1,high1,low1,T_ALIGN_LEFT);
	text->AppendText(TEXT2,font2,high2,low2);
	text->AppendText(TEXT3,font3,high3,low3);
	text->AppendText(TEXT4,font4,high4,low4);
	text->AppendText(TEXT5,font5,high5,low5);
	text->AppendText(TEXT6,font6,high6,low6);
					
	EAddChild(text);	*/
/*	TNodePtr node;
	FileElement *element = new FileElement(frame,node,"name","Muahah!!");
	EAddChild(element);*/
	//=============================================================================
}

TRenderView::~TRenderView()
{}

void TRenderView::AttachedToWindow()
{		
	//Fake a FrameResized to do positioning, frame calculus, etc. on all elements
	FrameResized(frame.ContentRect().Width(),frame.ContentRect().Height());
	
	//Fake a MouseMoved received for elements that need specials behaviours 
	//when mouse is over them
	BPoint point;
	uint32 val;
	GetMouse(&point,&val);
	MouseMoved(point,val,NULL); //We give val but we could give anything.
								//It Doesn't matter.	
	BView::AttachedToWindow();
}

void TRenderView::SetLowColor(rgb_color color)
{
	UIElement::lowColor = color;
	
	BView::SetLowColor(color);
}

void TRenderView::Draw(BRect updateRect)
{
	printf("RENDERER DEBUG: Calling TRenderView::EDraw()\n");
	bigtime_t time = real_time_clock_usecs();	
	
  	Window()->DisableUpdates();
  	Window()->BeginViewTransaction();
 
	//Let's update the layers that need to
	if (nextLayer)
		for (int32 i=0; i<nextLayer->CountItems(); i++)
			if (((UIElement *)nextLayer->ItemAt(i))->frame.Intersects(updateRect))
				((UIElement *)nextLayer->ItemAt(i))->EDraw();
	
	/*Many drawing above (mostly when drawing BBitmaps) are done asynchronously for speed
	 (DrawBitmapAsync() for instance) so we got to Sync() for safety. */
	//Sync();
 	Window()->EndViewTransaction(); 
	Window()->EnableUpdates();		
	printf("RENDERER INFO: DONE redrawing in %g microseconds.\n",real_time_clock_usecs() - time);	
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
	printf("RENDERER DEBUG: Calling TRenderView::EFrameResized()\n");
	bigtime_t time = real_time_clock_usecs();	
	EFrameResized(width,height);
	printf("RENDERER INFO: DONE resizing in %g microseconds.\n",real_time_clock_usecs() - time);
	
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

	//We manage the cursor change depending on the position here.	
	if (onTop == currentMouseOver){
		onTop->EMouseMoved(point,B_INSIDE_VIEW,message);
	}
	else {
		onTop->EMouseMoved(point,B_ENTERED_VIEW,message);
		currentMouseOver->EMouseMoved(point,B_EXITED_VIEW,message);
		if (onTop->cursor != currentMouseOver->cursor)
			SetViewCursor(onTop->cursor,true);
		currentMouseOver = onTop;
	}
}

void TRenderView::MessageReceived(BMessage *message)
{
	//Handle it's own message. Forward message to elements
	switch(message->what){
		case BUTTON_ELEMENT_MESSAGE:{
			ButtonElement *element = NULL;
			message->FindPointer("element",(void **)&element);
			if (element)
				element->EMessageReceived(message);
			}break;
		case CHECKBOX_ELEMENT_MESSAGE:{
			CheckBoxElement *element = NULL;
			message->FindPointer("element",(void **)&element);
			if (element)
				element->EMessageReceived(message);
			}break;			
		case FILE_ELEMENT_PANEL_MESSAGE:
		case FILE_ELEMENT_BUTTON_MESSAGE:{
			FileElement *element = NULL;
			message->FindPointer("element",(void **)&element);
			if (element)
				element->EMessageReceived(message);
			}break;
		case RADIOBUTTON_ELEMENT_MESSAGE:{
			RadioButtonElement *element = NULL;
			message->FindPointer("element",(void **)&element);
			if (element)
				element->EMessageReceived(message);
			}break;
		case TEXTCONTROL_ELEMENT_MESSAGE:{
			TextControlElement *element = NULL;
			message->FindPointer("element",(void **)&element);
			if (element)
				element->EMessageReceived(message);			
			}break;
		default:
			BView::MessageReceived(message);
	}
}

