#include <List.h>
#include <Rect.h>

#ifndef _UIELEMENT_H_
#define _UIELEMENT_H_

/*This class is never used as is, only derivated ones are.
  This class is the Base Drawing Element of the Rendered objects.
  It is meant to be used with our own objects inheriting from it of from BeOS
  UI objects (also derivating from it).*/
class UIElement{
	public:
								UIElement(BRect frame);
		virtual 				~UIElement();

				void			AddChildElement(UIElement *element);
		virtual void			DrawElement(BView *owner);

		virtual	void			PostTMessage(BMessage *message);
								/*You keep the ownership of message
								  arg: you must delete it yourself if needed.
								*/

				BRect 			frame;
								//This should be in the TRenderView coordinate system
								
				
				//Some options of the element
				bool			needUpdateOnMouseOver;
				bool			isZoomable;
								
				BList 			*nextLayer; 

				UIElement 		*FindElementFor(BPoint point);
};

#endif