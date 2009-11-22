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
	
	Original Author: 	Mark Hellegers (mark@firedisk.net)
	Project Start Date: October 18, 2000
	Class Start Date: September 21, 2002
*/
/*	DOMView implementation
	See DOMView.h for some more information
*/

// Standard C headers
#include <stdio.h>

// BeOS headers
#include <String.h>
#include <Rect.h>
#include <ListItem.h>
#include <Box.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <ScrollView.h>

// DOM headers
#include "TDocument.h"
#include "TNode.h"
#include "TNodeList.h"
#include "TNamedNodeMap.h"
#include "TElement.h"

// DOMView headers
#include "DOMView.h"
#include "DOMTextView.hpp"

DOMView	::	DOMView( TDocumentPtr aDocument )
				:	BWindow( BRect( 100, 100, 450, 400 ), "DOMWindow",
								   B_TITLED_WINDOW,
								   B_CURRENT_WORKSPACE)	{

	// Set background and add it to the window.
	BRect backRect = Bounds();
	BView * backGround = new BView( backRect, "Background", B_FOLLOW_ALL_SIDES,
					B_WILL_DRAW );
	backGround->SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );
	AddChild( backGround );

	// Create tree view. 
	BRect treeRect = backRect;
	treeRect.bottom -= B_H_SCROLL_BAR_HEIGHT;
	treeRect.right -= 200;
	treeRect.right -= B_V_SCROLL_BAR_WIDTH;
	mTree = new BOutlineListView(treeRect,
								 "DOMView",
								 B_SINGLE_SELECTION_LIST,
								 B_FOLLOW_ALL_SIDES );

	mTree->SetSelectionMessage( new BMessage( SELECTION ) );
	BScrollView * scrollTree
		= new BScrollView( "Scroll Tree",
									 mTree,
									 B_FOLLOW_TOP_BOTTOM | B_FOLLOW_LEFT,
									  0, true, true );
	scrollTree->SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );

	// Create text box.
	BRect textRect = backRect;
	textRect.left += 155;
	textRect.bottom -= 160;
	textRect.InsetBy( 5, 5 );
	BBox * textBox = new BBox( textRect, "TextBox", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP );

	// Create text popup menu.
	mTextMenu = new BPopUpMenu( "No text" );
	mTextMenu->SetEnabled( false );
	BRect menuRect( 0, 0, 100, 50 );
	BMenuField * textSelect =
		new BMenuField( menuRect, "TextPopUp", NULL, mTextMenu );

	textBox->SetLabel( textSelect );
	
	textRect = textBox->Bounds();
	textRect.InsetBy( 10, 20 );
	textRect.top += 5;
	textRect.bottom += 10;
	textRect.bottom -= B_H_SCROLL_BAR_HEIGHT;
	textRect.right -= B_V_SCROLL_BAR_WIDTH;

	// Create text view.
	BRect textDisplay( 0, 0, textRect.Width(), textRect.Height() );
	mText = new DOMTextView(textRect, "TextView",
							textDisplay,
							B_FOLLOW_ALL_SIDES);
	BScrollView * scrollText =
		new BScrollView( "Scroll Text",
								  mText,
								  B_FOLLOW_TOP | B_FOLLOW_LEFT_RIGHT,
								  B_FRAME_EVENTS,
								  true, true );
	scrollText->SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );

	BRect listRect = backRect;
	listRect.left += 155;
	listRect.top += 135;
	listRect.InsetBy( 5, 5 );

	// Create attribute box.
	BBox * attrBox = new BBox( listRect, "AttributeBox", B_FOLLOW_ALL_SIDES );
	attrBox->SetLabel( "Attributes" );

	listRect = attrBox->Bounds();
	listRect.InsetBy( 10, 20 );
	listRect.bottom += 10;
	listRect.right -= 80;
	listRect.bottom -= B_H_SCROLL_BAR_HEIGHT;
	listRect.right -= B_V_SCROLL_BAR_WIDTH;
	
	// Create attribute list view.
	mAttributes =
		new BListView( listRect,
							   "AttrView",
							   B_SINGLE_SELECTION_LIST,
							   B_FOLLOW_ALL_SIDES );
	BScrollView * scrollAttr =
		new BScrollView( "Scroll Attr", mAttributes,
			B_FOLLOW_TOP_BOTTOM | B_FOLLOW_LEFT,  B_FRAME_EVENTS, true, true );
	scrollAttr->SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );
	BStringItem * attrItem = new BStringItem( "No attributes" );
	mAttributes->AddItem( attrItem );

	listRect.left += 100;
	listRect.right += 80;

	// Create value view.
	mValues =
		new BListView( listRect,
							   "ValueView",
							   B_SINGLE_SELECTION_LIST,
							   B_FOLLOW_ALL_SIDES );
	BScrollView * scrollValue =
		new BScrollView( "Scroll Value",
								  mValues,
								  B_FOLLOW_ALL_SIDES,
								  B_FRAME_EVENTS,
								  true, true );
	scrollValue->SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );
	BStringItem * valueItem = new BStringItem( "No value" );
	mValues->AddItem( valueItem );
	
	// Add children to background view.
	backGround->AddChild( scrollTree );
	backGround->AddChild( textBox );
	backGround->AddChild( attrBox );
	textBox->AddChild( scrollText );
	attrBox->AddChild( scrollAttr );
	attrBox->AddChild( scrollValue );

	// Setup the document.
	setDocument( aDocument );

	// Show the window.
	Show();

}

DOMView	::	~DOMView()	{

	int32 items = mTree->CountItems();
	for ( int32 i = 0; i < items; i++ )	{
		delete mTree->RemoveItem( (int32) 0 );
	}
	
}

void DOMView :: EmptyListView(BListView * aListView) {

	for (int32 i = aListView->CountItems() - 1; i >= 0 ; i--) {
		delete aListView->RemoveItem(i);
	}
}

void DOMView	::	MessageReceived( BMessage * aMessage )	{

	switch ( aMessage->what )	{
		case SELECTION:	{
			int32 index = 0;
			aMessage->FindInt32( "index", &index );
			if (index > -1) {
				int32 current = 0;
				TNodePtr documentRoot = mDocument->getFirstChild();
				BListItem * firstItem = mTree->FirstItem();
				TNodePtr found = findNode(documentRoot, firstItem, index, current);
				EmptyListView(mAttributes);
				EmptyListView(mValues);
				if ( found->hasAttributes() )	{
					TNamedNodeMapPtr attrs = found->getAttributes();
					for ( unsigned int i = 0; i < attrs->getLength(); i++ )	{
						TNodePtr attr = attrs->item( i );
						BStringItem * attrItem =
							new BStringItem( attr->getNodeName().c_str() );
						mAttributes->AddItem( attrItem );
						BStringItem * valueItem =
							new BStringItem( attr->getNodeValue().c_str() );
						mValues->AddItem( valueItem );
					}
				}
				else	{
					BStringItem * attrItem =
						new BStringItem( "No attributes" );
					mAttributes->AddItem( attrItem );
					BStringItem * valueItem =
						new BStringItem( "No values" );
					mValues->AddItem( valueItem );
				}
				
				int menuItems = mTextMenu->CountItems();
				int32 markedIndex = 0;
				if ( menuItems )	{
					BMenuItem * marked = mTextMenu->FindMarked();
					if ( marked )	{
						markedIndex = mTextMenu->IndexOf( marked );
					}
				}
				for ( int i = 0; i < menuItems; i++ )	{
					delete mTextMenu->RemoveItem( (int32) 0 );
					mTextMenu->SetEnabled( false );
					mText->SetText( "" );
					mTextMenu->Invalidate();
				}
				
				int textNr = 1;
				if ( found->hasChildNodes() )	{
					TNodeListPtr children = found->getChildNodes();
					mSelectedNode = found;
					for ( unsigned int i = 0; i < children->getLength(); i++ )	{
						TNodePtr child = children->item( i );
						if ( child->getNodeType() == TEXT_NODE )	{
							mTextMenu->SetEnabled( true );
							BString * itemText = new BString( "Text part " );
							*itemText << textNr;
							BMessage * menuChange =
								new BMessage( TEXT_MENU_CHANGED );
							// Warning: Adding index of all children
							// Makes it easier to find
							menuChange->AddInt32( "index", i );
							BMenuItem * item =
								new BMenuItem( itemText->String(), menuChange );
							if ( (int32) i <= markedIndex )	{
								item->SetMarked( true );
								mText->SetText( child->getNodeValue().c_str(),
													   child->getNodeValue().size() );
							}
							mTextMenu->AddItem( item );
							textNr++;
						}
					}
				}
			}
			else {
				mAttributes->MakeEmpty();
				mValues->MakeEmpty();
				int menuItems = mTextMenu->CountItems();
				for ( int i = 0; i < menuItems; i++ )	{
					delete mTextMenu->RemoveItem( (int32) 0 );
					mTextMenu->SetEnabled( false );
					mText->SetText( "" );
					mTextMenu->Invalidate();
				}
			}
			break;
		}
		case TEXT_MENU_CHANGED:	{
			int32 index = 0;
			aMessage->FindInt32( "index", &index );
			TNodeListPtr children = mSelectedNode->getChildNodes();
			TNodePtr child = children->item( index );
			mText->SetText( child->getNodeValue().c_str(),
									  child->getNodeValue().size() );
			break;
		}
	}

}

bool DOMView	::	QuitRequested()	{
	
	return true;
	
}

void DOMView	::	showTree( const TNodePtr aNode, BStringItem * aParent )	{
	
	TNodeListPtr children = aNode->getChildNodes();
	int length = children->getLength();
	if (length != 0) {
		for (int i = length - 1; i >= 0; i--) {
			TNodePtr child = children->item(i);

			if (child->getNodeType() == ELEMENT_NODE) {
				BStringItem * childItem =
					new BStringItem(child->getNodeName().c_str(), 0, false);
				if (aParent == NULL)
					mTree->AddItem(childItem);
				else
					mTree->AddUnder(childItem, aParent);
				showTree(child, childItem);
			}
		}
	}	
}

void DOMView	::	showDocument()	{
	
	BListItem * item = mTree->FirstItem();
	delete item;
	mTree->MakeEmpty();

	SetTitle(mDocument->getDocumentURI().c_str());
	showTree(mDocument, NULL);

}

void DOMView	::	setDocument( TDocumentPtr aDocument )	{

	mDocument = aDocument;
	showDocument();
	
}

TNodePtr DOMView :: findNode(TNodePtr aNode,
							 BListItem * aItem,
							 int32 aTarget,
							 int32 & aCurrent) {

	TNodePtr result;
	
	if (aTarget == aCurrent) {
		// This is the node we're looking for
		result = aNode;
	}
	else {
		// Only look at the children if the current item is expanded.
		// Collapsed items are not included in the index.
		if (aItem->IsExpanded()) {
			TNodeListPtr children = aNode->getChildNodes();
			unsigned int i = 0;
			unsigned int j = 0;
			unsigned int length = children->getLength();
			bool foundNode = false;
			while (i < length && !foundNode) {
				TNodePtr child = children->item(i);
				// Only look at the child if it is an element.
				// Others will throw of the current count of the outline listview.
				if (child->getNodeType() == ELEMENT_NODE) {
					aCurrent++;
					BListItem * childItem = mTree->ItemUnderAt(aItem, true, j);
					result = findNode(child, childItem, aTarget, aCurrent);
					if (result.get()) {
						// Found it. Stop the loop and return it.
						foundNode = true;
					}
					j++;
				}
				i++;
			}
		}
	}

	return result;
	
}
