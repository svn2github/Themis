/*
	Copyright (c) 2001 Mark Hellegers. All Rights Reserved.
	
	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or
	sell copies of the Software, and to permit persons to whom
	the Software is furnished to do so, subject to the following
	conditions:
	
	   The above copyright notice and this permission notice
	   shall be included in all copies or substantial portions
	   of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
	KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
	WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
	OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
	OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
	OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
	
	Original Author: 	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	Project Start Date: October 18, 2000
	Class Start Date: August 23, 2003
*/

/*
	MsgView implementation
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

MsgView	::	MsgView( BMessage * aPluginList )
				:	BWindow( BRect( 100, 100, 500, 200 ), "Message View", B_TITLED_WINDOW,
									B_CURRENT_WORKSPACE )	{

	// Create the BView.
	BRect backRect = Bounds();
	BView * backGround =
		new BView( backRect, "Background", B_FOLLOW_ALL_SIDES, B_WILL_DRAW );
	backGround->SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );
	AddChild( backGround );

	// Create the popup menu.
	BRect menuRect = backRect;
	menuRect.bottom = 20;
	menuRect.right = 200;
	mSenderMenu = new BPopUpMenu( "Message Sender" );

	// Create the BMenuField and add it to the view.
	BMenuField * senderField =
		new BMenuField( menuRect, "senderField", "Sender", mSenderMenu );
	senderField->SetDivider( 50 );
	backGround->AddChild( senderField );
	
	// Create the message view and add it to the view.
	BRect messageViewRect = backRect;
	messageViewRect.InsetBy( 20, 20 );
	messageViewRect.left -= 10;
	messageViewRect.top += 10;
	BRect messageRect = messageViewRect;
	messageRect.OffsetTo( 0, 0 );
	messageRect.InsetBy( 5, 5 );
	mMessageView = new BTextView( messageViewRect, "MessageView", messageRect,
													B_FOLLOW_ALL_SIDES, B_WILL_DRAW );
	BScrollView * scrollView =
		new BScrollView( "MessageScrollView", mMessageView, B_FOLLOW_ALL_SIDES, 0,
								 true, true );
	backGround->AddChild( scrollView );

	// Add the general messages option.
	mSenderMenu->AddItem( new BMenuItem( "General Messages",
										new BMessage( CHANGE_MESSAGE_VIEW ) ) );
	vector<string> generalMessageVector;
	mMessageMap.insert(
		map<string,
				vector<string> >::value_type( "General Message",
															 generalMessageVector ) );

	if ( aPluginList != NULL )	{
		// Add the list of plugins to the popup menu.
		int32 count = 0;
		type_code code = B_STRING_TYPE;
		aPluginList->GetInfo( "plug_name", &code, &count );
		const char * pluginName;
		for ( int32 i = 0; i < count; i++ )	{
			aPluginList->FindString( "plug_name", i, &pluginName );
			mSenderMenu->AddItem( new BMenuItem( pluginName,
								new BMessage( CHANGE_MESSAGE_VIEW ) ) );
			string pluginString( pluginName );
			vector<string> messageVector;
			mMessageMap.insert(
				map<string,
						vector<string> >::value_type( pluginString,
																	 messageVector ) );
		}
	}	

}

MsgView	::	~MsgView()	{
	
}

void MsgView	::	MessageReceived( BMessage * aMessage )	{

	switch( aMessage->what )	{
		case CHANGE_MESSAGE_VIEW:	{
			BMenuItem * marked = mSenderMenu->FindMarked();
			string label( marked->Label() );
			map<string, vector<string> >::iterator it = mMessageMap.find( label );
			if ( it != mMessageMap.end() )	{
				mMessageView->SetText( "\0" );
				for ( unsigned int i = 0; i < ( (*it).second ).size(); i++ )	{
					mMessageView->Insert( ( (*it).second )[ i ].c_str() );
				}
			}
			break;
		}
		default:	{
			BWindow::MessageReceived( aMessage );
		}
	}
	
}

bool MsgView	::	QuitRequested()	{
	
	return true;
	
}

void MsgView	::	addMessage( string aMessage, string aSender  )	{

	Lock();

	string senderString = "";
	if ( aSender == "" )	{
		senderString = "General Messages";
	}
	else	{
		senderString = aSender;
	}
	if ( mMessageMap.count( senderString ) != 0 )	{
		map<string, vector<string> >::iterator i = mMessageMap.find( senderString );
		if ( i != mMessageMap.end() )	{
			( (*i).second ).push_back( aMessage );
		}
	}

	BMenuItem * marked = mSenderMenu->FindMarked();
	if ( marked != NULL )	{
		string label( marked->Label() );
		if ( senderString == label )	{
			mMessageView->Insert( aMessage.c_str() );
		}
	}
	Unlock();
	
}

void MsgView	::	addPlugin( string aPlugin )	{

	Lock();	
	mSenderMenu->AddItem( new BMenuItem( aPlugin.c_str(),
						new BMessage( CHANGE_MESSAGE_VIEW ) ) );
	vector<string> messageVector;
	mMessageMap.insert(
		map<string, vector<string> >::value_type( aPlugin, messageVector ) );
	Unlock();
	
}
