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
	Class Start Date: September 21, 2002
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
#include <PopUpMenu.h>

// DOM headers
#include "DOMSupport.h"

// Declarations of BeOS classes
class BStringItem;

// Declarations of DOMView classes
class DOMTextView;

// Namespaces used
using namespace std;

// Constants used
const int32 SELECTION = 'slct';
const int32 TEXT_MENU_CHANGED = 'tmcd';

class DOMView	:	public BWindow	{

	private:
		BOutlineListView * mTree;
		BListView * mAttributes;
		BListView * mValues;
		BPopUpMenu * mTextMenu;
		DOMTextView * mText;

		BList * mItems;

		TDocumentPtr mDocument;
		TNodePtr mSelectedNode;
	
	public:
		DOMView( TDocumentPtr aDocument );
		~DOMView();
		void MessageReceived( BMessage * aMessage );
		bool QuitRequested();
		void showTree( const TNodePtr aNode, BStringItem * aParent );
		void showDocument();
		void setDocument( TDocumentPtr aDocument );
		TNodePtr findNode( TNodePtr aNode, int32 aTarget, int32 & aCurrent );
		
	
};

#endif
