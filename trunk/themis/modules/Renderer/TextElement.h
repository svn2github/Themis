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
						TextElement(BRect frame, const char *text, BFont *font,
									rgb_color high, float size = 0);
									
		virtual 		~TextElement();
		
		virtual void	EDraw();
		virtual	void	EFrameResized(float width, float height);
				
	protected:
				//The text as given by the parser and the one cut in lines
				//to enter the element's frame.
				BString	 	text;
				char		**linedText;
				
				BFont   	font;
				
				//Saved here to avoid GetHeight() call at each Drawing
				font_height	fh;
				
				//high color (low being into all UIElements)				
				rgb_color	high;
				
				//Funcs and datas which are here to create the linedText array
				int32		TextForFrame(char *text);
		inline	int32		LineStartingAt(int32 n);
								
				BList		endsOfLines;
};

#endif
