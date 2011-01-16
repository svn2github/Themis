/*
 * PrefsWin.h
 *
 * author: Michael Weirauch
 */

#ifndef _NEWPREFSWIN_H_
#define _NEWPREFSWIN_H_

#include <View.h>
#include <Window.h>

// C++ headers
#include <vector>

// Namespaces used
using std::vector;

// Declarations
class PrefsListView;

class PrefsWin : public BWindow  {

	private:
		BView * fBackgroundView;
		PrefsListView * fPrefsListView;
		BView * fCurrentPrefsView;
		int32 fLastSelection;
		BRect fViewFrame;
		vector<BView *> mPrefViews;
		
		bool GetCheckBoxValue(BMessage * aMessage);
		void CreatePrefViews(BMessage * aPluginList);
		void SaveAppSettings();

	public:
		PrefsWin(BRect aFrame, BMessage * aPluginList);
		virtual ~PrefsWin();
		virtual void MessageReceived(BMessage* msg);
		virtual bool QuitRequested();	
		
};

#endif

/*
 * PrefsListView
 */

#ifndef PREFSLISTVIEW
#define PREFSLISTVIEW

#include <Box.h>
#include <ListView.h>


class PrefsListView : public BView {

	private:
		BListView * fListView;
		BBox * fBox;
		BMessage * mPluginList;

	public:
		PrefsListView(BRect aFrame, BMessage * aPluginList);
		virtual ~PrefsListView();
		virtual void AttachedToWindow();
		int32 CurrentSelection();
		void Select(int32 which);
										
};

#endif


/*
 * PrefsListItem
 */

#ifndef PREFSLISTITEM
#define PREFSLISTITEM

// BeOS headers
#include <Bitmap.h>
#include <ListItem.h>

class PrefsListItem : public BListItem {

	private:
		const char * fName;
		BBitmap * fBitmap;
		float fFontHeight;
		
	public:
		PrefsListItem(const char* name,
					  const uint8* bitmapdata);
		~PrefsListItem();
		virtual void DrawItem(BView * owner,
							  BRect itemRect,
							  bool drawEverything = false);
		virtual void Update(BView* owner,
							const BFont* font);
										
};

#endif
