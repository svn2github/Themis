/*	Themis Renderer

	Olivier MILLA (Methedras) methedras@online.fr
	
	This code falls under the General Themis License.
*/
#include <Button.h>

#include "UIElement.h"

#ifndef _BUTTON_ELEMENT_H_
#define _BUTTON_ELEMENT_H_

class ButtonElement : public BButton , public UIElement
{
	public:
						ButtonElement(BRect frame, const char *name, const char *label, 
								  	  BMessage *message, 
								  	  uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP, 
								  	  uint32 flags = B_WILL_DRAW | B_NAVIGABLE);
		virtual			~ButtonElement();
		
		//Coming from the BeOS class inheritance
		virtual	void	AttachedToWindow();
		
};

#endif

