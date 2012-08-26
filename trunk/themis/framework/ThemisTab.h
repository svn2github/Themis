/* ThemisTab.h */

#ifndef _THEMISTAB_H_
#define _THEMISTAB_H_

// BeOS headers
#include <TabView.h>

// Declarations of BeOS classes
class BBitmap;
class BString;
class BView;

// Declarations of Themis classes
class ThemisTabView;
class TabHistory;

class ThemisTab : public BTab {

	private:
		int32 fSiteID;
		TabHistory * fHistory;
		BString * fLabel;
		BBitmap * fFavIcon;

	public:
		ThemisTab(BView* view);
		~ThemisTab();
									
		virtual void DrawTab(ThemisTabView* owner,
							 BRect frame,
							 tab_position position,
							 bool full = true);
		virtual void DrawLabel(ThemisTabView* owner,
							   BRect frame);
		BBitmap * GetFavIcon();
		TabHistory * GetHistory();
		int32 GetSiteID();
		void SetFavIcon(BBitmap* bmp);
		void SetLabel(const char* label);
		void SetSiteID(int32 id);
		void AddHistoryEntry(const char * aEntry);
		
};

#endif
