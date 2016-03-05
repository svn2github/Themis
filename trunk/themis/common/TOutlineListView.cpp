/* TOutlineListView implementation
   See TOutlineListView.hpp for more information
*/

// TOutlineListView header
#include "TOutlineListView.hpp"

// BeOS headers
#include <ScrollView.h>

TOutlineListView :: TOutlineListView(
						BRect aFrame,
						const char * aName,
						list_view_type aType,
						uint32 aResizingMode,
						uint32 aFlags)
				 : BOutlineListView(
				 	aFrame,
				 	aName,
				 	aType,
				 	aResizingMode,
				 	aFlags) {

	fScroller = NULL;
	fPrefWidth = 0;
}

TOutlineListView :: ~TOutlineListView() {

}

void TOutlineListView :: Draw(BRect aUpdateRect) {

	BOutlineListView::Draw(aUpdateRect);

	float prefWidth;
	float prefHeight;
	GetPreferredSize(&prefWidth, &prefHeight);
	
	if (fPrefWidth != prefWidth) {
		BRect rect = Bounds();
		float proportion = 1;
		if (rect.Width() < prefWidth) {
			proportion = rect.Width() / prefWidth;
		}
			
		if (fScroller != NULL) {
			BScrollBar * scrollBar = fScroller->ScrollBar(B_HORIZONTAL);
			if (scrollBar != NULL) {
				scrollBar->SetProportion(proportion);
				scrollBar->SetRange(0, prefWidth - rect.Width());
			}
		}
		fPrefWidth = prefWidth;
	}
	
}

void TOutlineListView :: TargetedByScrollView(BScrollView * aScroller) {

	BOutlineListView::TargetedByScrollView(aScroller);
	
	fScroller = aScroller;
	
}

// Haiku's version of GetPreferredSize works fine
#ifndef HAIKU

void TOutlineListView :: GetPreferredSize(float * aWidth, float * aHeight) {

	BOutlineListView::GetPreferredSize(aWidth, aHeight);

	// Correct preferred width of BOutlineListView as BeOS calculates it wrong.
	int32 count = CountItems();
	float width = 0;
	for (int32 i = 0; i < count; i++) {
		width = ItemAt(i)->Width() + (ItemAt(i)->OutlineLevel() + 1) * be_plain_font->Size();
		if (width > *aWidth)
			*aWidth = width;
	}

}

#endif
