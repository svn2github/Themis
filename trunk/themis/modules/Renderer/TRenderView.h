/*	Themis Renderer

	Olivier MILLA (Methedras) methedras@online.fr
	
	This code falls under the General Themis License.
*/
#include <Messenger.h>
#include <View.h>

#include "UIBox.h"
#include "UIElement.h"

#ifndef _TRENDERVIEW_H_
#define _TRENDERVIEW_H_

/*The top View which manage the tree of DrawElements.
  That's the view we draw in.*/
class TRenderView : public BView , public UIElement
{
	public:
								TRenderView(UIBox frame, TNodePtr node);
		virtual					~TRenderView();
		
		virtual void			SetLowColor(rgb_color color);
		
		virtual	void			Draw(BRect updateRect);
		virtual void			FrameResized(float width, float height);
		
		virtual void			MouseDown(BPoint point);
		virtual	void			MouseUp(BPoint point);
		virtual void			MouseMoved(BPoint point, uint32 transit, const BMessage *message);
	
		virtual	void			MessageReceived(BMessage *message);
		
				BMessenger		userInterface; 
				
	private:
				//Pointer to the UIElement which has currently the mouse over it.
				UIElement		*currentMouseOver;
};

#endif
