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
									FakeSite(
										BRect rect,
										const char* title,
										int16 uid,
										BWindow* win = NULL );
		virtual						~FakeSite( void );
		void						AttachedToWindow( void );
		void						Draw( BRect updaterect );
		virtual void				MouseDown( BPoint where );
		
		bool						GetCookieState();
		int							GetDocBarProgress();
		const char*					GetDocBarText();
		int							GetImgBarProgress();
		const char*					GetImgBarText();
		bool						GetSecureState();
		const char*					GetStatusText();
		
		void						SetInfo(
										int doc_progress,
										bool delta_doc,
										const char* doc_text,
										int img_progress,
										bool delta_img,
										const char* img_text,
										const char* statustext,
										bool sec,
										bool cook_dis );
		int16						UniqueID();
										
		BWindow*					mainwin;
		
		BString						site_title;
		// add BString url_string later for now i use the url as title and url
		// ( better: i set the url as site_title atm )
		
		BBitmap*					site_fav_icon;
	private:
		int							fDocProgress;
		char*						fDocText;
		int							fImgProgress;
		char*						fImgText;
		bool						fSecure;
		bool						fCookiesDisabled;
		BString						fStatusText;
		
		int16						fUniqueID;
};

#endif
