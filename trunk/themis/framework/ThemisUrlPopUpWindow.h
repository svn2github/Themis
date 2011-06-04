/* ThemisUrlPopUpWindow.h */

#ifndef _THEMISURLPOPUPWINDOW_H_
#define _THEMISURLPOPUPWINDOW_H_

// BeOS headers
#include "Window.h"
#include <ScrollBar.h>

// C/C++ headers

// myheaders
#include "ThemisUrlPopUpView.h"
#include "Window.h"

class ThemisUrlPopUpWindow : public BWindow {

	private:
		BWindow * parentwindow;
		
		uint32 lastitem;
		BList * url_list;
		BScrollBar * vscroll;
		ThemisUrlPopUpView * urlpopupview;

	public:
		ThemisUrlPopUpWindow(BWindow* aParent, BRect aFrame, BList * aList);
		~ThemisUrlPopUpWindow();
									
		virtual void MessageReceived(BMessage *msg);
		void ListToDisplay(BList* list);
		void ResizeToPrefered();
		bool HasScrollBar() const;
		

};

#endif

