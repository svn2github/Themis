/* ThemisTab.h */

#ifndef _THEMISTAB_H_
#define _THEMISTAB_H_

// BeOS headers
#include <TabView.h>
#include <View.h>

// C/C++ headers

// myheaders
#include "FakeSite.h"
#include "ThemisTabView.h"
#include "TabHistory.h"

class ThemisTab : public BTab
{
	public:
									ThemisTab( BView *view, int16 uid );
									~ThemisTab();
									
		virtual void				DrawTab(
										ThemisTabView* owner,
										BRect frame,
										tab_position position,
										bool full=true );
		virtual void				DrawLabel(
										ThemisTabView* owner,
										BRect frame );
		
		TabHistory*					GetHistory();
										
		int16						UniqueID();
		
	private:
		int16						fUniqueID;
		
		TabHistory*					fHistory;
};

#endif
