/* See header for more info */

#include "commondefs.h"
#include "msgsystem.h"

#include "BitmapElement.h"
#include "TRenderer.h"

BitmapElement::BitmapElement(UIBox frame,  TNodePtr node, const char *url) : UIElement(frame,node)
{
	isZoomable = false;
	bitmap = NULL;
	
	frames   = 0;
	bmpFrame = BRect(0,0,frame.Width(),frame.Height());

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
