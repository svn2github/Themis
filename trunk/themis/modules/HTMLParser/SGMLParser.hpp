/* SGMLParser
	Parses an sgml file, using a dtd
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	27-02-2003
	
*/

#ifndef SGMLPARSER_HPP
#define SGMLPARSER_HPP

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

// Declarations of SGMLParser classes
class CommentDeclParser;
class DTDParser;
class DocTypeDeclParser;
class ElementParser;

using std::string;
using std::vector;
using std::map;

class SGMLParser	:	public BaseParser	{
	
	private:
		void createDTD();
		// Test function
		void showTree( TNodePtr aNode, int aSpacing );

	protected:
		// Declaration parsers
		CommentDeclParser * mCommentDecl;
		DTDParser * mDtdParser;
		DocTypeDeclParser * mDocTypeDecl;
		ElementParser * mElementParser;
		map<string, TSchemaPtr> mDtds;
		string mDefaultDtd;
		
		// Functions
		void setupParsers();
		void processSGMLDocument();
		void processSGMLDocEntity();
		void processProlog();
		void processOtherProlog();
		void processOtherPrologStar();
		void processBaseDocTypeDecl();
		string processDocTypeName();
		void processDocInstanceSet();
		void processBaseDocElement();
		void processDocElement();

	public:
		// Constructor
		SGMLParser( const char * aDocument = NULL );
		SGMLParser( SGMLTextPtr aDocument );
		// Destructor
		~SGMLParser();
		// Parsing functions
		TDocumentPtr parse( const char * aDtd, SGMLTextPtr aDocument );
		TDocumentPtr parse( const char * aDtd, const char * aDocument );
		void parseDTD( const char * aDtd );
	
};

#endif
