/*	Themis Renderer

	Olivier MILLA (Methedras) methedras@online.fr
	
	This code falls under the General Themis License.
*/
#include <TextControl.h>

#include "UIElement.h"

#ifndef _TEXT_CONTROL_ELEMENT_H_
#define _TEXT_CONTROL_ELEMENT_H_

class TextControlElement : public BTextControl , public UIElement
{
	public:
						TextControlElement(BRect frame, const char *name, 
										   const char *text, BMessage *message,
										   uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
										   uint32 flags = B_WILL_DRAW | B_NAVIGABLE);
		virtual			~TextControlElement();
		
		//Coming from the BeOS class inheritance		
		virtual void 	AttachedToWindow();	
};

#endif

