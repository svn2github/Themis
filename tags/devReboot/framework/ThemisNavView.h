/* ThemisNavView.h */

#ifndef _THEMISNAVVIEW_H_
#define _THEMISNAVVIEW_H_

// BeOS headers
#include <View.h>

// C/C++ headers

// myheaders
#include "ThemisUrlView.h"
#include "TPictureButton.h"

class ThemisNavView : public BView
{
	public:
											ThemisNavView(
												BRect rect );
		virtual								~ThemisNavView( void );
		void								AttachedToWindow( void );
		void								Draw( BRect updaterect );
		void								MouseDown( BPoint point );
		
		void								CreateInterfaceButtons();
		
		ThemisUrlView*						urlview;
		
		TPictureButton*						buttons[7];
};

#endif
