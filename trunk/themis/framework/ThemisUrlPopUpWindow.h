/* ThemisUrlPopUpWindow.h */

#ifndef _THEMISURLPOPUPWINDOW_H_
#define _THEMISURLPOPUPWINDOW_H_

// BeOS headers
#include <Window.h>

// Declarations used
class BScrollBar;
class BList;
class ThemisUrlPopUpView;

class ThemisUrlPopUpWindow : public BWindow {

	private:
		BWindow * parentwindow;
		ThemisUrlPopUpView * mUrlListView;
		BList * mUrlList;
		int32 mMaxHeight;
		float mItemHeight;
		
		
		uint32 lastitem;
		void SetUrlSelection(int aOffset = 0);
		void EmptyUrlList();

	public:
		ThemisUrlPopUpWindow(BWindow* aParent, BRect aFrame, BList * aList);
		~ThemisUrlPopUpWindow();
									
		virtual void MessageReceived(BMessage *msg);
		void ListToDisplay(BList* list);
		void ResizeToPrefered();

};

#endif

