/*	MarkedSecDeclParser
	Parses a marked section declaration
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	11-04-2003

*/

#ifndef MARKEDSECDECLPARSER_HPP
#define MARKEDSECDECLPARSER_HPP

// SGMLParser headers
#include "DeclarationParser.hpp"

class MarkedSecDeclParser	:	public DeclarationParser	{
	
	public:
		MarkedSecDeclParser( SGMLTextPtr aDocText, TSchemaPtr aDTD );
		~MarkedSecDeclParser();
		bool processDeclaration();
		void processStatusKeyWordSpec();
		bool processStatusKeyWord();

};

#endif
