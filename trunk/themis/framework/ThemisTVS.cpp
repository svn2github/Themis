/* ThemisTVS.cpp */

// BeOS headers

// C/C++ headers
#include <iostream>

// myheaders
#include "ThemisTVS.h"
#include "MainWindow.h"

ThemisTVS::ThemisTVS( void )
	: BApplication( "application/x-vnd.ThemisTVS-app" )
{
	// create mainwindow
	mainwindow = new MainWindow();
	mainwindow->Show();
}

ThemisTVS::~ThemisTVS( void )
{
}

bool
ThemisTVS::QuitRequested( void )
{
	cout << "shutting down ThemisTVS ..." << endl;
	
	if( mainwindow )
	{
		cout << "mainwindow exists -> closing ..." << endl;
		mainwindow = 0;
	}
	cout << "shutdown finished" << endl;
	return true;
}

void
ThemisTVS::MessageReceived( BMessage *msg )
{
	switch( msg->what )
	{
		default :
			BApplication::MessageReceived( msg );
	}
}

int
main()
{
	ThemisTVS* ThemisTVSapp = new ThemisTVS();
	ThemisTVSapp->Run();
	delete ThemisTVSapp;

	return 0;
}