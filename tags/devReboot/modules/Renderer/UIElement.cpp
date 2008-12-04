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

#include <AppDefs.h>
#include <Message.h>

#include "TRenderView.h"
#include "UIElement.h"
#include "Utils.h"

UIElement::UIElement(UIBox frame, TNodePtr node)
{
	UIElement::frame	  = frame;
	nextLayer			  = NULL;
	parentView			  = NULL;
	previousElement		  = NULL;
	SetColor(&lowColor,0,1,0);
	
	parentElement		  = NULL;
	UIElement::node 	  = node;
	cursor				  = (BCursor *)B_CURSOR_SYSTEM_DEFAULT;	
	
	//The following values are UA dependants (we can choose them)
	minHeight = minWidth = 0;
	maxHeight = maxWidth = 3000;
}

UIElement::~UIElement()
{
	if (nextLayer){
		UIElement *element = NULL;
		BView	  *view    = NULL;
		while (nextLayer->CountItems() > 0){
			element = (UIElement *)(nextLayer->RemoveItem((int32)0));
			view = dynamic_cast <BView *> (element);
			if (view)
				parentView->RemoveChild(view); 
			delete element;
		}
	}
	delete nextLayer;
}

void UIElement::EAddChild(UIElement *element)
{	
	//automatically forward some needed data
	element->parentView = parentView;
	element->parentElement = this;
	
	//We assume here that we will never meet a RGB(0,1,0)
	//(I'd say it's fairly true.)
	if (SameColor(element->lowColor,SetColorSelf(0,1,0)))
		element->lowColor = lowColor;
		
	//If the Element inherit from BeOS UI Element,add it to BeOS BViews' tree.
	if (dynamic_cast <BView *> (element))
		parentView->AddChild(dynamic_cast <BView *> (element)); 
	
	if (!nextLayer)
		nextLayer = new BList();
	
	element->previousElement = (UIElement *)nextLayer->LastItem();
			
	nextLayer->AddItem(element);
}

/*This is done considering that overlapments do not exist on this 
  planet which is of course true. ;-]]	
  Note: This is THE (you see big letters I hope !) func to OPTIMIZE.
  it's called a billion times a nano-sec. Should we make the nextLayer by default
  constructed and therefore remove the test if (nextLayer) ?*/
UIElement *UIElement::FindElementFor(BPoint point)
{	
	//First see if any other "upper" layer can handle the BPoint...
	if (nextLayer) 
		for (int32 i=0; i<nextLayer->CountItems(); i++)
			if (((UIElement *)nextLayer->ItemAt(i))->frame.Contains(point))
				return ((UIElement *)nextLayer->ItemAt(i))->FindElementFor(point);
					
	return this;
}

/*Same here: overlapment is the product of your imagination, repeat after me:
  "overlapment is the product of my imagination"...*/
void UIElement::EDraw()
{
	/*Here should come the drawing instructions of the subclasses items
	  They know the rect they have to draw in: it's their 'frame' field 
	  which can be updated by the TRenderView on resize/etc...
	*/
	
	if (nextLayer)
		for(int32 i=0; i<nextLayer->CountItems(); i++)
			((UIElement *)nextLayer->ItemAt(i))->EDraw();	
}

void UIElement::EMessageReceived(BMessage *message)
{
	//Do nothing, implemented by derivated classes to
	//manage incoming messages ala BHandler::MessageReceived(BMessage *message)
}

void UIElement::EMouseDown(BPoint point)
{
	//Does nothing, implemented by derivated class to 
	//do what they need on MouseDown clicks
}

void UIElement::EMouseUp(BPoint point)
{
	//Does nothing, implemented by derivated class to 
	//do what they need on MouseUp clicks	
}

void UIElement::EMouseMoved(BPoint point, uint32 transit, const BMessage *message)
{
	//Nothing to do here, implemented by derivated class when a mouse moves above them
	//See TRenderView::MouseMoved()
}

void UIElement::EFrameResized(float width, float height)
{
	/*The normal scheme of implementation is:
		1) To find whether a min/max value has been hit
		2) Update the value of the frame of the object
		
	//Do the calculus of the new frame for the element (for derivated classes)
	
	//Applies the min/max-height/width rules
	/*TODO: SEE WHAT IT SHOULD CHANGE: content/margin/border/padding ??
		TODO: Calculate the new height and width see visudet.html#min-height
		//Where do apply rule number 2 ??? her or at computation of min/max
		float width = 0, height = 0;
		if (width < minWidth)
			width = minWidth;
		else if (width > maxWidth)
			width = maxWidth;
		
		if (height < minHeight)
			height = minHeight;
		else if (height > maxHeight)
			height = maxHeight;
			
		frame.SetContent(width,height);	
	*/	
	//Then forward to next layer
	if (nextLayer) 
		for (int32 i=0; i<nextLayer->CountItems(); i++)
			((UIElement *)nextLayer->ItemAt(i))->EFrameResized(width,height);	
}

//This version is used by all drawing elements but textelement ones that overrides it
ElementFrame UIElement::GetElementFrame()
{
	ElementFrame element = {false,frame,frame.ContentRect().RightBottom()};	
	return element;
}

void UIElement::FillBackgroundColor()
{
	parentView->SetLowColor(lowColor);

	if (SameColor(parentElement->lowColor,lowColor))	
		return;
	
	parentView->SetHighColor(lowColor);
	
	parentView->FillRect(frame);
}

//BRect UIElement::GetContainingBlock()
