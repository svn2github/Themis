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
#include "Position.hpp"

// Declarations of SGMLParser classes
class CommentDeclParser;
class DTDParser;
class DocTypeDeclParser;
class ElementParser;

using namespace std;

class SGMLParser	:	public BaseParser	{
	
	private:
		// Test function
		void showTree( TNodeShared aNode, int aSpacing );

	protected:
		// Declaration parsers
		CommentDeclParser * mCommentDecl;
		DTDParser * mDtdParser;
		DocTypeDeclParser * mDocTypeDecl;
		ElementParser * mElementParser;
		
		// Functions
		void setupParsers( const char * aDtd );
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
		SGMLParser( const char * aDtd, const char * aDocument );
		SGMLParser( const char * aDtd, SGMLTextPtr aDocument );
		// Destructor
		~SGMLParser();
		// Parsing function
		TDocumentShared parse();
	
};

#endif
