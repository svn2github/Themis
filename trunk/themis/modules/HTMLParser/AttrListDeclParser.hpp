/*	AttrListDeclParser
	Parses an attribute list declaration
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	11-04-2003

*/

#ifndef ATTRLISTDECLPARSER_HPP
#define ATTRLISTDECLPARSER_HPP

// Standard C++ headers
#include <string>

// SGMLParser headers
#include "DeclarationParser.hpp"
#include "SGMLText.hpp"

class AttrListDeclParser	:	public DeclarationParser	{

	public:
		AttrListDeclParser( SGMLTextPtr aDocText, TSchemaPtr aDTD );
		~AttrListDeclParser();
		virtual bool processDeclaration();
		TElementPtr processAssElementType();
		TElementPtr processAttrDefList();
		TElementPtr processAttrDef();
		string processAttrName();
		string processDeclValue();
		string processDefValue();
	
};

#endif
