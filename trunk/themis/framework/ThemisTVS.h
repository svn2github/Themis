/* ThemisTVS.h */

#ifndef _THEMISTVS_H_
#define _THEMISTVS_H_

// Message Constants
#define BUTTON_BACK				'bbac'
#define BUTTON_FORWARD			'bfwd'
#define BUTTON_STOP				'bstp'
#define BUTTON_RELOAD			'brel'
#define BUTTON_HOME				'bhom'
#define CLOSE_OTHER_TABS		'clot'
#define CLOSE_URLPOPUP			'clpu'
#define SHOW_PREFERENCES		'shpr'
#define TAB_ADD					'tadd'
#define TAB_CLOSE				'tclo'
#define URL_TYPED				'urlt'
#define URL_LOADING				'urll'
#define URL_OPEN				'urlo'
#define URL_SELECT_MOUSE		'ursm'
#define URL_SELECT_NEXT			'ursn'
#define URL_SELECT_PREV			'ursp'
/*
// BeOS headers
#include <Application.h>
#include <Message.h>

// C/C++ headers

// myclasses
class MainWindow;

class ThemisTVS : public BApplication
{
	public:
												ThemisTVS( void );
		virtual							~ThemisTVS( void );
		virtual bool				QuitRequested( void );
		virtual void				MessageReceived( BMessage *msg );
		
		MainWindow					*mainwindow;
};
*/
#endif
