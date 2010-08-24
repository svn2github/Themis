/*
 * PrefsViews.h
 *
 * author: Michael Weirauch
 */

#ifndef _PREFSVIEWS_H_
#define _PREFSVIEWS_H_

// BeOS headers
#include <View.h>
#include <String.h>

// Declarations used
class BBox;
class BPopUpMenu;

/*
 * BasePrefsView
 */

class WindowPrefsView;


class BasePrefsView : public BView {

	protected:
		BBox * fMainBox;		

	public:
		BasePrefsView(BRect frame,
					  const char* name);
		
};


/*
 * WindowPrefsView
 */


class WindowPrefsView : public BasePrefsView {

	public:
		WindowPrefsView(BRect frame,
						const char* name);

};


/*
 * NetworkPrefsView
 */


class NetworkPrefsView : public BasePrefsView {

	public:
		NetworkPrefsView(BRect frame,
						 const char* name);

};


/*
 * PrivacyPrefsView
 */


class PrivacyPrefsView : public BasePrefsView {

	public:
		PrivacyPrefsView(BRect frame,
						 const char* name);

};


/*
 * HTML ParserPrefsView
 */


class HTMLParserPrefsView : public BasePrefsView {

	private:
		BPopUpMenu * mPopUpMenu;
		BString mDTDDir;

	public:
		HTMLParserPrefsView(BRect frame,
							const char* name);
		virtual void AttachedToWindow();

};


/*
 * CSS ParserPrefsView
 */


class CSSParserPrefsView : public BasePrefsView {

	private:
		BPopUpMenu * mPopUpMenu;
		BString mCSSDir;

	public:
		CSSParserPrefsView(BRect frame,
						   const char* name);
		virtual void AttachedToWindow();

};

/*
 * RendererPrefsView
 */


class RendererPrefsView : public BasePrefsView {

	public:
		RendererPrefsView(BRect frame,
						  const char* name);
									
};

#endif


/*
 * DigitOnlyMessageFilter
 */

#ifndef DIGITONLYMESSAGEFILTER
#define DIGITONLYMESSAGEFILTER

#include <MessageFilter.h>

#include <ctype.h>


class DigitOnlyMessageFilter : public BMessageFilter
{
	public:
		DigitOnlyMessageFilter()
			: BMessageFilter( B_ANY_DELIVERY, B_ANY_SOURCE )
		{
		};
		
		filter_result
		Filter(
			BMessage* msg,
			BHandler** target )
		{
			filter_result res = B_DISPATCH_MESSAGE;
			const char* bytes;
			
			switch( msg->what )
			{
				case B_KEY_DOWN :
				case B_KEY_UP :
				{
					msg->FindString( "bytes", &bytes );
					
					if( strlen( bytes ) > 1 )
					{
						res = B_SKIP_MESSAGE;
						break;
					}
					
					if(	*bytes == B_BACKSPACE ||
						*bytes == B_ENTER ||
						*bytes == B_TAB ||
						*bytes == B_DELETE ||
						*bytes == B_HOME ||
						*bytes == B_END ||
						*bytes == B_LEFT_ARROW ||
						*bytes == B_RIGHT_ARROW )
					{
						res = B_DISPATCH_MESSAGE;
						break;
					}
						
					if( !isdigit( *bytes ) )
					{
						res = B_SKIP_MESSAGE;
						break;
					}
				}
				default:
					break;
			}
			
			return res;
		};
};

#endif

