/* ThemisStatusView.h */

#ifndef _THEMISSTATUSVIEW_H_
#define _THEMISSTATUSVIEW_H_

// BeOS headers
#include <View.h>
#include <String.h>
#include <Bitmap.h>

// C/C++ headers

// myheaders

class ThemisProgressBar;

class ThemisStatusView : public BView
{
	public:
									ThemisStatusView(
										BRect rect );
		virtual						~ThemisStatusView( void );
		void						AttachedToWindow( void );
		void						Draw( BRect updaterect );
		
		
		void						SetLoadingInfo(
										int doc_progress,
										const char* status_text );
		
		void						SetValues(
										int doc_progress,
										const char* doc_text,
										int img_progress,
										const char* img_text,
										const char* status_text,
										bool sec,
										bool cook_dis );
	private:
		ThemisProgressBar*			fDocumentBar;
		ThemisProgressBar*			fImageBar;
		BBitmap*					fDocBmp;
		BBitmap*					fImgBmp;
		BBitmap*					fSecureBmp;
		BBitmap*					fInsecureBmp;
		BBitmap*					fCookieBmp;
		BBitmap*					fCookieDisabledBmp;
		BString						fStatusText;
		bool						fSecure;
		bool						fCookiesDisabled;
};

#endif

/////////////////////////////////////
// ThemisProgressBar
/////////////////////////////////////

#ifndef THEMISPROGRESSBAR
#define THEMISPROGRESSBAR

class ThemisProgressBar : public BView
{
	public:
									ThemisProgressBar(
										BRect rect,
										const char* name,
										uint32 resizingmode,
										uint32 flags );
		virtual void				Draw( BRect updaterect );
		void						Reset();
		void						SetValue( int value, const char* bartext );

	private:
		int							fProgress;
		BString						fBarText;
};



/////////////////////////////////////
// ProgressBarDocumentIcon
/////////////////////////////////////

const unsigned char pbar_doc_icon [] = {
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,
	0x00,0x00,0x00,0xff,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,
	0x00,0x00,0x00,0xff,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff
};

/////////////////////////////////////
// ProgressBarImageIcon
/////////////////////////////////////

const unsigned char pbar_img_icon [] = {
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0xff,0xff,0xff,0xff,0x96,0x96,0x96,0xff,0x00,0x00,0x00,0xff,
	0x96,0x96,0x96,0xff,0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,
	0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0x00,0x00,0x00,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0x00,0x00,0x00,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,
	0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0xff,0xff,0xff,0xff,
	0x96,0x96,0x96,0xff,0x00,0x00,0x00,0xff,0x96,0x96,0x96,0xff,
	0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff
};

/////////////////////////////////////
// ProgressBarInsecureIcon
/////////////////////////////////////

const unsigned char pbar_insecure_icon [] = {
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff
};

/////////////////////////////////////
// ProgressBarSecureIcon
/////////////////////////////////////

const unsigned char pbar_secure_icon [] = {
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0xff,0xff,0xff,0xff,
	0x00,0x00,0x00,0xff,0x00,0x00,0x00,0xff,0x00,0x00,0x00,0xff,
	0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0xff,0xff,0xff,0xff,
	0x00,0x00,0x00,0xff,0x00,0x00,0x00,0xff,0x00,0x00,0x00,0xff,
	0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,
	0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,0x00,0x00,0x00,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff
};

/////////////////////////////////////
// ProgressBarCookieIcon
/////////////////////////////////////

const unsigned char pbar_cookie_icon [] = {
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0xff,0xff,0xff,0xff,0x96,0x96,0x96,0xff,0x00,0x00,0x00,0xff,
	0x96,0x96,0x96,0xff,0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x96,0x96,0x96,0xff,0x00,0x00,0x00,0xff,
	0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,0x96,0x96,0x96,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0x00,0x00,0x00,0xff,
	0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,
	0x00,0x00,0x00,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,
	0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x96,0x96,0x96,0xff,0x00,0x00,0x00,0xff,
	0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,0x96,0x96,0x96,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0xff,0xff,0xff,0xff,
	0x96,0x96,0x96,0xff,0x00,0x00,0x00,0xff,0x96,0x96,0x96,0xff,
	0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff
};

/////////////////////////////////////
// ProgressBarCookieIcon
/////////////////////////////////////

const unsigned char pbar_cookie_disabled_icon [] = {
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0x00,0x00,0xff,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0xff,0xff,0xff,0xff,0x96,0x96,0x96,0xff,0x00,0x00,0x00,0xff,
	0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x96,0x96,0x96,0xff,0x00,0x00,0x00,0xff,
	0xff,0xff,0xff,0xff,0x00,0x00,0xff,0xff,0x96,0x96,0x96,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0x00,0x00,0x00,0xff,
	0xff,0xff,0xff,0xff,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,
	0x00,0x00,0x00,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0xff,0xff,
	0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x96,0x96,0x96,0xff,0x00,0x00,0xff,0xff,
	0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,0x96,0x96,0x96,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0xff,0xff,0xff,0xff,
	0x00,0x00,0xff,0xff,0x00,0x00,0x00,0xff,0x96,0x96,0x96,0xff,
	0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,0x64,0x64,0x64,0xff,
	0x64,0x64,0x64,0xff
};

#endif
