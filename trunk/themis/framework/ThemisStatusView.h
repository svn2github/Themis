/* ThemisStatusView.h */

#ifndef _THEMISSTATUSVIEW_H_
#define _THEMISSTATUSVIEW_H_

// BeOS headers
#include <View.h>
#include <StringView.h>

// C/C++ headers

// myheaders

class ThemisStatusView : public BView
{
	public:
												ThemisStatusView( BRect rect );
		virtual							~ThemisStatusView( void );
		void								AttachedToWindow( void );
		void								Draw( BRect updaterect );
		
		BStringView*				statustextview;
};

#endif
