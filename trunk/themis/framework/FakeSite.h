/* FakeSite.h */

#ifndef _FAKESITE_H_
#define _FAKESITE_H_

// BeOS headers
#include <View.h>
#include <String.h>
#include <Bitmap.h>
#include <Window.h>

// C/C++ headers

// myheaders

class FakeSite : public BView
{
	public:
												FakeSite( BRect rect, const char* title, BWindow* win = NULL );
		virtual							~FakeSite( void );
		void								AttachedToWindow( void );
		void								Draw( BRect updaterect );
		virtual void				MouseDown( BPoint where );
		
		BWindow*						mainwin;
		
		BString							site_title;
		// add BString url_string later for now i use the url as title and url
		// ( better: i set the url as site_title atm )
		
		BBitmap*						site_fav_icon;
		
};

#endif
