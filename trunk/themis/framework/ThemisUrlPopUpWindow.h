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

class ThemisUrlPopUpWindow : public BWindow
{
	public:
									ThemisUrlPopUpWindow( BWindow* parent, BRect frame );
		virtual void				MessageReceived( BMessage *msg );
		
		void						ListToDisplay( BList* list );
		void						ResizeToPrefered();
		void						TruncateUrlStrings();
		
		ThemisUrlPopUpView*			urlpopupview;
		BScrollBar*					vscroll;
		
	private:
		BWindow*					parentwindow;
		
		uint32						lastitem;
		BList*						url_list;
		BList*						trunc_list;
};

#endif

