/* ThemisTab.h */

#ifndef _THEMISTAB_H_
#define _THEMISTAB_H_

// BeOS headers
#include <TabView.h>
#include <View.h>

// C/C++ headers

// myheaders
#include "FakeSite.h"

class ThemisTab : public BTab
{
	public:
												ThemisTab( BView* view );
		
		virtual void				DrawTab( BView* owner, BRect frame,
													tab_position position, bool full=true );
		virtual void				DrawLabel( BView* owner, BRect frame );
};

#endif
