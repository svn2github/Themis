/* ThemisUrlPopUpView.h */

#ifndef _THEMISURLPOPUPVIEW_H_
#define _THEMISURLPOPUPVIEW_H_

// BeOS headers
#include <View.h>
#include <ListView.h>
#include <MessageFilter.h>
#include <GraphicsDefs.h>

// C/C++ headers

// myheaders
#include "ThemisTVS.h"

class ThemisUrlListView;

class ThemisUrlPopUpView : public BView
{
	public:
									ThemisUrlPopUpView(
										BRect frame,
										rgb_color gray );
													 	 
		virtual void				Draw( BRect updaterect );
		
		ThemisUrlListView*			ulv;
		
		rgb_color					fDarkGrayColor;
};

#endif

/////////////////////////////////////
// ThemisUrlListView
/////////////////////////////////////
// its just in here for later customization
// i did nothing special in here afair :))

#ifndef THEMISURLLISTVIEW
#define THEMISURLLISTVIEW

class ThemisUrlListView : public BListView
{
	public:
									ThemisUrlListView(
										BRect frame );
};

#endif

/////////////////////////////////////
// ThemisUrlPopUpViewMessageFilter
/////////////////////////////////////

#ifndef THEMISURLPOPUPVIEWMESSAGEFILTER
#define THEMISURLPOPUPVIEWMESSAGEFILTER

class ThemisUrlPopUpViewMessageFilter : public BMessageFilter
{
	public:
									ThemisUrlPopUpViewMessageFilter( BWindow* win );
		virtual filter_result		Filter( BMessage *msg, BHandler **target );
		
		BWindow*					window;
		int32						lastbutton;
};

#endif
