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
		TElementPtr mElements;
		TDocumentPtr mDocument;
		CommentDeclParser * commentParser;
	
	public:
		ElementParser( SGMLTextPtr aDocText, TDocumentPtr aDTD );
		virtual ~ElementParser();
		void setDocText( SGMLTextPtr aDocText );
		void setDTD( TDocumentPtr aDTD );
		void parse( const map<string, Position> & aEntityTexts, const string & aName );
		void parse( const string & aName );	// If you don't need entities
		TDocumentPtr getDocument() const;
		void processElementContent( const TElementPtr & aElementDecl,
												   TNodePtr aParent );
		void processUnknownTags( TNodePtr aParent );
		void processUnknownStartTag( TNodePtr aParent );
		void processUnknownEndTag();
		void processElement( const TElementPtr & aElementDecl, TNodePtr aParent );
		TElementPtr processStartTag( const TElementPtr & elementDecl, TNodePtr aParent );
		void processEndTag( const TElementPtr & elementDecl );
		string processGenIdSpec();
		void processAttrSpecList( TNodePtr aParent );
		void processAttrSpec( TNodePtr aParent );
		void processContent( const TElementPtr & aContent,
										const TElementPtr & aExceptions,
										TNodePtr aParent );
		void processBrackets( const TElementPtr & aContent,
										 const TElementPtr & aExceptions,
										 TNodePtr aParent );
		void processOptional( const TElementPtr & aContent,
										const TElementPtr & aExceptions,
										TNodePtr aParent );
		void processPlus( const TElementPtr & aContent,
								  const TElementPtr & aExceptions,
								  TNodePtr aParent );
		void processStar( const TElementPtr & aContent,
								  const TElementPtr & aExceptions,
								  TNodePtr aParent );
		void processOr( const TElementPtr & aContent,
								const TElementPtr & aExceptions,
								TNodePtr aParent );
		void processAnd( const TElementPtr & aContent,
								  const TElementPtr & aExceptions,
								  TNodePtr aParent );
		void processComma( const TElementPtr & aContent,
									   const TElementPtr & aExceptions,
									   TNodePtr aParent );
		void processDataText( const TElementPtr & aContent,
										 const TElementPtr & aExceptions,
										 TNodePtr aParent );
		void processExceptions( const TElementPtr & aExceptions, TNodePtr aParent );
		void processException( const TElementPtr & aExceptions, TNodePtr aParent );
		void processExceptionOtherContent();
		void processComments();
		void processComment();
		TElementPtr getElementDecl( const string & aName,
												   TElementPtr declarations ) const;

};

#endif
