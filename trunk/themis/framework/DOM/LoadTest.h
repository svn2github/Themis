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
		// Functions for examining tags
		bool isStartTag()	;
		bool isDocTypeTag();
		bool isHtmlTag();
		bool isHeadTag();
		bool isBodyTag();
		bool isTitleTag();
		bool isStyleTag();
		bool isScriptTag();
		bool isIsIndexTag();
		bool isBaseTag();
		bool isMetaTag();
		bool isLinkTag();
		bool isBlockLevelTag();
		bool isTextLevelTag();
		bool isHeadingTag();
		bool isAdressTag();
		bool isPTag();
		bool isListTag();
		bool isULTag();
		bool isOLTag();
		bool isDLTag();
		bool isLITag();
		bool isDDTag();
		bool isDTTag();
		bool isPreTag();
		bool isBodyStyleTag();
		bool isFormTag();
		bool isEmptyBlockTag();
		bool isTableTag();
		bool isTrTag();
		bool isCaptionTag();
		bool isTdTag();
		bool isThTag();
		bool isFontStyleTag();
		bool isFontStylePreTag();
		bool isPhraseTag();
		bool isFormFieldTag();
		bool isAnchorTag();
		bool isEmptyTextTag();
		bool isEmptyTextPreTag();
		bool isAppletTag();
		bool isFontTag();
		bool isMapTag();
		// Functions for getting a part of the input
		string getTag();
		string getText( bool aConserveSpaces = false );
		string getAttrValue();
		string getAttribute();		
		string getString( bool aConserveSpaces = false );
		// Functions for moving around in the input
		void skipTag(); // Skip the whole tag
		void backPedal(); // Go back to to mOldPos
		
		// Tag recognition functions
		void doctypeTag();
		void htmlTag();
		void headTag( TElementShared aParent );
		void normalHeadTag( TElementShared aParent );
		void bodyStyleTag( TElementShared aParent, bool aInsideForm = false );
		void adressTag( TElementShared aParent );
		void blockLevelTag( TElementShared aParent, bool aInsideForm = false );
		void textLevelTag( TElementShared aParent, bool aConserveSpace = false, bool aInsideAnchor = false );
		void flowLevelTag( TElementShared aParent );
		void pTag( TElementShared aParent );
		void listTag( TElementShared aParent );
		void tableTag( TElementShared aParent );
		void trTag( TElementShared aParent );
		void preTag( TElementShared aParent );
		void emptyElementTag( TElementShared aParent );
		void normalTextTag( TElementShared aParent, bool aConserveSpaces = false, bool aInsideAnchor = false );
	
	public:
		HTMLParser();
		~HTMLParser();
		void showTree( const TNodeShared aNode, int aSpacing );
		void showDocument();
		void setContent( string aContent );
		void startParsing( TDocumentShared aDocument );
};

class ReadException	{
	
	public:
		ReadException()	{};
		~ReadException()	{};

};

#endif
