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
		TElementShared mElements;
	
	public:
		ElementDeclParser( SGMLTextPtr aDocText, TDocumentShared aDTD,
								 	 TElementShared aParEntities, TElementShared aCharEntities );
		~ElementDeclParser();
		virtual void processDeclaration();
		TElementShared processElementType();
		void processTagMin( TElementShared aElement );
		void processDeclContent();
		void processContentModel( TElementShared aElement );
		TElementShared processModelGroup();
		TElementShared processSubModelGroup();
		TElementShared processContentToken();
		TElementShared processPrimContentToken();
		TElementShared processElementToken();
		TElementShared processOccIndicator();
		void processExceptions( TElementShared aElement );
		void processExclusions( TElementShared aElement );
		void processInclusions( TElementShared aElement );

		// Test function
		void showTree( TNodeShared aNode, int aSpacing );
	
};

#endif
