#ifndef _app_about_view_
#define _app_about_view_

// BeOS headers
#include <View.h>

// Declarations of BeOS classes
class BTextView;

class appaboutview: public BView {
	
	private:	
		BTextView *tv;
	
	public:
		appaboutview(BRect frame, const char *title, uint32 resizem, uint32 flags);
		void AttachedToWindow();
};
#endif
