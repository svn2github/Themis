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
#include <FilePanel.h>

#include "ButtonElement.h"

#ifndef _FILE_ELEMENT_H_
#define _FILE_ELEMENT_H_

class FileElement : public ButtonElement
{
	public:
							FileElement(UIBox frame, TNodePtr node, const char *name, const char *label, t_button_mode mode);
		virtual				~FileElement();	
		
		virtual	void		EMessageReceived(BMessage *message);
		
		//From BeOS inheritance
		virtual	void		AttachedToWindow();
		
	protected:
				BFilePanel	*panel;
	
	private:
				entry_ref	ref; //The ref of the selected file
};

#endif
