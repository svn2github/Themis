#include "appaboutview.h"
#include "app.h"
#include <ScrollView.h>
appaboutview::appaboutview(BRect frame, const char *title, uint32 resizem, uint32 flags)
	:BView(frame,title,resizem,flags) {
	BRect r=Bounds();
	r.right-=B_V_SCROLL_BAR_WIDTH;
	tv=new BTextView(r,"About Themis Info",r,B_FOLLOW_ALL,B_WILL_DRAW|B_NAVIGABLE);
	BScrollView *sv=new BScrollView("about_app_scroller",tv,B_FOLLOW_ALL,B_WILL_DRAW|B_NAVIGABLE_JUMP,false,true,B_FANCY_BORDER);
	AddChild(sv);
	BString info;
	info << "Themis Web Browser [Framework] Version " <<(float)THEMIS_FRAMEWORK_APP_VERSION;
#ifdef USEBONE
	info << "\n\t\t(Compiled for BONE)";
#endif
#ifdef USENETSERVER
	info << "\n\t\t(Compiled for net_server)";
#endif
	info <<"\nhttp://themis.sourceforge.net\n"<< "Copyright © 2002\n"<<"Project Founder and Lead Developer:\n\tRaymond \"Z3R0 One\" Rodgers <z3r0_one@users.sourceforge.net\n";
	info << "Contributors:\n\t\tLinus Almstrom\n\t\tMark Hellegers\n";
	tv->MakeEditable(false);
	tv->Insert(info.String());
}
void appaboutview::AttachedToWindow() {
}