/*	Themis Renderer

	Olivier MILLA (Methedras) methedras@online.fr
	
	This code falls under the General Themis License.
*/
#include <Font.h>
#include <String.h>

#include "UIElement.h"
#include "Globals.h"

#ifndef _TEXT_ELEMENT_H_
#define _TEXT_ELEMENT_H_

class TextElement : public UIElement
{
	public:
								TextElement(TNodePtr node, const char *text, 
										    BFont *font, rgb_color high, 
										    float size = 0);
									
		virtual 				~TextElement();
		
		virtual void			EDraw();
		virtual	void			EFrameResized(float width, float height);
		virtual ElementFrame	GetElementFrame();
				
	protected:
				//see ElementFrame in UIElement.h
				BPoint			startPoint;
				BPoint			endPoint;
				
				//The text as given by the parser 
				char		*text;
				
				BFont   	font;
				
				//Saved here to avoid GetHeight() call at each Drawing
				font_height	fh;
				
				//high color (low being into all UIElements)				
				rgb_color	highcolor;
				
				//Funcs and datas which are here to create the linedText array
				int32		TextForFrame(char *string);
				
				BList		startsOfLines;				
				BList		lengthsOfLines;
	
		/*Very important note about the frame field of UIElement. It's use
		is different from others class. In another class (Bitmap or whatever)
		the frame tells the drawing algorithm the frame in which it can draw.
		Here the frame is the result of the drawing and says where the text 
		actually is. The limits of the drawing are set by the parent 
		element (parent element containing the text).*/
};

#endif
