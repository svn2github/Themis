#include <View.h>

#include "UIElement.h"

#ifndef _TRENDERVIEW_H_
#define _TRENDERVIEW_H_

/*The top View which manage the tree of DrawElements.
  That's the view we draw in.
*/
class TRenderView : public BView , public UIElement
{
	public:
								TRenderView(BRect frame);
		virtual					~TRenderView();
		virtual	void			Draw(BRect updateRect);
		virtual void			FrameResized(float width, float height);
		virtual void			MouseDown(BPoint point);
		virtual void			MouseMoved(BPoint point, uint32 transit, const BMessage *message);
	
	private:
				UIElement		*currentMouseOver;
};

#endif
