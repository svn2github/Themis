/*	ElementParser
	Parses an element
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	17-04-2003

*/

#ifndef ELEMENTPARSER_HPP
#define ELEMENTPARSER_HPP

// Standard C++ headers
#include <map>
#include <string>

// SGMLParser headers
#include "BaseParser.hpp"
#include "Position.hpp"

// Namespaces used
using namespace std;

// Declarations of SGMLParser classes
class CommentDeclParser;

class ElementParser	:	public BaseParser	{
	
	private:
		string mName;
		TElementShared mElements;
		TDocumentShared mDocument;
		CommentDeclParser * commentParser;
	
	public:
		ElementParser( SGMLTextPtr aDocText, TDocumentShared aDTD,
							   TElementShared aParEntities, TElementShared aCharEntities );
		virtual ~ElementParser();
		void parse( const map<string, Position> & aEntityTexts, const string & aName );
		void parse( const string & aName );	// If you don't need entities
		void processElementContent( const TElementShared & aElementDecl,
												   TNodeShared aParent );
		void processElement( const TElementShared & aElementDecl, TNodeShared aParent );
		void processStartTag( const TElementShared & elementDecl );
		void processEndTag( const TElementShared & elementDecl );
		string processGenIdSpec();
		void processAttrSpecList();
		void processAttrSpec();
		void processContent( const TElementShared & aContent,
										const TElementShared & aExceptions,
										TNodeShared aParent );
		void processExceptions( const TElementShared & aExceptions, TNodeShared aParent );
		void processException( const TElementShared & aExceptions, TNodeShared aParent );
		void processComments();
		void processComment();
		TElementShared getElementDecl( const string & aName,
														TElementShared declarations ) const;

		// Test function
		void showTree( TNodeShared aNode, int aSpacing );
	
};

#endif
