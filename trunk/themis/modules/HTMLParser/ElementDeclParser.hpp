/*	ElementDeclParser
	Parses an element  declaration
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	11-04-2003

*/

#ifndef ELEMENTDECLPARSER_HPP
#define ELEMENTDECLPARSER_HPP

// SGMLParser headers
#include "DeclarationParser.hpp"

class ElementDeclParser	:	public DeclarationParser	{

	private:
		TElementPtr mElements;
	
	public:
		ElementDeclParser( SGMLTextPtr aDocText, TSchemaPtr aDTD );
		~ElementDeclParser();
		bool processDeclaration();
		TElementPtr processElementType();
		bool processTagMin( TElementDeclarationPtr aDeclaration );
		bool processDeclContent( TElementPtr aElement );
		bool processContentModel( TElementPtr aElement );
		TElementPtr processModelGroup();
		TElementPtr processSubModelGroup();
		TElementPtr processContentToken();
		TElementPtr processPrimContentToken();
		TElementPtr processElementToken();
		void processOccIndicator( TElementPtr aElementToken );
		TElementPtr processExceptions();
		bool processExclusions( TElementPtr aExceptions );
		bool processInclusions( TElementPtr aExceptions );

};

#endif
