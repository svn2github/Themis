/*	HTMLParser addon
	Basically only testing at the moment
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	03-09-2002
	
*/

#ifndef HTMLPARSER_H
#define HTMLPARSER_H

#include <SupportDefs.h>
#include <Handler.h>
#include "plugclass.h"

#include <string>

#include "DOMSupport.h"

using namespace std;

enum StringType	{
	TEXT,
	TAG,
	ATTRVALUE,
	ATTR
};

extern "C" __declspec( dllexport ) status_t Initialize( void * info = NULL );
extern "C" __declspec( dllexport ) status_t Shutdown( bool now = false );
extern "C" __declspec( dllexport ) PlugClass * GetObject();

class HTMLParser	:	public BHandler, public PlugClass	{
	
	private:
		unsigned int mPos;
		unsigned int mOldPos; // Position in string before last read.
		unsigned int mLineNr;
		unsigned int mCharNr;
		unsigned int mOldTagLineNr;
		unsigned int mOldTagCharNr;
		unsigned int mTagLineNr;
		unsigned int mTagCharNr;
		
		bool mAttrNoValue; // Indicates if an tag attribute has a value
		string mOldTag; // Previous tag
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
	
		// Main functions
		void reset();
		void showTree( const TNodeShared aNode, int aSpacing );
		void showDocument();
		void setContent( string aContent );
		void startParsing( TDocumentShared aDocument );
	
	public:
		HTMLParser( BMessage * info = NULL );
		~HTMLParser();
		void MessageReceived( BMessage * message );
		bool IsHandler();
		BHandler * Handler();
		bool IsPersistent();
		uint32 PlugID();
		char * PlugName();
		float PlugVersion();
		void Heartbeat();
		status_t ReceiveBroadcast( BMessage * message );
		int32 Type();
		
};

class ReadException	{
	
	public:
		ReadException()	{};
		~ReadException()	{};

};

#endif
