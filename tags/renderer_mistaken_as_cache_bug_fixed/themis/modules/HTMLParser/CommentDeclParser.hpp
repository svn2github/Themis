/*	CommentDeclParser
	Parses a comment declaration
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	11-04-2003

*/

#ifndef COMMENTDECLPARSER_HPP
#define COMMENTDECLPARSER_HPP

// Standard C++ headers
#include <map>
#include <string>

// SGMLParser headers
#include "DeclarationParser.hpp"
#include "Position.hpp"

// Namespaces used
using namespace std;

class CommentDeclParser	:	public DeclarationParser	{
	
	public:
		CommentDeclParser( SGMLTextPtr aDocText, TDocumentPtr aDTD );
		~CommentDeclParser();
		virtual void processDeclaration();
	
};

#endif
