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

class ThemisTab : public BTab
{
	public:
									ThemisTab( BView *view, int16 uid );
									
		virtual void				DrawTab(
										ThemisTabView* owner,
										BRect frame,
										tab_position position,
										bool full=true );
		virtual void				DrawLabel(
										ThemisTabView* owner,
										BRect frame );
										
		int16						UniqueID();
		
	private:
		int16						fUniqueID;
};

#endif
