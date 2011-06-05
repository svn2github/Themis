/* ThemisNavView.h */

#ifndef _THEMISNAVVIEW_H_
#define _THEMISNAVVIEW_H_

// BeOS headers
#include <View.h>

// Declarations used
class ThemisUrlView;
class TPictureButton;

class ThemisNavView : public BView {

	private:
		TPictureButton * buttons[7];

	public:
		ThemisNavView(BRect rect);
		virtual ~ThemisNavView();
		void AttachedToWindow();
		void Draw(BRect updaterect);
		void MouseDown(BPoint point);
		void CreateInterfaceButtons();
		void SetButtonMode(int aButton, int8 aMode, bool aForceMode = false);

		ThemisUrlView * urlview;
};

#endif
