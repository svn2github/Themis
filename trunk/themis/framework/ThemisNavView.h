/* ThemisNavView.h */

#ifndef _THEMISNAVVIEW_H_
#define _THEMISNAVVIEW_H_

// BeOS headers
#include <View.h>
#include <PictureButton.h>

// C/C++ headers

// myheaders
#include "ThemisUrlView.h"

class ThemisNavView : public BView
{
	public:
												ThemisNavView( BRect rect );
		virtual							~ThemisNavView( void );
		void								AttachedToWindow( void );
		void								Draw( BRect updaterect );
		
		void								CreateInterfaceButtons();
		
		ThemisUrlView*			urlview;
		
		BPictureButton*			buttons[7];
};

#endif
