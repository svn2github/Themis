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
using std::string;

// Declarations of SGMLParser classes
class CommentDeclParser;

class ElementParser	:	public BaseParser	{
	
	private:
		string mName;
		TDocumentPtr mDocument;
		CommentDeclParser * commentParser;
	
	public:
		ElementParser( SGMLTextPtr aDocText, TSchemaPtr aSchema );
		virtual ~ElementParser();
		void setDocText( SGMLTextPtr aDocText );
		void parse( const string & aName );
		TDocumentPtr getDocument() const;
		void processElementContent( const TDOMString & aName,
												   const TElementDeclarationPtr & aElementDecl,
												   TNodePtr aParent );
		void processUnknownTags( TNodePtr aParent );
		bool processUnknownStartTag( TNodePtr aParent );
		bool processUnknownEndTag();
		void processElement( const TDOMString & aName,
										const TElementDeclarationPtr & aDeclaration,
										TNodePtr aParent );
		TElementPtr processStartTag( const TDOMString & aName,
													const TElementPtr & elementDecl,
													TNodePtr aParent );
		void processEndTag( const TDOMString & aName );
		string processGenIdSpec();
		void processAttrSpecList( TNodePtr aParent );
		bool processAttrSpec( TNodePtr aParent );
		void skipContent( const TElementPtr & aContent );
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
		TElementDeclarationPtr getElementDecl( const string & aName,
												   				   TElementPtr declarations ) const;
		void processComments();
		bool processCommentDeclaration();
		TElementPtr getElement( TElementPtr aElement,
											 const TDOMString & aName );

};

#endif
