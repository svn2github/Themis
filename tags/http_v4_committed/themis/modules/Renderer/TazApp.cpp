#include <Alert.h>
#include <Button.h>
#include <Message.h>

#include "TazApp.h"
#include "TRenderView.h"
#include "TRenderWindow.h"
#include "UIBox.h"


TazApplication::TazApplication() : BApplication("application/x-vnd.TazCodeSousBe")
{	
	UIBox box(200,200);
	TNodePtr node;
	TRenderView *view = new TRenderView(box,node);
	TRenderWindow *win = new TRenderWindow(view);
	
	win->Show();
}

TazApplication::~TazApplication()
{}