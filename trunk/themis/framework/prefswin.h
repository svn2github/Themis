/* prefswindow.h */

#ifndef _PREFSWIN_H_
#define _PREFSWIN_H_

#include <ListView.h>
#include <String.h>
#include <View.h>
#include <Window.h>
#include <ColorControl.h>

class leftview;
class ColorBox;

class prefswin : public BWindow
{
	private:
		leftview*					lview;
		BView*						rview;
		BView*						bview;
		
		//ColorBox*					fMenuColorBox;
		ColorBox*					fPanelColorBox;
		ColorBox*					fThemeColorBox;
		ColorBox*					fActiveTabColorBox;
		ColorBox*					fInactiveTabColorBox;
		ColorBox*					fLightBorderColorBox;
		ColorBox*					fDarkBorderColorBox;
		ColorBox*					fShadowColorBox;
		
		// window prefs
		BString						fHomePage;
		bool						fIntelligentMaximize;
		bool						fShowTypeAhead;
		int8						fNewWindowStartPage;
		
		// tab prefs
		bool						fShowTabsAtStartup;
		bool						fOpenTabsInBackground;
		int8						fTabHistoryDepth;
		
		// font prefs
		
		// color prefs
		//rgb_color					fMenuColor;
		rgb_color					fPanelColor;
		rgb_color					fThemeColor;
		rgb_color					fActiveTabColor;
		rgb_color					fInactiveTabColor;
		rgb_color					fLightBorderColor;
		rgb_color					fDarkBorderColor;
		rgb_color					fShadowColor;
		
		// privacy prefs
		
		// HTML Parser prefs
		BString						fDTDToUsePath;
		
	public:
									prefswin(
										BRect frame,
										const char* title,
										window_look look,
										window_feel feel,
										uint32 flags,
										uint32 wspaces = B_CURRENT_WORKSPACE );
									~prefswin();
		void						MessageReceived( BMessage* msg );	
		bool						QuitRequested();
		
		void						SaveSettings();
};

#endif

/////////////////////////////////////
// leftview
/////////////////////////////////////

#ifndef LEFTVIEW
#define LEFTVIEW

class leftview : public BView
{
	public:
									leftview(
										BRect frame,
										const char* name,
										uint32 resizingmode,
										uint32 flags );
		
		void						AttachedToWindow();
					
		BListView*					listview;
};

#endif

/////////////////////////////////////
// ColorBox
/////////////////////////////////////

#ifndef COLORBOX
#define COLORBOX

class ColorBox : public BView
{
	public:
									ColorBox(
										BRect frame,
										const char* name,
										int colorindex,
										rgb_color startcolor );
		virtual						~ColorBox();
		virtual void				Draw( BRect updaterect );
		virtual void				MouseDown( BPoint point );
		
		void						SetColor( rgb_color color );
		
	private:
		int							fColorIndex;
		rgb_color					fColor;
};

#endif

/////////////////////////////////////
// ColorSelectWindow
/////////////////////////////////////

#ifndef COLORSELECTWINDOW
#define COLORSELECTWINDOW

#include <ColorControl.h>

class ColorSelectWindow : public BWindow
{
	public:
									ColorSelectWindow(
										BRect frame,
										const char* title,
										window_look look,
										window_feel feel,
										uint32 flags,
										BWindow* pwin,
										int	colorindex,
										rgb_color startcolor,
										uint32 wspaces = B_CURRENT_WORKSPACE );
									~ColorSelectWindow();
	
		BColorControl*				fColctrl;
	
	private:
		BWindow*					fParentWin;
};

#endif

/////////////////////////////////////
// DigitOnlyMessageFilter
/////////////////////////////////////

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
