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
									ThemisTab( BView* view );
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
										
		int32						GetViewID();
		
		void						SetViewID(
										int32 id );
		
	private:
		int32						fViewID;
		
		TabHistory*					fHistory;
};

#endif
