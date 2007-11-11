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
#include "commondefs.h"
#include "msgsystem.h"

#include "BitmapElement.h"
#include "TRenderer.h"

BitmapElement::BitmapElement(UIBox frame, TNodePtr node, const char *url) : UIElement(frame,node)
{
	bitmap = NULL;
	
	frames   = 0;
	bmpFrame = BRect(0,0,frame.Width(),frame.Height());

	BitmapElement::isClickable = isClickable;
	
	//Send a message to the Image handler so it downloads the image,
	//turn it into a series of BBitmap and send it back with it's 
	//additional infos (animation, period, number of images
//	BMessage message(IH_LOAD_IMAGE);
//	message.AddPointer("element",(void *)this);
//	message.AddString("URL",url);
	
//	parentView->renderer->Broadcast(MS_TARGET_IMAGE_HANDLER,&message);
}

BitmapElement::~BitmapElement()
{
	//delete bitmap ?
	//TODO: delete the pointer of pointers
}

void BitmapElement::EDraw()
{
	if (bitmap)
		parentView->DrawBitmapAsync(bitmap,bmpFrame,frame);
}

void BitmapElement::EFrameResized(float width, float height)
{
	ElementFrame prevFrame;
	
	if (bitmap) {
		frame = bitmap->Bounds();
		
		if (previousElement){	
			prevFrame = previousElement->GetElementFrame();
			frame.OffsetTo(prevFrame.mainFrame.ContentRect().LeftBottom()); //Move the frame to it's correct position
		}
		else {
			prevFrame = parentElement->GetElementFrame();
			frame.OffsetTo(prevFrame.mainFrame.ContentRect().LeftTop());	//Move the frame to it's correct position
		}
		
		UIElement::EFrameResized(frame.Width(),frame.Height());
	}
}

void BitmapElement::EMouseUp(BPoint point)
{
	//Send message for the link	if any
}


