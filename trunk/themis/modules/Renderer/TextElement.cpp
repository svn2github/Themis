/* See header for more info */

#include <stdio.h>
#include <stdlib.h>

#include <Message.h>
#include <SupportDefs.h>
#include <View.h>

#include "Globals.h"
#include "TextElement.h"

#define DEBUG 0

TextElement::TextElement(UIBox frame,  TNodePtr node, const char *text, BFont *font, rgb_color high,
						 float size) : UIElement(frame,node)
{
	//General options
	isZoomable 			  = false;
	
	//Member datas
	TextElement::text = text;
	
	TextElement::font = *font;
	if (size > 0)
		TextElement::font.SetSize(size);
	
	TextElement::high 	 = high;
	
	font->GetHeight(&fh);
	
	linedText = NULL;
	EFrameResized(frame.Width(),frame.Height());
}

TextElement::~TextElement()
{
	int32 i=0;
	
	//Free allocated memory for lines
	if (linedText){
		while (linedText[i]){
			free(linedText[i]);
			i++;
		}
		free (linedText);
	}

	//Free allocated memory for ends of lines	
	for (int32 i=0; i<endsOfLines.CountItems(); i++)
		delete (int32 *)(endsOfLines.RemoveItem((int32)0));	
}

void TextElement::EDraw()
{
	int32 i=0;
	BPoint where;
	where.Set(frame.LeftTop().x+3,frame.LeftTop().y+fh.ascent);	
	
	parentView->SetLowColor(lowcolor);
	parentView->SetHighColor(high);
	parentView->SetFont(&font);
		
	if (linedText){
		while (linedText[i]){
			parentView->DrawString(linedText[i],where);
			where.y += fh.ascent + fh.descent + fh.leading;
			i++;
		}
	}
	
	UIElement::EDraw();
}

int32 TextElement::TextForFrame(char *text)
{
	char temp     = '\0';	
	uint32 cursor = 1; //unsigned to avoid a compile warning below

	for (cursor = 1; cursor < strlen(text); cursor++){
		temp		 = text[cursor];
		text[cursor] = '\0';
		if (font.StringWidth(text) > frame.Width()){
			text[cursor] = temp;
			cursor--;
			//Word-wrapping for ends of lines
			while (text[cursor] != ' ' && text[cursor] != '-')
				cursor --;
			return cursor;
		}
		text[cursor] = temp;	
	}
		
	return (int32)cursor;
}

int32 TextElement::LineStartingAt(int32 n)
{
	int32 res = 0;

	for (int32 i = 0; i < n; i++)
		res += *((int32 *)(endsOfLines.ItemAtFast(i)));

	return res;
}	

//Is there real advantage of using ItemAtFast() instead of ItemAt() ?
//TODO: Avoid space at beginning of line
void TextElement::EFrameResized(float width, float height)
{

#if (DEBUG == 1)	
	bigtime_t now = real_time_clock_usecs();
#endif
	
	char *string  = (char *)text.String();
	int32 counter = 0;
	int32 i		  = 0;
	int32 cursor  = 0;
	
	//Free allocated memory for lines
	if (linedText){
		while (linedText[i]){
			free(linedText[i]);
			i++;
		}
		free (linedText);
	}
	
	//Free allocated memory for ends of lines
	for (i=0; i<endsOfLines.CountItems(); i++)
		delete (int32 *)(endsOfLines.RemoveItem((int32)0));	
			
	//Get the ends of the lines
	while (strlen(string) > 0){
		int32 *val = new int32;
				
		cursor = TextForFrame(string);
		*val   = cursor;
		endsOfLines.AddItem(val);
#if (DEBUG ==1)
		printf("strlen of string\"%s\" at the turn %d: %d\n",string,counter+1,strlen(string));
		printf("cursor: %d\n",cursor);
#endif
		string += cursor;	
			
		counter++;
	}
	
#if (DEBUG == 1)
	printf("Are added the %d followings:",endsOfLines.CountItems());
	for (i=0; i < endsOfLines.CountItems(); i++)
		printf(" %d",*((int32 *)endsOfLines.ItemAtFast(i)));
	printf("\n");
#endif
	
	//Allocates the array of pointers to the lines
	linedText = (char **)malloc((counter+1)*sizeof(char *));
	for (i=0; i < counter+1; i++)
		linedText[i] = NULL;
	
	//Allocates and Fills lines
	for(i=0; i < counter; i++){
		linedText[i] = (char *)malloc(*((int32 *)(endsOfLines.ItemAtFast(i)))+1);
		text.CopyInto(linedText[i],LineStartingAt(i),*((int32 *)(endsOfLines.ItemAtFast(i))));
		linedText[i][*((int32 *)(endsOfLines.ItemAtFast(i)))] = '\0';		

#if (DEBUG == 1)
		printf("linedText[%d] = %s\n",i,linedText[i]);
		printf("\tAllocated: %d\n",*((int32 *)(endsOfLines.ItemAtFast(i)))+1);
		printf("\tCopied from %d for %d\n",LineStartingAt(i),*((int32 *)(endsOfLines.ItemAtFast(i))));
#endif
	}
		
#if (DEBUG == 1)	
	printf("time: %d\n",real_time_clock_usecs() - now);
#endif

	UIElement::EFrameResized(width,height);
}


