/* ThemisUrlPopUpView.h */

#ifndef _THEMISURLPOPUPVIEW_H_
#define _THEMISURLPOPUPVIEW_H_

// BeOS headers
#include <View.h>
#include <ListView.h>
#include <MessageFilter.h>
#include <GraphicsDefs.h>

// Declarations used
class UrlItem;
class BScrollBar;

class ThemisUrlPopUpView : public BView {

	private:
		BListView * ulv;
		BScrollBar * mScrollBar;

	public:
		ThemisUrlPopUpView(BRect frame);

		virtual void Draw(BRect updaterect);
		const char * SetUrlSelection(int aOffset);
		int32 CurrentSelection();
		void MakeEmpty();
		void AddList(BList * aList);
		UrlItem * FirstItem();
		int32 CountItems();
		void SetScrollBarRange(float aMin, float aMax);
		void RemoveScrollBar();
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
