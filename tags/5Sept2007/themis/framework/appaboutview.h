#ifndef _app_about_view_
#define _app_about_view_
#include <View.h>
#include <TextView.h>
class appaboutview: public BView {
	public:
		appaboutview(BRect frame, const char *title, uint32 resizem, uint32 flags);
		void AttachedToWindow();
		BTextView *tv;
};
#endif
