/*	DOMView implementation
	See DOMView.h for some more information
*/

#include <stdio.h>

#include <String.h>
#include <Rect.h>
#include <ListItem.h>

#include "DOMView.h"
#include "commondefs.h"
#include "plugman.h"

#include "TDocument.h"
#include "TNode.h"
#include "TNodeList.h"

DOMView * viewer;
BMessage ** appSettings_p;
BMessage * appSettings;

status_t Initialize( void * info )	{
	
	viewer = NULL;
	if ( info != NULL )	{
		BMessage * message = (BMessage *) info;
		if ( message->HasPointer( "settings_message_ptr" ) )	{
			message->FindPointer( "settings_message_ptr", (void **) & appSettings_p );
			appSettings = *appSettings_p;
		}
		viewer = new DOMView( message );
	}
	else	{
		viewer = new DOMView();
	}
	
	return B_OK;
	
}

status_t Shutdown( bool now )	{
	
	delete viewer;
	
	return B_OK;
	
}

PlugClass * GetObject()	{
	
	return viewer;
	
}

DOMWindow	::	DOMWindow( TDocumentShared document )	:
							BWindow( BRect( 100, 100, 200, 200 ), "DOMWindow", B_TITLED_WINDOW,
								B_CURRENT_WORKSPACE )	{

	BRect area = Bounds();
	area.right -= B_V_SCROLL_BAR_WIDTH;

	tree = new BOutlineListView( area, "DOMView", B_SINGLE_SELECTION_LIST,
			B_FOLLOW_ALL_SIDES );
	showDocument( document );

	scrollTree = new BScrollView( "Scroll Tree", tree,
			B_FOLLOW_ALL_SIDES,  0, false, true );

	AddChild( scrollTree );
	Show();

}

DOMWindow	::	~DOMWindow()	{
	
}

bool DOMWindow	::	QuitRequested()	{
	
	return true;
	
}

void DOMWindow	::	showTree( const TNodeShared aNode, BStringItem * parent )	{
	
	TNodeListShared children = aNode->getChildNodes();
	int length = children->getLength();
	if ( length != 0 )	{
		for ( int i = length - 1; i >= 0; i-- )	{
			TNodeShared child = make_shared( children->item( i ) );
	
			if ( child->getNodeType() == ELEMENT_NODE )	{
				BStringItem * childItem = new BStringItem( child->getNodeName().c_str() );
				tree->AddUnder( childItem, parent );

				/*
				// Check for attributes
				TNamedNodeMapShared attributes = child->getAttributes();
				for ( unsigned int j = 0; j < attributes->getLength(); j++ )	{
					TNodeWeak attr = attributes->item( j );
					TNodeShared tempAttr = make_shared( attr );
					for ( int j = 0; j < aSpacing + 1; j++ )	{
						cout << "  ";
					}
					cout << "Attribute " << tempAttr->getNodeName();
					cout << " with value " << tempAttr->getNodeValue() << endl;
				}
				*/

				showTree( child, childItem );
			}
		}
	}	
}

void DOMWindow	::	showDocument( TDocumentShared document )	{
	
	BStringItem * root = new BStringItem( document->getNodeName().c_str() );
	tree->AddItem( root );
	showTree( document, root );

}


DOMView	::	DOMView( BMessage * info )	:	BHandler( "DOMView" ), PlugClass( info )	{
	
}

DOMView	::	~DOMView()	{
	
	delete window;
	
}

void DOMView	::	MessageReceived( BMessage * message )	{
	
	message->PrintToStream();
	
}

bool DOMView	::	IsHandler()	{
	
	return true;
	
}

BHandler * DOMView	::	Handler()	{
	
	return this;
	
}

bool DOMView	::	IsPersistent()	{
	
	return true;
	
}

uint32 DOMView	::	PlugID()	{
	
	return 'tree';
	
}

char * DOMView	::	PlugName()	{
	
	return "DOM Viewer";
	
}

float DOMView	::	PlugVersion()	{
	
	return 0.0;
	
}

void DOMView	::	Heartbeat()	{
	
}

status_t DOMView	::	ReceiveBroadcast( BMessage * message )	{
	
	printf( "DOMView is receiving broadcast:\n" );
	
	int32 command = 0;
	message->FindInt32( "command", &command );
	
	switch ( command )	{
		case COMMAND_INFO:	{
			printf( "COMMAND_INFO called\n" );
			// Check if it is dom data
			BString type;
			message->FindString( "type", &type );
			if ( type != "dom" )	{
				printf( "Message not recognized\n" );
				break;
			}
			
			// Get the pointer out
			void * document = NULL;
			message->FindPointer( "data_pointer", &document );
			if ( document )	{
				printf( "Got pointer\n" );
				TDocumentShared * temp = (TDocumentShared *) document;
				TDocumentShared copy = *temp;
				printf( "Use count of document: %i\n", (int) copy.use_count() );
				window = new DOMWindow( copy );
			}
			break;
		}
		default:	{
			message->PrintToStream();
		}
	}
	
	return B_OK;
	
}

int32 DOMView	::	Type()	{
	
	return TARGET_DOM;
	
}
