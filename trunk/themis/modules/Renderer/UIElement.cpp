#include <stdio.h>

#include <Message.h>

#include "UIElement.h"


UIElement::UIElement(BRect frame)
{
	UIElement::frame 	  = frame;
	needUpdateOnMouseOver = false;
	isZoomable 			  = true;
	nextLayer			  = NULL;
}

UIElement::~UIElement()
{
	if (nextLayer)
		for (int32 j=0; j<nextLayer->CountItems(); j++)
			delete (UIElement *)(nextLayer->RemoveItem((int32)0));
	delete nextLayer;

}

void UIElement::AddChildElement(UIElement *element)
{	
	if (!nextLayer)
		nextLayer = new BList();

	nextLayer->AddItem(element);
}

/*This is done considering that overlapments do not exist on this 
  planet which is of course true. ;-]]	*/
UIElement *UIElement::FindElementFor(BPoint point)
{	
	//First see if any other "upper" layer can handle the BPoint...
	if (nextLayer) //We do this test before the calcul of the owning for speed
		for (int32 i=0; i<nextLayer->CountItems(); i++)
			if (((UIElement *)nextLayer->ItemAt(i))->frame.Contains(point))
				return ((UIElement *)nextLayer->ItemAt(i))->FindElementFor(point);
					
	return this;
}

/*Same here: overlapment is the product of your imagination, repeat after me:
  "overlapment is the product of my imagination"...*/
void UIElement::DrawElement(BView *owner)
{
	/*Here should come the drawing instructions of the subclasses items
	  They know the rect they have to draw in: it's their 'frame' field 
	  which can be updated by the TRenderView on resize/etc...
	*/
	
	if (nextLayer)
		for(int32 i=0; i<nextLayer->CountItems(); i++)
			((UIElement *)nextLayer->ItemAt(i))->DrawElement(owner);	
}

void UIElement::PostTMessage(BMessage *message)
{
	//Do nothing, implemented by derivated classes to
	//manage incoming messages ala BHandler::MessageReceived(BMessage *message)
}