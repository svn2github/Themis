/*	MarkedSecDeclParser
	Parses a marked section declaration
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	11-04-2003

*/

#ifndef MARKEDSECDECLPARSER_HPP
#define MARKEDSECDECLPARSER_HPP

// Standard C++ headers
#include <map>
#include <string>

// SGMLParser headers
#include "DeclarationParser.hpp"
#include "Position.hpp"

// Namespaces used
using namespace std;

class MarkedSecDeclParser	:	public DeclarationParser	{
	
	public:
		MarkedSecDeclParser( SGMLTextPtr aDocText, TDocumentShared aDTD );
		~MarkedSecDeclParser();
		void processDeclaration();
		void processStatusKeyWordSpec();
		void processStatusKeyWord();

};

#endif
