/* DTDParser
	Parses a dtd
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	14-04-2003
	
*/

#ifndef DTDPARSER_HPP
#define DTDPARSER_HPP

// Standard C++ headers
#include <string>
#include <vector>
#include <map>

// Themis headers
#include "DOMSupport.h"
#include "TDocument.h"

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

using namespace std;

class DTDParser	:	public BaseParser	{
	
	protected:
		// Declaration parsers
		CommentDeclParser * commentDecl;
		MarkedSecDeclParser * markedSecDecl;
		EntityDeclParser * entityDecl;
		ElementDeclParser * elementDecl;
		AttrListDeclParser * attrListDecl;
		
		// Functions
		void processDeclaration();
		void processDs();
		void processDsStar();

	public:
		// Constructor
		DTDParser( const char * aFileName, TDocumentShared aDTD );
		// Destructor
		~DTDParser();
		// Parsing function
		TDocumentShared parse();
	
};

#endif
