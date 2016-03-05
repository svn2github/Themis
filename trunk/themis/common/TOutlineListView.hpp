/*
	TOutlineListView
	Same as a BOutlineListView, but properly sizes the horizontal scrollbar
	
	Mark Hellegers (mark@firedisk.net)
	05-03-2016

*/

#ifndef TOUTLINELISTVIEW_HPP
#define TOUTLINELISTVIEW_HPP

// BeOS headers
#include <OutlineListView.h>

// Declarations used
class BScrollView;

class TOutlineListView : public BOutlineListView {

	private:
		BScrollView * fScroller;
		float fPrefWidth;

	public:
		TOutlineListView(
			BRect aFrame,
			const char * aName,
			list_view_type aType = B_SINGLE_SELECTION_LIST,
			uint32 aResizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
			uint32 aFlags = B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE);
		virtual ~TOutlineListView();
		
		virtual void Draw(BRect aUpdateRect);
		virtual void TargetedByScrollView(BScrollView * aScroller);

// Haiku's version of GetPreferredSize works fine
#ifndef HAIKU
		virtual void GetPreferredSize(float * aWidth, float * aHeight);
#endif
	
};

#endif
