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

#include <Message.h>
#include <Messenger.h>

#include "FileElement.h"
#include "Globals.h"

FileElement::FileElement(UIBox frame, TNodePtr node, const char *name, const char *label, t_button_mode mode) 
			  : ButtonElement(frame,node,name,label,mode)
{
	panel = NULL;
}

FileElement::~FileElement()
{
	delete panel;
}

void FileElement::AttachedToWindow()
{
	//Create the file panel
	BMessenger panelTarget((BHandler *)parentView);
	BMessage panelMessage(FILE_ELEMENT_PANEL_MESSAGE);
	panelMessage.AddPointer("element",this);
	
	panel = new BFilePanel(B_OPEN_PANEL,&panelTarget,NULL,B_FILE_NODE,false,&panelMessage,NULL);
	
	//Set Message Target
	BMessage *message = new BMessage(FILE_ELEMENT_BUTTON_MESSAGE);
	message->AddPointer("element",this);
	SetMessage(message);
	SetTarget((BHandler *)parentView);
	
	BButton::AttachedToWindow();
}

void FileElement::EMessageReceived(BMessage *message)
{	
	switch(message->what){
		case FILE_ELEMENT_BUTTON_MESSAGE:{
			panel->Show();
			}break;
		case FILE_ELEMENT_PANEL_MESSAGE:{
			message->FindRef("refs",&ref);
			}break;
		default:
			BView::MessageReceived(message);
	}
}



