/* DTDParser
	Parses a dtd
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	14-04-2003
	
*/

#ifndef DTDPARSER_HPP
#define DTDPARSER_HPP

// Themis headers
#include "DOMSupport.h"

// SGMLParser headers
#include "BaseParser.hpp"
#include "SGMLText.hpp"
#include "Position.hpp"

// Declarations of SGMLParser classes
class CommentDeclParser;
class MarkedSecDeclParser;
class EntityDeclParser;
class ElementDeclParser;
class AttrListDeclParser;

class DTDParser	:	public BaseParser	{
	
	protected:
		// Declaration parsers
		CommentDeclParser * commentDecl;
		MarkedSecDeclParser * markedSecDecl;
		EntityDeclParser * entityDecl;
		ElementDeclParser * elementDecl;
		AttrListDeclParser * attrListDecl;
		
		// Functions
		bool processDeclaration();
		bool processDs();
		void processDsStar();

	public:
		// Constructor
		DTDParser( const char * aFileName, TSchemaPtr aDTD );
		// Destructor
		~DTDParser();
		// Setting a new DTD
		void setDTD( TSchemaPtr aDTD );
		// Parsing function
		TSchemaPtr parse();
		TSchemaPtr parse( const char * aFileName );
	
};

#endif
