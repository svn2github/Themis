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
#include <Font.h>
#include <String.h>

#include "UIElement.h"
#include "Globals.h"

#ifndef _TEXT_ELEMENT_H_
#define _TEXT_ELEMENT_H_

struct LowColorRect {
	LowColorRect() { color = NULL; 
					 height = 0.0; 
					 width = 0.0; };					 
	~LowColorRect() {};
	
	float	  height;
	float     width;
	
	float     verticalOffset;
	
	rgb_color *color;
};

struct LineRect {
	LineRect() { width = 0.0 ; 
				 height.ascent = 0.0;
				 height.descent = 0.0;
				 height.leading = 0.0; };
	~LineRect() {};			 

	float 		width;
	font_height	height;
};

class TextElement : public UIElement
{
	public:
								TextElement(TNodePtr node, const char *text, 
										    BFont *font, rgb_color *high, rgb_color *low = NULL, t_alignment alignement = T_ALIGN_LEFT);
									
		virtual 				~TextElement();
		
		virtual void			EDraw();
		virtual	void			EFrameResized(float width, float height);
		virtual ElementFrame	GetElementFrame();
				
				void			AppendText(const char *text, BFont *font, rgb_color *high, rgb_color *low = NULL);
				
	protected:
				void			FillTextLowColor(int32 *k, BPoint currentPenPosition);
				
				int				alignement;
				//see ElementFrame in UIElement.h
				BPoint			startPoint;
				BPoint			endPoint;
				
				//The list of textElements
				BString		text;	// The complete text
				BList		textStarts; // the index of each different text Start, int32 *
				BList		textLengths; //the list of lengths as int32 *
				BList   	fonts;   // a list of BFont *
				BList		highColors; // a list of rgb_color *
				BList		lowColors; // a list of rgb_color *
				BList		lowColorsRects; // a list of LowColorRects * 
								
				//Funcs and datas which are here to create the linedText array
				int32		TextForFrame(char *string, int32 refCounter, LineRect *lineRect);
				int32		ListIndexForTextCursor(int32 textCursor);
				
				BList		startsOfLines;	//a list of int32 *	
				BList		lengthsOfLines; //Still needed ?
				BList       linesRects; // a list of LineRect * describing the distance between lines
				BList		spacesJumped; //a list of int32 * describing the number of space jumped at beginning of line

		/*Very important note about the frame field of UIElement. It's use
		is different from others class. In another class (Bitmap or whatever)
		the frame tells the drawing algorithm the frame in which it can draw.
		Here the frame is the result of the drawing and says where the text 
		actually is. The limits of the drawing are set by the parent 
		element (parent element containing the text).*/
};

#endif
