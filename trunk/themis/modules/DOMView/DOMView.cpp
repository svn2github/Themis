/*	DOMView implementation
	See DOMView.h for some more information
*/

#include <stdio.h>

#include <String.h>
#include <Rect.h>
#include <ListItem.h>
#include <Box.h>
#include <MenuField.h>
#include <MenuItem.h>

#include "DOMView.h"
#include "commondefs.h"
#include "plugman.h"

#include "TDocument.h"
#include "TNode.h"
#include "TNodeList.h"
#include "TNamedNodeMap.h"

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
							BWindow( BRect( 100, 100, 450, 400 ), "DOMWindow", B_TITLED_WINDOW,
								B_CURRENT_WORKSPACE )	{

	BRect backRect = Bounds();
	BView * backGround = new BView( backRect, "Background", B_FOLLOW_ALL_SIDES,
					B_WILL_DRAW );

	backGround->SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );

	AddChild( backGround );

	BRect treeRect = backRect;
	BRect listRect = treeRect;
	treeRect.bottom -= B_H_SCROLL_BAR_HEIGHT;
	treeRect.right -= 200;
	treeRect.right -= B_V_SCROLL_BAR_WIDTH;

	tree = new BOutlineListView( treeRect, "DOMView", B_SINGLE_SELECTION_LIST,
			B_FOLLOW_ALL_SIDES );
	doc = document;
	showDocument();

	tree->SetSelectionMessage( new BMessage( SELECTION ) );

	scrollTree = new BScrollView( "Scroll Tree", tree,
			B_FOLLOW_TOP_BOTTOM | B_FOLLOW_LEFT,  0, true, true );

	scrollTree->SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );

	listRect.left += 155;
	
	BRect textRect = listRect;
	textRect.bottom -= 160;
	textRect.InsetBy( 5, 5 );

	BBox * textBox = new BBox( textRect, "TextBox", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP );

	textMenu = new BPopUpMenu( "No text" );
	textMenu->SetEnabled( false );
	
	BRect menuRect( 0, 0, 100, 50 );
	BMenuField * textSelect = new BMenuField( menuRect, "TextPopUp", NULL, textMenu );

	textBox->SetLabel( textSelect );
	
	textRect = textBox->Bounds();
	textRect.InsetBy( 10, 20 );
	textRect.top += 5;
	textRect.bottom += 10;
	textRect.bottom -= B_H_SCROLL_BAR_HEIGHT;
	textRect.right -= B_V_SCROLL_BAR_WIDTH;

	BRect textDisplay( 0, 0, textRect.Width(), textRect.Height() );

	text = new BTextView( textRect, "TextView", textDisplay, B_FOLLOW_ALL_SIDES, B_WILL_DRAW );

	BScrollView * scrollText = new BScrollView( "Scroll Text", text,
			B_FOLLOW_TOP | B_FOLLOW_LEFT_RIGHT,  B_FRAME_EVENTS, true, true );

	scrollText->SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );

	listRect.top += 135;
	listRect.InsetBy( 5, 5 );

	BBox * attrBox = new BBox( listRect, "AttributeBox", B_FOLLOW_ALL_SIDES );
	attrBox->SetLabel( "Attributes" );

	listRect = attrBox->Bounds();
	listRect.InsetBy( 10, 20 );
	listRect.bottom += 10;
	listRect.right -= 80;
	listRect.bottom -= B_H_SCROLL_BAR_HEIGHT;
	listRect.right -= B_V_SCROLL_BAR_WIDTH;
	
	attributes = new BListView( listRect, "AttrView", B_SINGLE_SELECTION_LIST,
			B_FOLLOW_ALL_SIDES );

	scrollAttr = new BScrollView( "Scroll Attr", attributes,
			B_FOLLOW_TOP_BOTTOM | B_FOLLOW_LEFT,  B_FRAME_EVENTS, true, true );

	scrollAttr->SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );

	BStringItem * attrItem = new BStringItem( "No attributes" );
	attributes->AddItem( attrItem );

	listRect.left += 100;
	listRect.right += 80;

	values = new BListView( listRect, "ValueView", B_SINGLE_SELECTION_LIST,
			B_FOLLOW_ALL_SIDES );

	scrollValue = new BScrollView( "Scroll Value", values,
			B_FOLLOW_ALL_SIDES,  B_FRAME_EVENTS, true, true );

	scrollValue->SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );

	BStringItem * valueItem = new BStringItem( "No value" );
	values->AddItem( valueItem );
	
	backGround->AddChild( scrollTree );
	backGround->AddChild( textBox );
	backGround->AddChild( attrBox );
	textBox->AddChild( scrollText );
	attrBox->AddChild( scrollAttr );
	attrBox->AddChild( scrollValue );
	Show();
	
}

DOMWindow	::	~DOMWindow()	{

	int32 items = tree->CountItems();
	for ( int32 i = 0; i < items; i++ )	{
		delete tree->RemoveItem( (int32) 0 );
	}
	
}

void DOMWindow	::	MessageReceived( BMessage * message )	{

	switch ( message->what )	{
		case SELECTION:	{
			int32 index = 0;
			message->FindInt32( "index", &index );
			int32 current = 0;
			TNodeShared found = findNode( doc, index, current );
			attributes->RemoveItems( 0, attributes->CountItems() );
			values->RemoveItems( 0, values->CountItems() );
			if ( found->hasAttributes() )	{
				TNamedNodeMapShared attrs = found->getAttributes();
				for ( unsigned int i = 0; i < attrs->getLength(); i++ )	{
					TNodeShared attr = make_shared( attrs->item( i ) );
					BStringItem * attrItem = new BStringItem( attr->getNodeName().c_str() );
					attributes->AddItem( attrItem );
					BStringItem * valueItem = new BStringItem( attr->getNodeValue().c_str() );
					values->AddItem( valueItem );
				}
			}
			else	{
				BStringItem * attrItem = new BStringItem( "No attributes" );
				attributes->AddItem( attrItem );
				BStringItem * valueItem = new BStringItem( "No values" );
				values->AddItem( valueItem );
			}
			
			int menuItems = textMenu->CountItems();
			int32 markedIndex = 0;
			if ( menuItems )	{
				BMenuItem * marked = textMenu->FindMarked();
				if ( marked )	{
					markedIndex = textMenu->IndexOf( marked );
				}
			}
			for ( int i = 0; i < menuItems; i++ )	{
				delete textMenu->RemoveItem( (int32) 0 );
				textMenu->SetEnabled( false );
				text->SetText( "" );
				textMenu->Invalidate();
			}
			
			int textNr = 1;
			if ( found->hasChildNodes() )	{
				TNodeListShared children = found->getChildNodes();
				selectedNode = found;
				for ( unsigned int i = 0; i < children->getLength(); i++ )	{
					TNodeShared child = make_shared( children->item( i ) );
					if ( child->getNodeType() == TEXT_NODE )	{
						textMenu->SetEnabled( true );
						BString * itemText = new BString( "Text part " );
						*itemText << textNr;
						BMessage * menuChange = new BMessage( TEXT_MENU_CHANGED );
						// Warning: Adding index of all children
						// Makes it easier to find
						menuChange->AddInt32( "index", i );
						BMenuItem * item = new BMenuItem( itemText->String(), menuChange );
						if ( (int32) i <= markedIndex )	{
							item->SetMarked( true );
							text->SetText( child->getNodeValue().c_str(), child->getNodeValue().size() );
						}
						textMenu->AddItem( item );
						textNr++;
					}
				}
			}
							
			break;
		}
		case TEXT_MENU_CHANGED:	{
			int32 index = 0;
			message->FindInt32( "index", &index );
			TNodeListShared children = selectedNode->getChildNodes();
			TNodeShared child = make_shared( children->item( index ) );
			text->SetText( child->getNodeValue().c_str(), child->getNodeValue().size() );
		
			break;	
		}
		default:	{
			//message->PrintToStream();
			break;
		}
	}

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

				showTree( child, childItem );
			}
		}
	}	
}

void DOMWindow	::	showDocument()	{
	
	int32 items = tree->CountItems();
	for ( int32 i = 0; i < items; i++ )	{
		delete tree->RemoveItem( (int32) 0 );
	}

	BStringItem * root = new BStringItem( doc->getNodeName().c_str() );
	tree->AddItem( root );
	showTree( doc, root );

}

void DOMWindow	::	setDocument( TDocumentShared document )	{

	doc = document;
	showDocument();
	
}

TNodeShared DOMWindow	::	findNode( TNodeShared node, int32 target, int32 & current )	{
	
	if ( node.get() && ( node->getNodeType() == ELEMENT_NODE ||
								node->getNodeType() == DOCUMENT_NODE ) )	{
		
		if ( target == current )	{
			// This is the node we're looking for
			return node;
		}
		
		// Get the next node
		// I'm using getChildNodes instead of getFirstChild,
		// because of a weird crash with getFirstChild. Not sure who is wrong here
		TNodeListShared children = node->getChildNodes();
		TNodeShared result = TNodeShared();
		TNodeShared next = TNodeShared();
		if ( children->getLength() )	{
			current++;
			TNodeShared next = make_shared( children->item( 0 ) );
			result = findNode( next, target, current );
		}
		if ( result.get() )	{
			return result;
		}
		else	{
			current++;
			next = make_shared( node->getNextSibling() );
			return findNode( next, target, current );
		}							
	}
	else	{
		// Was not an element or a document node or didn't exist
		if ( node.get() )	{
			TNodeShared next = make_shared( node->getNextSibling() );
			return findNode( next, target, current );
		}
		else	{
			// Node doesn't exist
			current--;
		}
	}
	
	return TNodeShared();
	
}

DOMView	::	DOMView( BMessage * info )	:	BHandler( "DOMView" ), PlugClass( info )	{
	
	window = NULL;
	
}

DOMView	::	~DOMView()	{
	
	window->Lock();
	delete window;
	
}

void DOMView	::	MessageReceived( BMessage * message )	{
	
	//message->PrintToStream();
	
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
				TDocumentShared * temp = (TDocumentShared *) document;
				TDocumentShared copy = *temp;
				if ( !window )	{
					window = new DOMWindow( copy );
				}
				else	{
					window->Lock();
					window->setDocument( copy );
					window->Unlock();
				}
			}
			break;
		}
		default:	{
			message->PrintToStream();
			return PLUG_DOESNT_HANDLE;
		}
	}
	
	return PLUG_HANDLE_GOOD;
	
}

int32 DOMView	::	Type()	{
	
	return TARGET_DOM;
	
}
