/*	DOMView addon
	Shows the DOM Tree in a window
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	21-09-2002
	
*/

#ifndef DOMVIEW_H
#define DOMVIEW_H

// Standard C++ headers
#include <string>

// BeOS headers
#include <SupportDefs.h>
#include <Handler.h>
#include <Window.h>
#include <OutlineListView.h>
#include <ListView.h>
#include <ScrollView.h>
#include <PopUpMenu.h>
#include <TextView.h>

// Themis headers
#include "plugclass.h"

// DOM headers
#include "DOMSupport.h"

// Declarations of BeOS classes
class BStringItem;

// Namespaces used
using namespace std;

#define SELECTION 'slct'
#define TEXT_MENU_CHANGED 'tmcd'

extern "C" __declspec( dllexport ) status_t Initialize( void * info = NULL );
extern "C" __declspec( dllexport ) status_t Shutdown( bool now = false );
extern "C" __declspec( dllexport ) PlugClass * GetObject();

class DOMWindow	:	public BWindow	{

	private:
		BOutlineListView * tree;
		BScrollView * scrollTree;
		BListView * attributes;
		BScrollView * scrollAttr;
		BListView * values;
		BScrollView * scrollValue;
		BPopUpMenu * textMenu;
		BTextView * text;

		BList * items;

		TDocumentPtr doc;
		TNodePtr selectedNode;
	
	public:
		DOMWindow( TDocumentPtr document );
		~DOMWindow();
		void MessageReceived( BMessage * message );
		bool QuitRequested();
		void showTree( const TNodePtr aNode, BStringItem * parent );
		void showDocument();
		void setDocument( TDocumentPtr document );
		TNodePtr findNode( TNodePtr node, int32 target, int32 & current );
		
	
};

class DOMView	:	public BHandler, public PlugClass	{
	
	private:
		DOMWindow * window;
	
	public:
		DOMView( BMessage * info = NULL );
		~DOMView();
		void MessageReceived( BMessage * message );
		bool IsHandler();
		BHandler * Handler();
		bool IsPersistent();
		uint32 PlugID();
		char * PlugName();
		float PlugVersion();
		void Heartbeat();
		status_t ReceiveBroadcast( BMessage * message );
		int32 Type();
		uint32 BroadcastTarget();
		status_t BroadcastReply(BMessage *msg);
		
};

#endif
