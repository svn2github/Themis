/* See header for more info */

#include <stdio.h>
#include <stdlib.h>

#include <Message.h>
#include <SupportDefs.h>
#include <View.h>

#include "Globals.h"
#include "TextElement.h"
#include "TRenderView.h"

#define RDEBUG 0
#define RSPEED 0

TextElement::TextElement(TNodePtr node, const char *text, BFont *font, rgb_color high,
						 float size) : UIElement(frame,node)
{
	//General options
	isZoomable 			  = false;
	
	//Member datas
	TextElement::text = const_cast <char *> (text);
	
	TextElement::font = *font;
	if (size > 0)
		TextElement::font.SetSize(size);
	
	highcolor 	 = high;
	
	font->GetHeight(&fh);
	
	//just inits. the values don't mean a thing
	startPoint = frame.LeftTop();
	endPoint   = frame.RightBottom();
}

TextElement::~TextElement()
{
	//Free allocated memory for lengths of lines
	while (lengthsOfLines.CountItems() > 0)
		delete (int32 *)(lengthsOfLines.RemoveItem((int32)0));	
			
	//Free allocated memory for starts of lines
	while (startsOfLines.CountItems() > 0)
		delete (int32 *)(startsOfLines.RemoveItem((int32)0));			
}

void TextElement::EDraw()
{
#if (RSPEED == 1)	
	bigtime_t now = real_time_clock_usecs();
#endif 
	BPoint where;
	where.Set(frame.LeftTop().x+3,frame.LeftTop().y+fh.ascent);	
	
	parentView->SetLowColor(lowcolor);
	parentView->SetHighColor(highcolor);
	parentView->SetFont(&font);

	for (int32 i=0; i < startsOfLines.CountItems(); i++){
		parentView->DrawString(text + *(int32 *)startsOfLines.ItemAtFast(i),
							   *(int32 *)lengthsOfLines.ItemAtFast(i),where);
								   
		where.y += fh.ascent + fh.descent + fh.leading;
	}

#if (RSPEED == 1)		
	printf("drawing time: %d microsecs\n",real_time_clock_usecs() - now);
#endif

	UIElement::EDraw();
}

/* An attempt to optimize the spped of the TextForFrame method. Doesn't work yet
int32 TextElement::TextForFrame(char *string)
{
	char 	temp     = '\0';
	float 	width   = parentElement->frame.ContentRect().Width();
	uint32  length = strlen(string); //To avoid recomputing each time
	uint32 	cursor = (uint32) (width / font.StringWidth("m")); //unsigned to avoid a compile warning below
	uint32 	cursorMem = 1;
	bool	issup = true;
	
	if (cursor < length){
		temp = string[cursor];
		string[cursor] = '\0';
		issup =	(font.StringWidth(string) > width);	
	}
	else{
		cursor = 1;
		issup  = false;
	}
				
#if (RDEBUG == 1)	
		printf("TextFrame::strlen = %d\n",strlen(string));
#endif 
	
	while (cursor < length || cursor == 1){
		temp		 = string[cursor];
		string[cursor] = '\0';
		if (font.StringWidth(string) > width){
			if (issup = false){
				string[cursor] = temp;
				cursor--;
				//Word-wrapping for ends of lines
				cursorMem = cursor;
				while (string[cursor] != ' ' && string[cursor] != '-')
					if (cursor > 0)
						cursor --;
					else //if the frame is reduced less than the size of 1 word we print all we can
						return (int32)cursorMem;
							
				return (int32)cursor;				
			}
			else 
				cursor--;
		}
		else {
			if (issup = true){
				string[cursor] = temp;
				//Word-wrapping for ends of lines
				cursorMem = cursor;
				while (string[cursor] != ' ' && string[cursor] != '-')
					if (cursor > 0)
						cursor --;
					else //if the frame is reduced less than the size of 1 word we print all we can
						return (int32)cursorMem;
						
				return (int32)cursor;				
			}
			else
				cursor++;
		}	
		string[cursor] = temp;	
	}
		
	return (int32)cursor;
}
*/

//TextFrame methe. Should certainly be optimized
//but works quite farely by now
int32 TextElement::TextForFrame(char *string)
{
	char temp     = '\0';	
	uint32 cursor = 1; //unsigned to avoid a compile warning below
	uint32 cursorMem = 1;
	
#if (RDEBUG == 1)	
		printf("TextFrame::strlen = %d\n",strlen(string));
#endif 
	
	uint32 length = strlen(string); //To avoid recomputing each time
	for (cursor = 1; cursor < length; cursor++){
		temp		 = string[cursor];
		string[cursor] = '\0';
		if (font.StringWidth(string) > parentElement->frame.ContentRect().Width()){
			string[cursor] = temp;
			cursor--;
			//Word-wrapping for ends of lines
			cursorMem = cursor;
			while (string[cursor] != ' ' && string[cursor] != '-')
				if (cursor > 0)
					cursor --;
				else //if the frame is reduced less than the size of 1 word we print all we can
					return (int32)cursorMem;
						
			return (int32)cursor;
		}
		string[cursor] = temp;	
	}
		
	return (int32)cursor;
}


//Is there real advantage of using ItemAtFast() instead of ItemAt() ?
//TODO: Test if copy needed when code goes back into Themis.
void TextElement::EFrameResized(float width, float height)
{
#if (RSPEED == 1)	
	bigtime_t now = real_time_clock_usecs();
#endif
	
#if (RDEBUG == 1)		
	printf("-------------------------------\n");
#endif
	
//	char *string  = (char *)text.String();
	char *string  = strdup(text);
	char *ref     = string;
	int32 cursorMem = 0;
	int32 cursor  = 0;
	int32 startMem = 0;
	
	//Free allocated memory for lengths of lines
	while (lengthsOfLines.CountItems() > 0)
		delete (int32 *)(lengthsOfLines.RemoveItem((int32)0));	
			
	//Free allocated memory for starts of lines
	while (startsOfLines.CountItems() > 0)
		delete (int32 *)(startsOfLines.RemoveItem((int32)0));	
	
	//Cut the lines
	while (string[0] != '\0'){  //Equivalent here to while(strlen(string) > 0) but faster
		int32 *length = new int32;
		int32 *start  = new int32;
		
		//Avoid space at beginning of line:
		while (string[cursor] == ' ')
 			cursor++;

#if (RDEBUG == 1)	
		printf("start of line = %d\n",cursor);
#endif 		
 		//Here are local start and length (of the shortened string!)	
 		*start = cursor;
 		startMem = *start;
		*length = TextForFrame(string + cursor);

#if (RDEBUG == 1)	
		printf("length of line = %d\n",*length);
#endif 		
		string += *length + *start;	
#if (RDEBUG ==1)
		printf("string is reduced to\"%s\" after turn %d.\n",string,startsOfLines.CountItems()+1);
#endif		

		//Local start is converted back to global value
		*start  += cursorMem;
		
#if (RDEBUG ==1)
		printf("Saving start = %d (= %d + %d) and length = %d.\n",*start,*start - cursorMem,cursorMem,*length);
#endif	
		//Values are saved
 		startsOfLines.AddItem(start);
		lengthsOfLines.AddItem(length);

#if (RDEBUG ==1)
		int32 cprint = cursorMem;
#endif	 				
		//We have to keep the count from the beginning of the complete string		
		cursorMem += *length + startMem; 
		
		cursor = 0;
	}
	
	free(ref);

#if (RSPEED == 1)	
	printf("line cutting time: %d microseconds\n",real_time_clock_usecs() - now);
#endif 
	
	//Now we have to update the frame of the TextElement 
	//We have to guess now the frame that will be produced when actually drawing with EDraw()
/*	if (previousElement == NULL){
		//Do calculus of startPoint (to be added if needed)
		
		//Do calculus of endPoint
		char *lastLine = strdup(text + *(int32 *)startsOfLines.LastItem());
		endPoint = parentElement->frame.ContentRect().LeftBottom();
		endPoint.y += font.StringWidth(lastline);
		free(lastLine);
		
		//Set the RightBottom of frame
		frame.SetRightBottom(BPoint(parentElement->frame.ContentRect().RightBottom().x,endPoint.y)
	} */ 
		
	UIElement::EFrameResized(width,height);
}

ElementFrame TextElement::GetElementFrame()
{
	ElementFrame element = {false,frame,endPoint};	
	return element;	
}
