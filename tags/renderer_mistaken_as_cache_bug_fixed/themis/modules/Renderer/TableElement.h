/*	Themis Renderer

	Olivier MILLA (Methedras) methedras@online.fr
	
	This code falls under the General Themis License.
*/
#include "Globals.h"
#include "Utils.h"
#include "UIElement.h"

#ifndef _TABLE_ELEMENT_H_
#define _TABLE_ELEMENT_H_

class TableElement : public UIElement
{
	public:
							TableElement(UIBox frame,  TNodePtr node, int cellpadding, 
										 int cellspacing, rgb_color bgcolor, 
										 rgb_color bordercolor);
		virtual 			~TableElement();
		
		virtual	void		EDraw();
		virtual void		EFrameResized(float deltaWidth, float deltaHeight);
		
		void	AddColumn();
		void	AddRow();
	
	private:
				int 		cellpadding;
				int 		cellspacing;
				int			rows;
				int			columns;
				rgb_color	bordercolor;
				BList		rects;
};

#endif

