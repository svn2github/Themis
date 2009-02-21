#include "appaboutview.h"
#include "app.h"
#include <ScrollView.h>

appaboutview::appaboutview(
	BRect frame,
	const char* title,
	uint32 resizem, uint32 flags )
	: BView( frame, title, resizem, flags)
{
	BRect r = Bounds();
	r.right -= B_V_SCROLL_BAR_WIDTH;
	tv = new BTextView(
		r,
		"About Themis Info",
		r,
		B_FOLLOW_ALL,
		B_WILL_DRAW | B_NAVIGABLE  );
	
	BScrollView* sv = new BScrollView(
		"about_app_scroller",
		tv,
		B_FOLLOW_ALL,
		B_WILL_DRAW | B_NAVIGABLE_JUMP,
		false,
		true,
		B_FANCY_BORDER );
	
	AddChild(sv);
	
	BString info;
	info << "Themis Web Browser [Framework] Version " <<(float)THEMIS_FRAMEWORK_APP_VERSION;
#ifdef USEBONE
	info << "\n\t\t(Compiled for BONE)\n";
#endif
#ifdef USENETSERVER
	info << "\n\t\t(Compiled for net_server)\n";
#endif
	info << "http://themis.sourceforge.net | http://www.themisbrowser.org\n";
	info << "Copyright © 2000-2003\n";
	info << "Project Founder and Lead Developer:\n";
	info << "\tRaymond \"Z3R0 One\" Rodgers\n";
	info << "\t<z3r0_one@users.sourceforge.net>\n\n";
	info << "Contributors:\n";
	info << "\t\tLinus Almstrom\n";
	info << "\t\tMark Hellegers\n";
	info << "\t\tOlivier Milla\n";
	info << "\t\tMichael Weirauch\n";
	info << "\n\n\"Themis is 95% MORE Evil than Microsoft's Internet Explorer!\"\n";
	
	tv->MakeEditable( false );
	tv->Insert( info.String() );
}

void
appaboutview::AttachedToWindow()
{
}
