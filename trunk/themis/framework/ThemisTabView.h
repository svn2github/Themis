/* ThemisTabView.h */

#ifndef _THEMISTABVIEW_H_
#define _THEMISTABVIEW_H_

// BeOS headers
#include <TabView.h>
#include <PictureButton.h>

// C/C++ headers

// myheaders

class ThemisTabView : public BTabView
{
	public:
									ThemisTabView(
										BRect frame,
										const char *name,
										button_width width,
										uint32 resizingmode,
										uint32 flags,
										const rgb_color* arr );

		virtual void				AttachedToWindow();
		virtual void				Draw( BRect updaterect );
		virtual void				DrawBox( BRect selTabRect );
		virtual BRect				DrawTabs( void );
		virtual void				MakeFocus( bool focus );
		virtual void				MouseDown( BPoint point );
		virtual void				Select( int32 tabindex );
		virtual void				SetFocusTab( int32 tab, bool focused );
		
		void						CreateCloseTabViewButton();
		void						DynamicTabs( bool newtab );
		void						SetFakeSingleView();
		void						SetNormalTabView();
				
		float						tab_width;
		bool						fake_single_view;
		
		BPictureButton*				close_tabview_button;
		
		// for doubleclick detection on empty tabview-space
		// ( doubleclick opens new tab )
		uint32						lastbutton;
		
		rgb_color					fBackgroundColor;
		rgb_color					fInactiveTabColor;
		rgb_color					fBlackColor;
		rgb_color					fWhiteColor;
};

#endif
