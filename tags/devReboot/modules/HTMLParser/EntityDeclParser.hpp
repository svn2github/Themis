/*	EntityDeclParser
	Parses an entity  declaration
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	11-04-2003

*/

#ifndef ENTITYDECLPARSER_HPP
#define ENTITYDECLPARSER_HPP

// SGMLParser headers
#include "DeclarationParser.hpp"
#include "Position.hpp"

class EntityDeclParser	:	public DeclarationParser	{
	
	public:
		EntityDeclParser( SGMLTextPtr aDocText, TSchemaPtr aDTD );
		~EntityDeclParser();
		bool processDeclaration();
		TElementPtr processEntityName();
		void processEntityText( TElementPtr & entity );
		bool processDataText( TElementPtr & entity );
		string processGenEntityName();
		string processParEntityName();
	
};

#endif
