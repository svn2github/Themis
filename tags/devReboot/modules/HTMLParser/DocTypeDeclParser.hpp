/*	DocTypeDeclParser
	Parses a document type declaration
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	14-04-2003

*/

#ifndef DOCTYPEDECLPARSER_HPP
#define DOCTYPEDECLPARSER_HPP

// Standard C++ headers
#include <string>

// SGMLParser headers
#include "DeclarationParser.hpp"
#include "SGMLText.hpp"

// Namespaces used
using std::string;

class DocTypeDeclParser	:	public DeclarationParser	{
	
	private:
		string mDocTypeName;
	
	public:
		DocTypeDeclParser( SGMLTextPtr aDocText, TSchemaPtr aDTD );
		~DocTypeDeclParser();
		virtual bool processDeclaration();
		string processDocTypeName();
		string getDocTypeName() const;
	
};

#endif
