/*	CommentDeclParser
	Parses a comment declaration
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	11-04-2003

*/

#ifndef COMMENTDECLPARSER_HPP
#define COMMENTDECLPARSER_HPP

// SGMLParser headers
#include "DeclarationParser.hpp"

class CommentDeclParser	:	public DeclarationParser	{
	
	public:
		CommentDeclParser( SGMLTextPtr aDocText, TSchemaPtr aDTD );
		~CommentDeclParser();
		virtual bool processDeclaration();
	
};

#endif
