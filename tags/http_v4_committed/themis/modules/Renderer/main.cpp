#include "TazApp.h"

int main()
{
	TazApplication *app = new TazApplication();
	app->Run();
	delete app;
	
	return B_OK;	
}