/* prefswindow.h */

#ifndef _PREFSWIN_H_
#define _PREFSWIN_H_

#include <Window.h>
#include <View.h>

class prefswin : public BWindow
{
	private:
		//aboutview *view;
	
	public:
									prefswin(
										BRect frame,
										const char* title,
										window_look look,
										window_feel feel,
										uint32 flags,
										uint32 wspaces = B_CURRENT_WORKSPACE );
									~prefswin();
		void						MessageReceived( BMessage* msg );	
		bool						QuitRequested();
};

#endif
