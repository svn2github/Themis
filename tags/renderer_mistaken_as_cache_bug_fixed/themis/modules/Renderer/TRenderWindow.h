#include <Window.h>

#include "TRenderView.h"

class TRenderWindow : public BWindow
{
	public:
							TRenderWindow(TRenderView *view);
		virtual				~TRenderWindow();
	
		virtual	void		MessageReceived(BMessage *message);
		virtual	bool		QuitRequested();
				
	private:
				BMessenger 	*panel;
				TRenderView *view;

};
