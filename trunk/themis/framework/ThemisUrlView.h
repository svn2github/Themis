/* ThemisUrlView.h */

#ifndef _THEMISURLVIEW_H_
#define _THEMISURLVIEW_H_

// BeOS headers
#include <View.h>
#include <Bitmap.h>
#include <TextView.h>
#include <MessageFilter.h>
#include <Window.h>

// C/C++ headers

// myheaders

class ThemisUrlTextView;

class ThemisUrlView : public BView
{
	public:
									ThemisUrlView(
										BRect frame,
										const char* name,
										uint32 resizingmode,
										uint32 flags );
										
		virtual void				AttachedToWindow();											 	 
		virtual void				Draw( BRect updaterect );
		virtual void				MouseDown( BPoint point );
		
		const char*					Text();
		BTextView*					TextView();
		void						SetFavIcon( BBitmap* fav );
		virtual void				SetText( const char* newtext );
		
		ThemisUrlTextView*			textview;
	
	private:	
		BBitmap*					fav_icon;
};

#endif

/////////////////////////////////////
// ThemisUrlTextView
/////////////////////////////////////

#ifndef THEMISURLTEXTVIEW
#define THEMISURLTEXTVIEW

class ThemisUrlTextView : public BTextView
{
	public:
									ThemisUrlTextView(
										BRect frame,
										const char* name,
										BRect textrect,
										uint32 resizingmode,
										uint32 flags );
		
		void						MouseDown(
										BPoint point );
		
		void						Paste(
										BClipboard* clipboard );
};

#endif

/////////////////////////////////////
// ThemisUrlViewMessageFilter
/////////////////////////////////////

#ifndef THEMISURLVIEWMESSAGEFILTER
#define THEMISURLVIEWMESSAGEFILTER

class ThemisUrlViewMessageFilter : public BMessageFilter
{
	public:
									ThemisUrlViewMessageFilter( BWindow* win );
		virtual filter_result		Filter( BMessage *msg, BHandler **target );
		
		BWindow*					window;
	
};

#endif
