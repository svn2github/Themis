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
	UIBox	mainFrame;
//	BPoint	startPoint; Not sure it's useful
	BPoint  endPoint;
};

/*
typedef enum css_positioning_scheme {
	CSS_NORMAL_FLOW			=0,
	CSS_FLOAT				=1,
	CSS_ABSOLUTE_POSITION	=2;
};

struct CSSContext{
	css_flow;	
};*/

/*This class is never used as is, only derivated ones are.
  This class is the Base Drawing Element of the Rendered objects.
  It is meant to be used with our own objects inheriting from it of from BeOS
  UI objects (also derivating from it).*/
class UIElement
{
	public:
								UIElement(UIBox frame, TNodePtr node); //IMPORTANT NOTE: By now the frame argument is useless as it is never taken into account in the drawing mechanisms 
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
								  arg: you must delete it yourself (if needed !!!).*/
								  
		virtual ElementFrame	GetElementFrame();

				UIElement 		*FindElementFor(BPoint point);

				UIElement		*parentElement;
				
				UIBox 			frame;
								//This should be in the TRenderView coordinate system
				
				//The node from the DOM Tree				
				TNodePtr		node;
				
				rgb_color		lowColor;
								
				BList 			*nextLayer; 

				TRenderView		*parentView;
				
				BCursor			*cursor;
				
				double			minHeight, maxHeight;
				double 			minWidth , maxWidth;
				
				//The previous element in parent's nextLayer BList
				UIElement		*previousElement;

	protected:
				//A method that fills the frame with the backgroud color
				//and set the correct lowColor
				void			FillBackgroundColor();	
				
//				CSSContext		cssContext;
				
//				BRect			GetContainingBlock();
};

#endif
