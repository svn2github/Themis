/*	AttrListDeclParser
	Parses an attribute list declaration
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	11-04-2003

*/

#ifndef ATTRLISTDECLPARSER_HPP
#define ATTRLISTDECLPARSER_HPP

// Standard C++ headers
#include <map>
#include <string>

// SGMLParser headers
#include "DeclarationParser.hpp"
#include "SGMLText.hpp"
#include "Position.hpp"

// Namespaces used
using namespace std;

class AttrListDeclParser	:	public DeclarationParser	{

	private:
		TElementPtr mAttrLists;
		
	public:
		AttrListDeclParser( SGMLTextPtr aDocText, TDocumentPtr aDTD );
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
