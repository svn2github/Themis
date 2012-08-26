/* ThemisTabView.h */

#ifndef _THEMISTABVIEW_H_
#define _THEMISTABVIEW_H_

// BeOS headers
#include <MessageFilter.h>
#include <TabView.h>

// C/C++ headers

// Declarations used
class TPictureButton;

class ThemisTabView : public BTabView {

	private:
		// for doubleclick detection on empty tabview-space
		// ( doubleclick opens new tab )
		uint32 lastbutton;
		float tab_width;
		bool fake_single_view;
		TPictureButton * close_tabview_button;

	public:
		ThemisTabView(
			BRect frame,
			const char* name,
			button_width width,
			uint32 resizingmode,
			uint32 flags);

		virtual void AttachedToWindow();
		virtual void Draw(BRect updaterect);
		virtual void DrawBox(BRect selTabRect);
		virtual BRect DrawTabs(void);
		virtual void MakeFocus(bool focus);
		virtual void MouseDown(BPoint point);
		virtual void Select(int32 tabindex);
		virtual void SetFocusTab(int32 tab, bool focused);
		void CreateCloseTabViewButton();
		void DynamicTabs(bool newtab);
		void SetFakeSingleView();
		void SetNavButtonsByTabHistory();
		void SetNormalTabView();
		float GetTabWidth() const;
		void DisableFakeSingleView();
		void MessageReceived(BMessage * aMessage);
		const char * GetPreviousHistoryEntry();
		const char * GetNextHistoryEntry();
		void SetHistoryDepth(int8 aDepth);
		void ClearHistory();

};

#endif

/////////////////////////////////////
// ContainerViewMessageFilter
/////////////////////////////////////

#ifndef CONTAINERVIEWMESSAGEFILTER
#define CONTAINERVIEWMESSAGEFILTER

// Declarations used
class Win;

class ContainerViewMessageFilter : public BMessageFilter {

	private:
		Win * window;
	
	public:
		ContainerViewMessageFilter(Win * win);
		virtual filter_result Filter(BMessage * msg, BHandler **target);
		
};

#endif
