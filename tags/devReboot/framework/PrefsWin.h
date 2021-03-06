/*
 * PrefsWin.h
 *
 * author: Michael Weirauch
 */

#ifndef _NEWPREFSWIN_H_
#define _NEWPREFSWIN_H_

#include <View.h>
#include <Window.h>

class PrefsListView;


class PrefsWin : public BWindow
{
	public:
									PrefsWin(
										BRect frame );
		
		virtual void				MessageReceived(
										BMessage* msg );
		
		virtual bool				QuitRequested();	
		
	private:
		void						SaveAppSettings();
		
		BView*						fBackgroundView;
		
		PrefsListView*				fPrefsListView;
		
		BView*						fCurrentPrefsView;
		
		int32						fLastSelection;
		
		BRect						fViewFrame;
};

#endif

/*
 * PrefsListView
 */

#ifndef PREFSLISTVIEW
#define PREFSLISTVIEW

#include <Box.h>
#include <ListView.h>


class PrefsListView : public BView
{
	public:
									PrefsListView(
										BRect frame );
		
		virtual void				AttachedToWindow();
		
		int32						CurrentSelection();
		
		void						Select(
										int32 which );
										
	private:
		BListView*					fListView;
		BBox*						fBox;
};

#endif


/*
 * PrefsListItem
 */

#ifndef PREFSLISTITEM
#define PREFSLISTITEM

#include <Bitmap.h>
#include <ListItem.h>

class PrefsListItem : public BListItem
{
	public:
									PrefsListItem(
										const char* name,
										const uint8* bitmapdata );
		
									~PrefsListItem();
		
		virtual void				DrawItem(
										BView* owner,
										BRect itemRect,
										bool drawEverything = false );
		
		virtual void				Update(
										BView* owner,
										const BFont* font );
										
	private:
		const char*					fName;
		BBitmap*					fBitmap;
		
		float						fFontHeight;		
};

#endif
