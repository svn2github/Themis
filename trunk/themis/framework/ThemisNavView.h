/* ThemisNavView.h */

#ifndef _THEMISNAVVIEW_H_
#define _THEMISNAVVIEW_H_

// BeOS headers
#include <View.h>
#include <PictureButton.h>

// C/C++ headers

// myheaders
#include "ThemisUrlView.h"

class ThemisPictureButton;

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
		
		ThemisPictureButton*				buttons[7];
};

#endif

/////////////////////////////////////
// ThemisPictureButton
/////////////////////////////////////

#ifndef THEMISPICTUREBUTTON
#define THEMISPICTUREBUTTON

class ThemisPictureButton : public BPictureButton
{
	public:
										ThemisPictureButton(
											BRect rect,
											const char* name,
											BPicture* off,
											BPicture* on,
											BMessage* message,
											uint32 behaviour,
											uint32 resizingmode,
											uint32 flags );
		virtual void					Draw( BRect updaterect );
		virtual void					MouseDown( BPoint point );
		virtual void					MouseMoved( BPoint point, uint32 transit, const BMessage* message );
		virtual void					MouseUp( BPoint point );
	
	private:
		bool							i_was_disabled_on_flag;
};

#endif
