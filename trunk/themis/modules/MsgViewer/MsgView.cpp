/*	MsgView implementation
	See MsgView.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// BeOS headers
#include <View.h>
#include <TextView.h>
#include <ScrollView.h>
#include <PopUpMenu.h>
#include <MenuField.h>
#include <MenuItem.h>

// MsgViewer headers
#include "MsgView.hpp"

MsgView	::	MsgView()
				:	BWindow( BRect( 100, 100, 500, 200 ), "Message View", B_TITLED_WINDOW,
									B_CURRENT_WORKSPACE )	{

	printf( "Constructing MsgView\n" );
	
	BRect backRect = Bounds();
	BView * backGround =
		new BView( backRect, "Background", B_FOLLOW_ALL_SIDES, B_WILL_DRAW );
	backGround->SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );
	AddChild( backGround );

	BRect menuRect = backRect;
	menuRect.bottom = 20;
	menuRect.right = 200;
	senderMenu = new BPopUpMenu( "Message Sender" );

	BMenuField * senderField =
		new BMenuField( menuRect, "senderField", "Sender", senderMenu );
	senderField->SetDivider( 50 );
	backGround->AddChild( senderField );
	
	BRect messageViewRect = backRect;
	messageViewRect.InsetBy( 20, 20 );
	messageViewRect.left -= 10;
	messageViewRect.top += 10;
	BRect messageRect = messageViewRect;
	messageRect.OffsetTo( 0, 0 );
	messageRect.InsetBy( 5, 5 );
	messageView = new BTextView( messageViewRect, "MessageView", messageRect,
													B_FOLLOW_ALL_SIDES, B_WILL_DRAW );
	BScrollView * scrollView =
		new BScrollView( "MessageScrollView", messageView, B_FOLLOW_ALL_SIDES, 0,
								 true, true );
	backGround->AddChild( scrollView );

}

MsgView	::	~MsgView()	{
	
	printf( "Destroying MsgView\n" );
	
}

void MsgView	::	MessageReceived( BMessage * message )	{

	switch( message->what )	{
		case CHANGE_MESSAGE_VIEW:	{
			BMenuItem * marked = senderMenu->FindMarked();
			string label( marked->Label() );
			map<string, vector<string> >::iterator it = messageMap.find( label );
			if ( it != messageMap.end() )	{
				messageView->SetText( "\0" );
				for ( unsigned int i = 0; i < ( (*it).second ).size(); i++ )	{
					messageView->Insert( ( (*it).second )[ i ].c_str() );
				}
			}
			break;
		}
		default:	{
			BWindow::MessageReceived( message );
		}
	}
	
}

bool MsgView	::	QuitRequested()	{
	
	return true;
	
}

void MsgView	::	addMessage( const char * sender, const char * message )	{

	string messageString = string( message );
	messageString += '\n';

	string senderString = string( sender );
	if ( messageMap.count( senderString ) == 0 )	{
		vector<string> messageVector;
		messageVector.push_back( messageString );
		messageMap.insert(
			map<string, vector<string> >::value_type( senderString, messageVector ) );
		Lock();
		senderMenu->AddItem( new BMenuItem( sender,
																	 new BMessage( CHANGE_MESSAGE_VIEW ) ) );
		Unlock();
	}
	else	{
		map<string, vector<string> >::iterator i = messageMap.find( senderString );
		if ( i != messageMap.end() )	{
			( (*i).second ).push_back( messageString );
		}
	}

	Lock();
	BMenuItem * marked = senderMenu->FindMarked();
	if ( marked != NULL )	{
		string label( marked->Label() );
		if ( senderString == label )	{
			messageView->Insert( messageString.c_str() );
		}
	}
	Unlock();
	
}
