/*	ElementDeclParser
	Parses an element  declaration
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	11-04-2003

*/

#ifndef ELEMENTDECLPARSER_HPP
#define ELEMENTDECLPARSER_HPP

// Standard C++ headers
#include <map>
#include <string>

// SGMLParser headers
#include "DeclarationParser.hpp"
#include "Position.hpp"

// Namespaces used
using namespace std;

class ElementDeclParser	:	public DeclarationParser	{

	private:
		TElementPtr mElements;
	
	public:
		ElementDeclParser( SGMLTextPtr aDocText, TDocumentPtr aDTD );
		~ElementDeclParser();
		bool processDeclaration();
		TElementPtr processElementType();
		void processTagMin( TElementPtr aElement );
		bool processDeclContent( TElementPtr aElement,
											  bool aException = true );
		void processContentModel( TElementPtr aElement );
		TElementPtr processModelGroup();
		TElementPtr processSubModelGroup();
		TElementPtr processContentToken();
		TElementPtr processPrimContentToken();
		TElementPtr processElementToken();
		TElementPtr processOccIndicator( TElementPtr aElementToken );
		TElementPtr processExceptions();
		bool processExclusions( TElementPtr aExceptions );
		bool processInclusions( TElementPtr aExceptions );

		// Test function
		void showTree( TNodePtr aNode, int aSpacing );
	
};

#endif
