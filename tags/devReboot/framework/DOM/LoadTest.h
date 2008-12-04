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
		unsigned int mLineNr;
		unsigned int mCharNr;
		bool mAttrNoValue; // Indicates if an tag attribute has a value
		string mTag;	// Current tag
		string mContent; // Content of the to be parsed file
		StringType mStringType; // Type of the text last retrieved
		bool mCloseTag; // Indicates if the current tag is a closing tag
		TDocumentShared mDocument; // Stores the document to be build
		// Functions for examining tags
		inline bool isStartTag()	;
		inline bool isDocTypeTag();
		inline bool isHtmlTag();
		inline bool isCommentTag();
		inline bool isHeadTag();
		inline bool isBodyTag();
		inline bool isTitleTag();
		inline bool isStyleTag();
		inline bool isScriptTag();
		inline bool isIsIndexTag();
		inline bool isBaseTag();
		inline bool isMetaTag();
		inline bool isLinkTag();
		inline bool isBlockLevelTag();
		inline bool isTextLevelTag();
		inline bool isHeadingTag();
		inline bool isAdressTag();
		inline bool isPTag();
		inline bool isListTag();
		inline bool isULTag();
		inline bool isOLTag();
		inline bool isDLTag();
		inline bool isLITag();
		inline bool isDDTag();
		inline bool isDTTag();
		inline bool isPreTag();
		inline bool isBodyStyleTag();
		inline bool isFormTag();
		inline bool isEmptyBlockTag();
		inline bool isTableTag();
		inline bool isTrTag();
		inline bool isCaptionTag();
		inline bool isTdTag();
		inline bool isThTag();
		inline bool isFontStyleTag();
		inline bool isFontStylePreTag();
		inline bool isPhraseTag();
		inline bool isFormFieldTag();
		inline bool isInputTag();
		inline bool isSelectTag();
		inline bool isTextAreaTag();
		inline bool isOptionTag();
		inline bool isAnchorTag();
		inline bool isEmptyTextTag();
		inline bool isEmptyTextPreTag();
		inline bool isAppletTag();
		inline bool isParamTag();
		inline bool isFontTag();
		inline bool isMapTag();
		inline bool isAreaTag();
		// Functions for getting a part of the input
		bool isWhiteSpace();
		void nextChar();
		string getTag();
		string getText( bool aConserveSpaces = false );
		string getAttrValue();
		string getAttribute();
		string getComment();
		string getString( bool aConserveSpaces = false );
		// Functions for moving around in the input
		void skipTag(); // Skip the whole tag
		void backPedal(); // Go back to to mOldPos
		
		// Tag recognition functions
		void doctypeTag();
		void htmlTag();
		void commentTag( TElementShared aParent );
		void headTag( TElementShared aParent );
		void normalHeadTag( TElementShared aParent );
		void bodyStyleTag( TElementShared aParent, bool aInsideForm = false );
		void adressTag( TElementShared aParent );
		void blockLevelTag( TElementShared aParent, bool aInsideForm = false );
		void textLevelTag( TElementShared aParent, bool aConserveSpaces = false, bool aInsideAnchor = false );
		void flowLevelTag( TElementShared aParent );
		void formFieldTag( TElementShared aParent, bool aConserveSpaces = false );
		void pTag( TElementShared aParent );
		void listTag( TElementShared aParent );
		void tableTag( TElementShared aParent );
		void trTag( TElementShared aParent );
		void preTag( TElementShared aParent );
		void emptyElementTag( TElementShared aParent );
		void selectTag( TElementShared aParent, bool aConserveSpaces = false );
		void pcDataTag( TElementShared aParent, bool aConserveSpaces = false );
		void appletTag( TElementShared aParent, bool aConserveSpaces = false, bool aInsideAnchor = false );
		void mapTag( TElementShared aParent );
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
