/*	EntityDeclParser
	Parses an entity  declaration
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	11-04-2003

*/

#ifndef ENTITYDECLPARSER_HPP
#define ENTITYDECLPARSER_HPP

// Standard C++ headers
#include <map>
#include <string>

// SGMLParser headers
#include "DeclarationParser.hpp"
#include "Position.hpp"

// Namespaces used
using namespace std;

class EntityDeclParser	:	public DeclarationParser	{
	
	public:
		EntityDeclParser( SGMLTextPtr aDocText, TDocumentShared aDTD,
								  TElementShared aParEntities, TElementShared aCharEntities );
		~EntityDeclParser();
		map<string, Position> getEntityTexts();
		void processDeclaration();
		TElementShared processEntityName();
		void processEntityText( TElementShared & entity );
		void processDataText( TElementShared & entity );
		string processGenEntityName();
		string processParEntityName();
	
};

#endif
