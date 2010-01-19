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
#include <stdio.h>
#include <stdlib.h>

#include <Message.h>
#include <SupportDefs.h>
#include <View.h>

#include "Globals.h"
#include "TextElement.h"
#include "TRenderView.h"
#include "Utils.h"

#define RDEBUG 	0
#define RDEBUG2	0
#define RDEBUG3 0
#define RDEBUG4 1
#define RSPEED 	0

#define TEXT_GENERAL_INDENT 3

/* What's left TODO in the TextElement :
	_ Fix little bug with the lowColor in some very particular cases
	_ Implement the indent used to begin paragraphs (easy)
	_ Try to switch all char * use to BString use for multi-sized character problems
	_ Try to avoid making copies in the algorithm (2 by now IIRC)
	_ Implement the Rect Tracking for links, change color and all things related to text links
	_ Try to minimize the number of GetStringWidth calls to avoid going back and forth to the app_server (Optimization) 
	_ Rewrite FindText() func in TRendererEngine.cpp and make the Process() function pass all possible parameters to the TextElement
*/

//the font and high MUST be allocated on the heap
TextElement::TextElement(TNodePtr node, const char *text, BFont *font, rgb_color *high, rgb_color *low, t_alignment alignement) : UIElement(UIBox(0, 0),node)
{	
	//Member datas
	TextElement::text = text;
	TextElement::alignement = alignement;
	
	int32 *val = new int32;
	*val = 0;
	int32 *len = new int32;
	*len = strlen(text);
	textStarts.AddItem(val);
	highColors.AddItem(high);
	textLengths.AddItem(len);
	fonts.AddItem(font);	
	
	if (!low){
		low = new rgb_color;
		SetColor(low,0,1,0);
	}
	lowColors.AddItem(low);
	
	frame.SetPadding(TEXT_GENERAL_INDENT,0,TEXT_GENERAL_INDENT,0);

	//just inits. the values don't mean a thing
	startPoint = frame.ContentRect().LeftTop();
	endPoint   = frame.ContentRect().RightBottom();
}

TextElement::~TextElement()
{
	//Free allocated memory for lengths of lines
	while (textLengths.CountItems() > 0)
		delete (int32 *)(textLengths.RemoveItem((int32)0));	
		
	//Free allocated memory for lengths of lines
	while (lengthsOfLines.CountItems() > 0)
		delete (int32 *)(lengthsOfLines.RemoveItem((int32)0));	
			
	//Free allocated memory for starts of lines
	while (startsOfLines.CountItems() > 0)
		delete (int32 *)(startsOfLines.RemoveItem((int32)0));	

	//Free allocated memory for textStarts
	while (textStarts.CountItems() > 0)
		delete (int32 *)(textStarts.RemoveItem((int32)0));	
		
	//Free allocated memory for starts of lines
	while (fonts.CountItems() > 0)
		delete (BFont *)(fonts.RemoveItem((int32)0));	

	//Free allocated memory for starts of lines
	while (highColors.CountItems() > 0)
		delete (rgb_color *)(highColors.RemoveItem((int32)0));	

	//Free allocated memory for starts of linesHeights
	while (linesRects.CountItems() > 0)
		delete (LineRect *)(linesRects.RemoveItem((int32)0));	
		
	//Free allocated memory for starts of linesHeights
	while (spacesJumped.CountItems() > 0)
		delete (int32 *)(spacesJumped.RemoveItem((int32)0));	

	//Free allocated memory for starts of linesHeights
	while (lowColors.CountItems() > 0)
		delete (rgb_color *)(lowColors.RemoveItem((int32)0));	

	//Free allocated memory for starts of linesHeights
	while (lowColorsRects.CountItems() > 0)
		delete (LowColorRect *)(lowColorsRects.RemoveItem((int32)0));																	
}


void TextElement::EDraw()
{
	BPoint where = frame.ContentRect().LeftTop(); 	
	float  startOfLineRef = where.x;
		
#if (RSPEED == 1)	
	bigtime_t now = real_time_clock_usecs();
#endif 

#if (RDEBUG == 1)	
	printf("EDraw: Start Point = ");
	where.PrintToStream();
#endif
	
	int32 j = 0;
	int32 k = 0;
	int32 nlineCursor = 0;
	int32 lineCursor = 0;
	int32 textCursor = 0;	
				  	
	const char *string = text.String();
	for (int32 i=0; i < startsOfLines.CountItems(); i++){
		lineCursor = 0;
		textCursor += *(int32 *)spacesJumped.ItemAtFast(i);
		LineRect *lineRect = (LineRect *)linesRects.ItemAtFast(i);
		where.x = startOfLineRef;
		switch (alignement){ 
			case T_ALIGN_CENTER:	where.x += (parentElement->frame.ContentRect().Width() - lineRect->width )/2; break;
			case T_ALIGN_RIGHT:		where.x += parentElement->frame.ContentRect().Width()  - lineRect->width; break;
			default:	    		break; //case T_ALIGN_LEFT
		}				
		if (i > 0){ 
			where.y += ((LineRect *)linesRects.ItemAtFast(i-1))->height.descent;
			where.y += lineRect->height.ascent;
			where.y += lineRect->height.leading;		
		}
		else //first line
			where.y += lineRect->height.ascent;

		parentView->MovePenTo(where);	
		while ((lineCursor + *(int32 *)textLengths.ItemAtFast(j) - textCursor) <= *(int32 *)lengthsOfLines.ItemAtFast(i)){
			parentView->SetHighColor(*(rgb_color *)highColors.ItemAtFast(j));
			FillTextLowColor(&k,parentView->PenLocation()); //Also set lowColor
			parentView->SetFont((BFont *)fonts.ItemAtFast(j));
			parentView->DrawString(string + nlineCursor + lineCursor,*(int32 *)textLengths.ItemAtFast(j) - textCursor);			
			lineCursor += *(int32 *)textLengths.ItemAtFast(j) - textCursor;
			textCursor = 0;		
			j++;
			if (j == textLengths.CountItems()) //We are done drawing
				return;
		}
		parentView->SetHighColor(*(rgb_color *)highColors.ItemAtFast(j));
		FillTextLowColor(&k,parentView->PenLocation()); //Also set lowColor
		parentView->SetFont((BFont *)fonts.ItemAtFast(j));		
		parentView->DrawString(string + nlineCursor + lineCursor,*(int32 *)lengthsOfLines.ItemAtFast(i) - lineCursor);
		textCursor  += *(int32 *)lengthsOfLines.ItemAtFast(i) - lineCursor; 				
		if (i < (startsOfLines.CountItems() - 1))
			nlineCursor = *(int32 *)startsOfLines.ItemAtFast(i+1);		
	}
#if (RDEBUG == 1)		
	//2 drawing for debugs
	parentView->SetHighColor(RGB_RED);
	parentView->StrokeRect(frame);
	parentView->StrokeLine(frame.MarginRect().LeftTop(),endPoint);
#endif
	
#if (RSPEED == 1)		
	printf("drawing time: %d microsecs\n",real_time_clock_usecs() - now);
#endif
	
	UIElement::EDraw();
}

//TextFrame. that's THE ONE to optimize to get speed improvment
//but works quite farely by now though linear
//Could probably even be moved to recursive
int32 TextElement::TextForFrame(char *string, int32 refCounter, LineRect *lineRect)
{
	char temp     = '\0';	
	int32 cursorMem = 1;
	BFont *font = NULL;
	int32 textListCursor = 0;
				
	int32 globalCursor = 0; 
	float globalStringWidth = 0;
	
	int32 localStringRefCounter = refCounter;
	int32 localCursor = 0;
	int32 localLength = 0; 		
	char *localString = NULL;

	for (textListCursor = ListIndexForTextCursor(refCounter); textListCursor < textStarts.CountItems(); textListCursor++){
		font = (BFont *)fonts.ItemAtFast(textListCursor);
		if ((textListCursor + 1) < textStarts.CountItems()){
			localLength = *(int32 *)textStarts.ItemAtFast(textListCursor + 1) - localStringRefCounter;
			localStringRefCounter = *(int32 *)textStarts.ItemAtFast(textListCursor + 1); //Update for next turns. permits for the first turn to take into account beginning of line in the mddle of a type of text
		}
		else
			localLength = strlen(string);
	
		localString = (char *)malloc((localLength+1)*sizeof(char));
		strncpy(localString,string + globalCursor,localLength);
		localString[localLength] = '\0';

		LowColorRect *lowColorRect = new LowColorRect();
		lowColorRect->color = (rgb_color *)lowColors.ItemAtFast(textListCursor);
		font_height fh;
		font->GetHeight(&fh);
		lowColorRect->verticalOffset = fh.ascent + fh.leading;
		lowColorRect->height = lowColorRect->verticalOffset + fh.descent;
		 
#if (RDEBUG3 == 1)
	BString str4;
	text.CopyInto(str4,refCounter + globalCursor,localLength);
	printf("TEXT RENDERERING:TextForFrame(): localString is now: %s\n",str4.String());		
#endif
		//now that we are here we have the localLength, localString
		//We so start the algorithm on this line
		for (localCursor = 1; localCursor < (localLength + 1); localCursor++){			
			temp		 = localString[localCursor];
			localString[localCursor] = '\0';	
#if (RDEBUG3 == 1)	
	printf("TEXT RENDERERING:TextForFrame(): Considering localString as %s\n",localString);
	printf("TEXT RENDERERING:TextForFrame(): The Comparison is then: (%f + %f) > %f\n",globalStringWidth,font->StringWidth(localString),parentElement->frame.ContentRect().Width());	
#endif	
			if ((globalStringWidth + font->StringWidth(localString)) > parentElement->frame.ContentRect().Width()){
				localString[localCursor] = temp;
				localCursor--;	
#if (RDEBUG3 == 1)
	BString str;
	text.CopyInto(str,refCounter,globalCursor + localCursor);
	printf("TEXT RENDERERING:TextForFrame(): text cut INIT: %s\n",str.String());		
#endif
				//Word-wrapping for ends of lines
				cursorMem = localCursor;
				while (localString[localCursor] != ' ' && localString[localCursor] != '-'){
					if (localCursor > 0){
						localCursor --;
					}	
					else { //if the frame is reduced less than the size of 1 word we print all we can
#if (RDEBUG3 == 1)
	BString str2;
	text.CopyInto(str2,refCounter,globalCursor + cursorMem);
	printf("TEXT RENDERERING:TextForFrame(): 0 encounter, returning %s\n",str2.String());
#endif
						if (globalCursor > 0){
							lowColorRect->width = globalStringWidth;
							lowColorsRects.AddItem(lowColorRect);
							lineRect->width = globalStringWidth;
							free(localString);	//We are sure to quit the func: free used mem									
							return globalCursor;
						}
						else {
							temp = localString[cursorMem];
							localString[cursorMem] = '\0';						
							lowColorRect->width = font->StringWidth(localString);
							localString[cursorMem] = temp;
							lowColorsRects.AddItem(lowColorRect);	
							lineRect->width = lowColorRect->width; //equivalent to font->StringWidth(localString) but faster													
							free(localString);	//We are sure to quit the func: free used mem									
							return cursorMem;						
						}
					}
				}
#if (RDEBUG3 == 1)
	BString str3;
	text.CopyInto(str3,refCounter,globalCursor + localCursor);
	printf("TEXT RENDERERING:TextForFrame(): space/- encountered, returning %s\n",str3.String());
#endif		
				temp = localString[localCursor];
				localString[localCursor] = '\0';						
				lowColorRect->width = globalStringWidth + font->StringWidth(localString);
				localString[localCursor] = temp;				
				lowColorsRects.AddItem(lowColorRect);
				lineRect->width = lowColorRect->width; //equivalent to font->StringWidth(localString) but faster													
				free(localString);	//We are sure to quit the func: free used mem														
				return globalCursor + localCursor;								
			}
			//else
			localString[localCursor] = temp;					
		}
		lowColorRect->width = font->StringWidth(localString);
		lowColorsRects.AddItem(lowColorRect);		
		globalStringWidth += lowColorRect->width;  //globalStringWidth += font->StringWidth(localString);			
		globalCursor += localLength;
		free(localString);				
	}
	
	return globalCursor;
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
	
//	int32 stringLength = text.CountChars();
	char *string  = text.LockBuffer((int32)0);
	int32 cursorMem = 0;
	int32 cursor  = 0;
	int32 startMem = 0;
	
	//Free allocated memory for lengths of lines
	while (lengthsOfLines.CountItems() > 0)
		delete (int32 *)(lengthsOfLines.RemoveItem((int32)0));	
			
	//Free allocated memory for starts of lines
	while (startsOfLines.CountItems() > 0)
		delete (int32 *)(startsOfLines.RemoveItem((int32)0));	

	//Free allocated memory for starts of linesHeights
	while (linesRects.CountItems() > 0)
		delete (LineRect *)(linesRects.RemoveItem((int32)0));	

	//Free allocated memory for starts of linesHeights
	while (spacesJumped.CountItems() > 0)
		delete (int32 *)(spacesJumped.RemoveItem((int32)0));	

	//Free allocated memory for starts of linesHeights
	while (lowColorsRects.CountItems() > 0)
		delete (LowColorRect *)(lowColorsRects.RemoveItem((int32)0));	
			
	//Cut the lines
	while (string[0] != '\0'){  //Equivalent here to while(strlen(string) > 0) but faster
		int32 *length = new int32;
		int32 *start  = new int32;
		int32 *jump   = new int32;
		*jump = 0;
		
		//Avoid space at beginning of line:
		while (string[cursor] == ' '){
 			(*jump)++;
			cursor++;
		}
		
		spacesJumped.AddItem(jump);
		
#if (RDEBUG == 1)	
		printf("start of line = %d\n",cursor);
#endif 		

#if (RDEBUG3 == 1)	
	printf("----------- Line %d ---------\n",startsOfLines.CountItems());
#endif	
 		//Here are local start and length (of the shortened string!)	
 		*start = cursor;
 		startMem = *start;
		LineRect *lineRect = new LineRect(); 		
		*length = TextForFrame(string + cursor, cursorMem + cursor,lineRect);
		
		//Now get the distance that should be used (the max one) 
		//between this line and previous one
		font_height	fh;
		int32 maxIndex = ListIndexForTextCursor(*length + cursorMem + cursor) + 1;
		
		for (int32 i=ListIndexForTextCursor(cursorMem + cursor); i <  maxIndex; i++){
			((BFont *)fonts.ItemAtFast(i))->GetHeight(&fh);
				
			if (fh.ascent > lineRect->height.ascent)
				lineRect->height.ascent = fh.ascent;
			if (fh.descent > lineRect->height.descent)
				lineRect->height.descent = fh.descent;
			if (fh.leading > lineRect->height.leading)
				lineRect->height.leading = fh.leading;								
		}
		//Add it to the BList
		linesRects.AddItem(lineRect);	

#if (RDEBUG4 == 1)
	if ((dynamic_cast <TextElement *> (parentElement)) != NULL)
		printf("ParentElement is text.");
	else
		printf("ParentElement has unknown type.\n");
		
	parentElement->frame.PrintUIBoxToStream();
#endif

#if (RDEBUG == 1)	
		printf("length of line = %d\n",*length);
		if (*length == 0){
			printf("RENDERER (BIG BAD UGLY) ERROR in TEXTELEMENT: length of line = 0 (Themis will now commit suicide!! bye...)\n");
			thread_id id = find_thread(NULL);
			thread_info info;
			get_thread_info(id,&info);
			kill_team(info.team);
			return; //Quit the infinite loop
		}
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

#if (RSPEED == 1)	
	printf("line cutting time: %d microseconds\n",real_time_clock_usecs() - now);
#endif 
	
	text.UnlockBuffer();

#if (RDEBUG2 ==1)
	BString report;
	report << "---------------------------\n" << "The initial strings are:\n";
	for (int32 k=0; k < textStarts.CountItems(); k++){
		report.Append("\t");
		report.Append(text.String() + *(int32 *)textStarts.ItemAt(k),*(int32 *)textLengths.ItemAt(k));
		report.Append("\n");
	}
	report.Append("\n");
	report.Append("The lines are cut:\n");
	for(int32 k=0; k < startsOfLines.CountItems(); k++){
		report.Append("\t");
		report.Append(text.String() + *(int32 *)startsOfLines.ItemAt(k),*(int32 *)lengthsOfLines.ItemAt(k));
		report.Append("\n");
	}
	printf("%s",report.String());		 
#endif

	//Now we have to update the frame of the TextElement 
	//We have to guess now the frame that will be produced when actually drawing with EDraw()		
	ElementFrame prevFrame;
	
	if (previousElement){	
		prevFrame = previousElement->GetElementFrame();
		frame.SetLeftTop(prevFrame.mainFrame.ContentRect().LeftBottom()); //That sets the LeftTop BPoint
	}
	else {
		ElementFrame prevFrame = parentElement->GetElementFrame();
		frame.SetLeftTop(prevFrame.mainFrame.ContentRect().LeftTop());			//That sets the LeftTop BPoint
	}

	//Give startPoint it's value
	startPoint = frame.ContentRect().LeftTop();
				
	//Do calculus of endPoint
	char *lastLine = text.LockBuffer((int32)0);
	lastLine += *(int32 *)startsOfLines.LastItem();
		
	endPoint.x = frame.ContentRect().left;
	endPoint.x += ((BFont *)fonts.LastItem())->StringWidth(lastLine) + TEXT_GENERAL_INDENT;
	text.UnlockBuffer();
		
	endPoint.y = frame.ContentRect().top; 
	LineRect *lineRectCursor = NULL;
	for (int32 i=0; i < linesRects.CountItems(); i++){
		lineRectCursor = (LineRect *)linesRects.ItemAtFast(i);
		endPoint.y += lineRectCursor->height.ascent + lineRectCursor->height.descent + lineRectCursor->height.leading;
	}
	
	//Set the RightBottom of frame
	frame.SetRightBottom(BPoint(prevFrame.mainFrame.ContentRect().RightBottom().x,endPoint.y));		

#if (RDEBUG == 1)				
	//2 debugs line		
	printf("Text Frame n°%d now equals:\n",this);
	frame.PrintToStream();
#endif
		
	UIElement::EFrameResized(frame.Width(),frame.Height());
}

ElementFrame TextElement::GetElementFrame()
{
	ElementFrame element = {false,frame,endPoint};	
	return element;	
}

int32 TextElement::ListIndexForTextCursor(int32 textCursor)
{
	int32 index  = 1;
	int32 *cursor = NULL;

	while(1){
		cursor = (int32 *)textStarts.ItemAt(index);	
		if (cursor){
			if (*cursor < textCursor)
				index++;
			else
				return index-1;
		}
		else
			return index - 1;					
	}

	//To avoid a warning: never reached
	return index;
}

void TextElement::FillTextLowColor(int32 *k, BPoint currentPenPosition)
{
	LowColorRect *lowColorRect = (LowColorRect *)lowColorsRects.ItemAtFast(*k);

	if (SameColor(*lowColorRect->color,0,1,0))
		parentView->SetLowColor(lowColor); 	
	else {
		parentView->SetLowColor(*lowColorRect->color); 	
		BRect rect(0,0,lowColorRect->width,lowColorRect->height);
		currentPenPosition.y -= lowColorRect->verticalOffset;
		rect.OffsetTo(currentPenPosition);
		parentView->FillRect(rect,B_SOLID_LOW);
	}

	(*k)++;
}

void TextElement::AppendText(const char *text, BFont *font, rgb_color *high, rgb_color *low)
{
	TextElement::text.Append(text);
	
	int32 *len = new int32;
	*len = strlen(text);
	int32 *val = new int32;
	*val = *(int32 *)textStarts.LastItem() + *(int32 *)textLengths.LastItem();
	
	textStarts.AddItem(val);
	highColors.AddItem(high);
	textLengths.AddItem(len);
	fonts.AddItem(font);		
	
	if (!low){	
		low = new rgb_color;
		SetColor(low,0,1,0);
	}
	lowColors.AddItem(low);
		
}

