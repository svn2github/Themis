#include <Application.h>
#include <Messenger.h>
#include <Window.h>

#include "Globals.h"
#include "TRenderWindow.h"

TRenderWindow::TRenderWindow(TRenderView *view) : BWindow(BRect(100,100,700,450),"Rendering...nothing but \"ing\" like \"wing\".",B_TITLED_WINDOW,B_NOT_ANCHORED_ON_ACTIVATE)
{
	panel = NULL;
	AddChild(TRenderWindow::view = view);
	
	BMessenger messenger(view);
	BMessage message(R_WELCOME);
	message.AddRect("rect",Bounds());
	
	messenger.SendMessage(&message);
	
	SetTitle(view->Name());
}

TRenderWindow::~TRenderWindow()
{
	delete panel;
}

void TRenderWindow::MessageReceived(BMessage *message)
{
	switch (message->what)
		default:
			BWindow::MessageReceived(message);
}

bool TRenderWindow::QuitRequested()
{
	return true;
}