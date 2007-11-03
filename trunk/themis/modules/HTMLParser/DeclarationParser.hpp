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
using std::map;
using std::string;

class DeclarationParser	:	public BaseParser	{
	
	public:
		DeclarationParser( SGMLTextPtr aDocText, TSchemaPtr aDTD );
		virtual ~DeclarationParser();
		void setDocText( SGMLTextPtr aDocText );
		void setDTD( TSchemaPtr aDTD );
		bool parse( const map<string, Position> & aEntityTexts );
		bool parse();	// If you don't need entities
		virtual bool processDeclaration();
		bool processExtEntitySpec( TElementPtr & entity );
		bool processExternalId( TElementPtr & entity );
		string processPublicId();
		string processSystemId();
	
};

#endif
