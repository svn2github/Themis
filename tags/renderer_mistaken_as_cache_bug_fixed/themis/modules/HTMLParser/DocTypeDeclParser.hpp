/*	DocTypeDeclParser
	Parses a document type declaration
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	14-04-2003

*/

#ifndef DOCTYPEDECLPARSER_HPP
#define DOCTYPEDECLPARSER_HPP

// Standard C++ headers
#include <map>
#include <string>

// SGMLParser headers
#include "DeclarationParser.hpp"
#include "Position.hpp"

// Namespaces used
using namespace std;

class DocTypeDeclParser	:	public DeclarationParser	{
	
	private:
		string mDocTypeName;
	
	public:
		DocTypeDeclParser( SGMLTextPtr aDocText, TDocumentPtr aDTD );
		~DocTypeDeclParser();
		virtual void processDeclaration();
		string processDocTypeName();
		string getDocTypeName() const;
	
};

#endif
