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

class ThemisUrlListView;

class ThemisUrlPopUpView : public BView {

	public:
		ThemisUrlPopUpView(BRect frame);

		virtual void Draw(BRect updaterect);
		BListView * ulv;
		
};

#endif

/////////////////////////////////////
// ThemisUrlPopUpViewMessageFilter
/////////////////////////////////////

#ifndef THEMISURLPOPUPVIEWMESSAGEFILTER
#define THEMISURLPOPUPVIEWMESSAGEFILTER

class ThemisUrlPopUpViewMessageFilter : public BMessageFilter {

	private:
		int32 lastbutton;

	public:
		ThemisUrlPopUpViewMessageFilter(BWindow* win);

		virtual filter_result Filter(BMessage *msg, BHandler **target);
		
};

#endif
