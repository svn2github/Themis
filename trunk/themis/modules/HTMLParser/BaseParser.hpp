/* BaseParser
	Provides the base functionality for all parser classes
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	12-04-2003
	
*/

#ifndef BASEPARSER_HPP
#define BASEPARSER_HPP

// Standard C++ headers
#include <string>
#include <vector>
#include <map>

// Boost headers
#include "boost/shared_ptr.hpp"

// Themis headers
#include "DOMSupport.h"
#include "TDocument.h"

// SGMLParser headers
#include "SGMLText.hpp"
#include "Position.hpp"

// Namespaces used
using namespace std;

// Typedefinitions
typedef boost::shared_ptr<SGMLText> SGMLTextPtr;

class BaseParser	{
	
	protected:
		// Variables
		TDocumentShared mDocument;
		TDocumentShared mDTD;
		TElementShared mParEntities;
		TElementShared mCharEntities;
		SGMLTextPtr mDocText;
		
		map<string, Position> mEntityTexts;
		
		// Reference syntax
		string mMdo;		// Markup open declaration
		string mMdc;		// Markup close declaration
		string mDso;		// Declaration subset open delimeter
		string mMsc;		// Marked section close delimeter
		string mGrpo;	// Group open delimeter
		string mGrpc;		// Group close delimeter
		string mRefc;		// Reference end
		string mCom;		// Comment delimiter
		string mPero;		// Parameter entity reference open delimiter
		string mLit;		// Literal delimiter
		string mLitA;		// Alternative literal delimiter
		string mRni;		// Reserved name indicator
		string mOpt;		// Optional element
		string mPlus;		// Required and repeatable element
		string mRep;		// Optional and repeatable element
		string mAnd;		// Elements must occur in any order
		string mOr;		// One and only one element must occur
		string mSeq;		// Elements must occur in the order entered
		string mStago;	// Element start tag opening symbol
		string mTagc;		// Element tag closing symbol
		string mEtago;	// Element end tag opening symbol
		string mVi;			// Equals sign for attribute specification
		
		// Functions
		void setupSyntax();
		void process( const string & symbol );
		void processS();
		void processEe();
		void processComment();
		void processParEntityReference();
		void processParLiteral( TElementShared & entity );
		string processMinLiteral();
		string processMinData();
		string processName();
		string processNameToken();
		void processSStar();
		void processPs();
		void processPsStar();
		void processPsPlus();
		void processTs();
		void processTsStar();
		string processRepParData();
		string processRepCharData();
		string processGI();
		TElementShared processNameGroup();
		string processNameTokenGroup();
		TElementShared processConnector();
		string processAttrValueSpec();
		string processAttrValue();
		string processAttrValueLit();
		string processCharData( string aEndString, bool aSpaceEnd = true );
		char processDataChar();

	public:
		// Constructor
		BaseParser();
		// Destructor
		virtual ~BaseParser();
		virtual void setDocText( SGMLTextPtr aDocText );
	
};

#endif
