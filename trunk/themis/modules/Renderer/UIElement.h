/*	Themis Renderer

	Olivier MILLA (Methedras) methedras@online.fr
	
	This code falls under the General Themis License.
*/
#include <List.h>
#include <Rect.h>
#include <View.h>

#ifndef _UIELEMENT_H_
#define _UIELEMENT_H_

/*This class is never used as is, only derivated ones are.
  This class is the Base Drawing Element of the Rendered objects.
  It is meant to be used with our own objects inheriting from it of from BeOS
  UI objects (also derivating from it).*/
class UIElement
{
	public:
								UIElement(BRect frame);
		virtual 				~UIElement();

				void			EAddChild(UIElement *element);
		virtual void			EDraw();
		virtual	void			EFrameResized(float deltaWidth, float deltaHeight);
			
		virtual	void			EMouseDown(BPoint point);
		virtual void			EMouseUp(BPoint point);
		virtual	void			EMouseMoved(BPoint point, uint32 transit, const BMessage *message);
				
		virtual	void			PostTMessage(BMessage *message);
								/*You keep the ownership of message
								  arg: you must delete it yourself if needed.*/

				BRect 			frame;
								//This should be in the TRenderView coordinate system
								
				
				//Some options of the element
				bool			isZoomable;
				
				rgb_color		lowcolor;
								
				BList 			*nextLayer; 

				UIElement 		*FindElementFor(BPoint point);

				BView			*parentView;

	protected:
				//Some UIElements call this on EFrameResized()
				void			ProportionalResizingAndMoving(float deltaWidth, float deltaHeight);
};

#endif
