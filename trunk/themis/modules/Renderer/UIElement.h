/*	Themis Renderer

	Olivier MILLA (Methedras) methedras@online.fr
	
	This code falls under the General Themis License.
*/
#include <Cursor.h>
#include <List.h>
#include <Rect.h>
#include <View.h>

#include "TNode.h"

#include "UIBox.h"

#ifndef _UIELEMENT_H_
#define _UIELEMENT_H_

class TRenderView;

/*This ElementFrame struct is mainly to be able to get the framing of TextElemnts 
  that have shapes others than a simple rectangle. Used only by now with TextElement.
  isFrameOnly = true when not text element => startPoint = frame.LeftTop();	
  										   => endPoint   = frame.RightBottom();
  isFrameOnly = false when text element
  	exemple of what represent the datas:
  				jfdksqljlkfdqs   -
  			kjfldsqmlqdlkfdlkd   | frame.Height()
  			djkqskjdksdkjdssjd   |
  			djdkaijdjdizr	     -
  			|----------------|
  			   frame.Width()
	
	startPoing is at the leftop of the first text letter (a 'j').
	endPoint is at the rightbottom of the last letter (a 'r').
*/  			   
struct ElementFrame {
	bool	isFrameOnly;
	UIBox	mainframe;
//	BPoint	startPoint; Not sure it's useful
	BPoint  endPoint;
};

/*This class is never used as is, only derivated ones are.
  This class is the Base Drawing Element of the Rendered objects.
  It is meant to be used with our own objects inheriting from it of from BeOS
  UI objects (also derivating from it).*/
class UIElement
{
	public:
								UIElement(UIBox frame, TNodePtr node);
		virtual 				~UIElement();

				void			EAddChild(UIElement *element);
		virtual void			EDraw();
		virtual	void			EFrameResized(float width, float height); //Not sure the parameters are needed
																		  //Maybe should pass recusively the new width/height
																		  //of the parent element. Probably it's ContentRect's height/width
		virtual	void			EMouseDown(BPoint point);
		virtual void			EMouseUp(BPoint point);
		virtual	void			EMouseMoved(BPoint point, uint32 transit, const BMessage *message);
				
		virtual	void			EMessageReceived(BMessage *message);
								/*You keep the ownership of message
								  arg: you must delete it yourself if needed.*/
								  
		virtual ElementFrame	GetElementFrame();

				UIElement 		*FindElementFor(BPoint point);

				UIElement		*parentElement;
				
				UIBox 			frame;
								//This should be in the TRenderView coordinate system
				
				//The node from the DOM Tree				
				TNodePtr		node;
				
				//Some options of the element TODO: Use or Remove
				bool			isZoomable;
				
				rgb_color		lowcolor;
								
				BList 			*nextLayer; 

				TRenderView		*parentView;
				
				BCursor			*cursor;
				
				double			minHeight, maxHeight;
				double 			minWidth , maxWidth;
				
				//The previous element in parent's BList
				UIElement		*previousElement;

	protected:
				//Some UIElements call this on EFrameResized()
			//	void			ProportionalResizingAndMoving(float deltaWidth, float deltaHeight);
};

#endif
