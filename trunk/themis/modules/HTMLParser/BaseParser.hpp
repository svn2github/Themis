/*
	Copyright (c) 2003 Mark Hellegers. All Rights Reserved.
	
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
	Class Start Date: April 12, 2003
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

/// Base class for all parser classes.

/**
	This class provides the base functionality for all the parser classes
	the SGML parser uses.
*/

class BaseParser	{
	
	protected:
		// Variables
		/// The resulting document tree.
		TDocumentPtr mDocument;
		/// The dtd to use when parsing.
		TDocumentPtr mDTD;
		/// The parameter entities stored in the dtd.
		TElementPtr mParEntities;
		/// The character entities stored in the dtd.
		TElementPtr mCharEntities;
		/// The SGML text to parse.
		SGMLTextPtr mDocText;
		
		/// The entity texts.
		/**
			@todo	This one needs some looking at.
		*/
		map<string, Position> mEntityTexts;
		
		// Reference syntax
		/// Markup open declaration.
		string mMdo;
		/// Markup close declaration.
		string mMdc;
		/// Declaration subset open delimeter.
		string mDso;
		/// Marked section close delimeter.
		string mMsc;
		/// Group open delimeter.
		string mGrpo;
		/// Group close delimeter.
		string mGrpc;	
		/// Reference end.
		string mRefc;	
		/// Comment delimiter.
		string mCom;
		/// Parameter entity reference open delimiter.
		string mPero;
		/// Literal delimiter.
		string mLit;
		/// Alternative literal delimiter.
		string mLitA;
		/// Reserved name indicator.
		string mRni;
		/// Optional element.
		string mOpt;
		/// Required and repeatable element.
		string mPlus;
		/// Optional and repeatable element.
		string mRep;
		/// Elements must occur in any order.
		string mAnd;
		/// One and only one element must occur.
		string mOr;
		/// Elements must occur in the order entered.
		string mSeq;
		/// Element start tag opening symbol.
		string mStago;
		/// Element tag closing symbol.
		string mTagc;
		/// Element end tag opening symbol.
		string mEtago;
		/// Equals sign for attribute specification.
		string mVi;
		
		// Functions
		void createDTD();
		void setupSyntax();
		bool process( const string & symbol, bool aException = true );
		bool processS( bool aException = true );
		bool processEe( bool aException = true );
		void processComment();
		void processParEntityReference();
		string processLiteral();
		void processParLiteral( TElementPtr & entity );
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
		TElementPtr processNameGroup();
		string processNameTokenGroup();
		TElementPtr processConnector();
		string processAttrValueSpec();
		string processAttrValue();
		string processAttrValueLit();
		string processCharData( string aEndString, bool aSpaceEnd = true );
		char processDataChar();

	public:
		/// Constructor of the BaseParser class.
		/**
			The constructor of the BaseParser class. It creates a new sgml text
			and a DTD and sets up the syntax.
		*/
		BaseParser();
		/// Destructor of the BaseParser class.
		/**
			The destructor of the BaseParser class does nothing.
			Everything is cleaned up	automatically.
		*/
		virtual ~BaseParser();
		/// Set the document text to the argument.
		/**
			Set the document text to the text provided in the argument.
			
			@param	aDocText	The document text to set the parser to.
		*/
		virtual void setDocText( SGMLTextPtr aDocText );
	
};

#endif
