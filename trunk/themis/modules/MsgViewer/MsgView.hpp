/*	MsgView window
	Does the actual displaying of the messages sent to the MsgView addon
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	23-08-2003
	
*/

#ifndef MSGVIEW_HPP
#define MSGVIEW_HPP

// Standard C++ headers
#include <string>
#include <map>
#include <vector>

// BeOS headers
#include <Window.h>

// Declarations of BeOS classes
class BTextView;
class BPopUpMenu;
class BMessage;

// Namespaces used
using namespace std;

// Constants used
const uint32 CHANGE_MESSAGE_VIEW = 'cmsv';

class MsgView	:	public BWindow	{

	private:
		BTextView * messageView;
		BPopUpMenu * senderMenu;
		map<string, vector<string> >  messageMap;
	
	public:
		MsgView( BMessage * pluginList = NULL );
		~MsgView();
		void MessageReceived( BMessage * message );
		bool QuitRequested();
		void addMessage( const char * sender, const char * message );
		void addPlugin( const char * plugin );
		
};

#endif
