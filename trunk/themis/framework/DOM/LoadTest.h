/*	LoadTest header file
	Define a class HTMLParser here
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	03-07-2002

*/

#ifndef LOADTEST_H
#define LOADTEST_H

#include <string>

#include "DOMSupport.h"

using namespace std;

enum StringType	{
	TEXT,
	TAG,
	ATTRVALUE,
	ATTR
};

class HTMLParser	{
	
	private:
		unsigned int mPos;
		unsigned int mOldPos; // Position in string before last read.
		string mTag;	// Current tag
		string mContent; // Content of the to be parsed file
		StringType mStringType; // Type of the text last retrieved
		bool mCloseTag; // Indicates if the current tag is a closing tag
		TDocumentShared mDocument; // Stores the document to be build
		string getTag();
		bool isStartTag()	;
		bool isFontStyleTag();
		string getText( bool aConserveSpaces = true );
		string getAttrValue();
		string getAttribute();		
		string getString();
		void backPedal(); // Go back to to mOldPos
		
		// Tag recognition functions
		void doctypeTag();
		void htmlTag();
		void headTag( TElementShared aParent );
		void titleTag( TElementShared aParent );
		void bodyTag( TElementShared aParent );
		void pTag( TElementShared aParent );
		void headingTag( TElementShared aParent );
		void preTag( TElementShared aParent );
		void hrTag( TElementShared aParent );
		void blockquoteTag( TElementShared aParent );
		void aTag( TElementShared aParent );
		void fontStyleTag( TElementShared aParent );
	
	public:
		HTMLParser();
		~HTMLParser();
		void showTree( const TNodeShared aNode, int aSpacing );
		void showDocument();
		void setContent( string aContent );
		void startParsing( TDocumentShared aDocument );
};

#endif
