/* prefswindow.cpp */

#include "prefswin.h"
#include "app.h"

prefswin::prefswin(
	BRect frame,
	const char* title,
	window_look look,
	window_feel feel,
	uint32 flags,
	uint32 wspaces )
	: BWindow( frame, title, look, feel, flags, wspaces ) 
{
	/*view=new aboutview(Bounds(),"aboutview",B_FOLLOW_ALL,B_WILL_DRAW|B_NAVIGABLE_JUMP);
	AddChild(view);
	SetSizeLimits(500,1000,200,400);
	*/
	Show();
}

prefswin::~prefswin() 
{
}

bool
prefswin::QuitRequested() 
{
	( ( App* )be_app )->PWin = NULL;
	return true;
}

void prefswin::MessageReceived( BMessage* msg )
{
	switch( msg->what ) {
		default:
			BWindow::MessageReceived( msg );
	}
	
}