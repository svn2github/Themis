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

class HTMLParser	{
	
	private:
		unsigned int mPos;
		string mTag;	// Current tag
		string mContent; // Content of the to be parsed file
		bool mCloseTag; // Indicates if the current tag is a closing tag
		TDocumentShared mDocument; // Stores the document to be build
		void getNextTag();
		bool isStartTag()	;
		string getText( bool conserveSpaces = true );		
		
		// Tag recognition functions
		void doctypeTag();
		void htmlTag();
		void headTag( TElementShared parent );
		void titleTag( TElementShared parent );
		void bodyTag( TElementShared parent );
		void pTag( TElementShared parent );
		void headingTag( TElementShared parent );
	
	public:
		HTMLParser();
		~HTMLParser();
		void showTree( const TNodeShared node, int spacing );
		void showDocument();
		void setContent( string aContent );
		void startParsing( TDocumentShared document );
};

#endif
