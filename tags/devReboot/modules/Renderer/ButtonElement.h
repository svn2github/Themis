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
#include <Button.h>

#include "UIElement.h"

#ifndef _BUTTON_ELEMENT_H_
#define _BUTTON_ELEMENT_H_

enum t_button_mode {
	RESET_BUTTON_MODE 	= 0,
	SUBMIT_BUTTON_MODE  = 1,
	IMAGE_BUTTON_MODE	= 2,
	PUSH_BUTTON_MODE	= 3,
	FILE_BUTTON_MODE	= 4
};

class ButtonElement : public BButton , public UIElement
{
	public:
							ButtonElement(UIBox frame,  TNodePtr node, const char *name, const char *label, t_button_mode mode);
		virtual				~ButtonElement();
	
		virtual void		EFrameResized(float width, float height);
		virtual	void		EMessageReceived(BMessage *message);
		
		//Coming from the BeOS class inheritance
		virtual	void		AttachedToWindow();
		
	protected:
				t_button_mode	buttonMode;
		
};

#endif

