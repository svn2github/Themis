/*	DeclarationParser
	Provides the base functionality for declaration parsers
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	12-04-2003

*/

#ifndef DECLARATIONPARSER_HPP
#define DECLARATIONPARSER_HPP

// Standard C++ headers
#include <map>
#include <string>

// SGMLParser headers
#include "BaseParser.hpp"
#include "Position.hpp"

// Namespaces used
using namespace std;

class DeclarationParser	:	public BaseParser	{
	
	public:
		DeclarationParser( SGMLTextPtr aDocText, TDocumentShared aDTD,
									TElementShared aParEntities, TElementShared aCharEntities );
		virtual ~DeclarationParser();
		void parse( const map<string, Position> & aEntityTexts );
		void parse();	// If you don't need entities
		virtual void processDeclaration();
		void processExtEntitySpec( TElementShared & entity );
		void processExternalId( TElementShared & entity );
		string processPublicId();
		string processSystemId();
	
};

#endif
