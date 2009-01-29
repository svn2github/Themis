/*	SGMLParser implementation
	See SGMLParser.hpp for more information
	
	Also includes main()
*/

// Standard C headers
#include <stdio.h>

// DOM headers
#include "TNode.h"
#include "TNodeList.h"
#include "TNamedNodeMap.h"
#include "TElement.h"

// SGMLParser headers
#include "SGMLParser.hpp"
#include "DTDParser.hpp"
#include "TSchema.hpp"
#include "ReadException.hpp"
#include "CommentDeclParser.hpp"
#include "DocTypeDeclParser.hpp"
#include "ElementParser.hpp"
#include "ElementToken.hpp"
#include "TElementDeclaration.hpp"

SGMLParser :: SGMLParser(SGMLScanner * aScanner, TSchemaPtr aSchema)
		   : BaseParser(aScanner, aSchema) {
	
	mCommentDeclParser = new CommentDeclParser(aScanner, aSchema);
	mDocTypeDeclParser = new DocTypeDeclParser(aScanner, aSchema);
	mDTDParser = new DTDParser(aScanner, aSchema);

}

SGMLParser :: ~SGMLParser() {
	
	delete mCommentDeclParser;
	delete mDocTypeDeclParser;
	delete mDTDParser;
}

void SGMLParser :: showTree(const TNodePtr aNode, int aSpacing) {
	
	TNodeListPtr children = aNode->getChildNodes();
	int length = children->getLength();
	if ( length != 0 )	{
		for ( int i = 0; i < length; i++ )	{
			TNodePtr child = children->item( i );
			for ( int j = 0; j < aSpacing; j++ )	{
				printf("  ");
			}
			printf("Child name: %s\n", child->getNodeName().c_str());
			if ( child->getNodeType() == ELEMENT_NODE )	{
				// Check for attributes
				TNamedNodeMapPtr attributes = child->getAttributes();
				for ( unsigned int j = 0; j < attributes->getLength(); j++ )	{
					TNodePtr attr = attributes->item( j );
					for ( int j = 0; j < aSpacing + 1; j++ )	{
						printf("  ");
					}
					printf("Attribute %s", attr->getNodeName().c_str());
					printf(" with value %s\n", attr->getNodeValue().c_str());
				}
			}
			showTree( child, aSpacing + 1 );
		}
	}	
}

bool SGMLParser :: parseOtherProlog() {
	
	bool result = false;
	
	if (mToken == DECLARATION_SYM) {
		Token token = mScanner->lookAhead();
		if (token == COMMENT_SYM) {
			mToken = mCommentDeclParser->parse(token);
			result = true;
			parseS();
		}
		else
			result = false;
	}
	
	return result;
	
}

void SGMLParser :: parseOtherPrologStar() {

	bool otherFound = true;

	while (otherFound) {
		otherFound = parseOtherProlog();
	}

}

void SGMLParser :: parseBaseDocTypeDecl() {

	// If we don't find a doctype declaration, we skip it.
	if (mToken == DECLARATION_SYM) {
		mToken = mScanner->nextToken();
		if (mToken == IDENTIFIER_SYM) {
			string identifier = mScanner->getTokenText();
			if (identifier == "DOCTYPE") {
				mToken = mScanner->nextToken();
				mToken = mDocTypeDeclParser->parse(mToken);
			}
			else {
				// Found unknown token.
				string message = "Unknown token found: " + mScanner->getTokenText();
				throw ReadException(mScanner->getLineNr(),
									mScanner->getCharNr(),
									message,
									GENERIC,
									true);
			}
		}
	}

}

void SGMLParser :: parseProlog() {
	
	// Not very complete, but works for html files
	parseOtherPrologStar();
	parseBaseDocTypeDecl();
	parseOtherPrologStar();

}

void SGMLParser :: parseSchema(const char * aSchemaFile) {
	
	mDTDParser->parse(aSchemaFile);
	
}

TDocumentPtr SGMLParser :: parse(const char * aSchemaFile, string aText) {
	
	printf("Loading schema\n");
	clock_t start = clock();
	loadSchema(aSchemaFile);
	clock_t end = clock();
	printf("Time taken for loading the schema: %f\n", (double)(end - start)/CLOCKS_PER_SEC);

	start = clock();
	printf("Starting to scan the HTML document\n");
	mScanner->setDocument(aText);
	printf("Loaded the document\n");
	// Assume the doctype is HTML.
	mDocTypeName = "HTML";
	ElementParser elementParser(mScanner, mSchema, mDocTypeName);
	// See if we can scan a whole HTML document.
	TDocumentPtr document;
	try {
		mToken = mScanner->nextToken();
		parseSStar();
		printf("Got first token: %s\n", mScanner->getTokenText().c_str());
		parseProlog();
		while (mToken != EOF_SYM) {
			switch (mToken) {
				case ELEMENT_OPEN_SYM: {
					// Kickstart the element parser.
					TElementPtr element = elementParser.parseStartTag();
					TDOMString name = element->getTagName();
					ElementToken elmToken = ElementToken(START_TAG, name, element);
					TElementDeclarationPtr declaration = mSchema->getDeclaration(mDocTypeName);
					mToken = elementParser.parse(elmToken, declaration);
					document = elementParser.getDocument();
					showTree(document, 0);
					break;
				}
				case DECLARATION_SYM: {
					mToken = mScanner->nextToken();
					if (mToken == COMMENT_SYM) {
						if (mCommentDeclParser == NULL)
							mCommentDeclParser = new CommentDeclParser(mScanner, TSchemaPtr());
						mToken = mCommentDeclParser->parse(mToken, ELEMENT_OPEN_SYM);
					}
					else
						throw ReadException(mScanner->getLineNr(),
											mScanner->getCharNr(),
											"Expected comment sym",
											GENERIC,
											true);
					break;
				}
				case DECLARATION_END_SYM: {
					mToken = mScanner->nextToken(ELEMENT_OPEN_SYM);
					break;
				}
				case TEXT_SYM: {
					mToken = mScanner->nextToken();
					break;
				}
				case SPACE_SYM: {
					// Not doing anything with that right now.
					mToken = mScanner->nextToken();
					break;
				}
				default: {
					printf("Found token: %s\n", mScanner->getTokenText().c_str());
					mToken = mScanner->nextToken();
				}
			}
		}
	}
	catch(ReadException r) {
		printf(
			"Found error: line: %i char %i message: %s\n",
			r.getLineNr(),
			r.getCharNr(),
			r.getErrorMessage().c_str());
	}

	end = clock();
	printf("Time taken: %f\n", (double)(end - start)/CLOCKS_PER_SEC);

	return document;

}

void SGMLParser :: parse(const char * aSchemaFile, const char * aDocument) {

	printf("Loading schema\n");
	clock_t start = clock();
	loadSchema(aSchemaFile);
	clock_t end = clock();
	printf("Time taken for loading the schema: %f\n", (double)(end - start)/CLOCKS_PER_SEC);

	start = clock();
	printf("Starting to scan the HTML document\n");
	mScanner->setDocument(aDocument);
	printf("Loaded the document\n");
	// Assume the doctype is HTML.
	mDocTypeName = "HTML";
	ElementParser elementParser(mScanner, mSchema, mDocTypeName);
	// See if we can scan a whole HTML document.
	try {
		mToken = mScanner->nextToken();
		parseSStar();
		printf("Got first token: %s\n", mScanner->getTokenText().c_str());
		parseProlog();
		while (mToken != EOF_SYM) {
			switch (mToken) {
				case ELEMENT_OPEN_SYM: {
					// Kickstart the element parser.
					TElementPtr element = elementParser.parseStartTag();
					TDOMString name = element->getTagName();
					ElementToken elmToken = ElementToken(START_TAG, name, element);
					TElementDeclarationPtr declaration = mSchema->getDeclaration(mDocTypeName);
					mToken = elementParser.parse(elmToken, declaration);
					TDocumentPtr document = elementParser.getDocument();
					showTree(document, 0);
					break;
				}
				case DECLARATION_SYM: {
					mToken = mScanner->nextToken();
					if (mToken == COMMENT_SYM) {
						if (mCommentDeclParser == NULL)
							mCommentDeclParser = new CommentDeclParser(mScanner, TSchemaPtr());
						mToken = mCommentDeclParser->parse(mToken, ELEMENT_OPEN_SYM);
					}
					else
						throw ReadException(mScanner->getLineNr(),
											mScanner->getCharNr(),
											"Expected comment sym",
											GENERIC,
											true);
					break;
				}
				case DECLARATION_END_SYM: {
					mToken = mScanner->nextToken(ELEMENT_OPEN_SYM);
					break;
				}
				case TEXT_SYM: {
					mToken = mScanner->nextToken();
					break;
				}
				case SPACE_SYM: {
					// Not doing anything with that right now.
					mToken = mScanner->nextToken();
					break;
				}
				default: {
					printf("Found token: %s\n", mScanner->getTokenText().c_str());
					mToken = mScanner->nextToken();
				}
			}
		}
	}
	catch(ReadException r) {
		printf(
			"Found error: line: %i char %i message: %s\n",
			r.getLineNr(),
			r.getCharNr(),
			r.getErrorMessage().c_str());
	}

	end = clock();
	printf("Time taken: %f\n", (double)(end - start)/CLOCKS_PER_SEC);

}

void SGMLParser :: loadSchema(const char * aSchemaFile) {

	string schemaFileName = aSchemaFile;

	if (mSchemas.size() == 0) {
		// We need to parse it first.
		printf("Parsing first schema\n");
		parseSchema(aSchemaFile);
		mSchema->computeSchema();
		mSchemas.insert(map<string, TSchemaPtr>::value_type(schemaFileName, mSchema));
	}
	else if (mSchemas.count(schemaFileName)) {
		// We already parsed it once. Just reload it as current.
		map<string, TSchemaPtr>::iterator i = mSchemas.find(schemaFileName);
		printf("Found an existing schema\n");
		mSchema = (*i).second;
		mCommentDeclParser->setSchema(mSchema);
		mDocTypeDeclParser->setSchema(mSchema);
		mDTDParser->setSchema(mSchema);
	}
	else {
		// There are already schemas parsed, but not the one requested.
		printf("Parsing additional schema\n");
		// Create a new schema to parse into.
		mSchema = TSchemaPtr(new TSchema());
		mSchema->setup();
		printf("Setup done. Now assigning\n");
		mCommentDeclParser->setSchema(mSchema);
		mDocTypeDeclParser->setSchema(mSchema);
		mDTDParser->setSchema(mSchema);
		
		// Parse the schema and store it.
		printf("Parsing...\n");
		parseSchema(aSchemaFile);
		mSchema->computeSchema();
		mSchemas.insert(map<string, TSchemaPtr>::value_type(schemaFileName, mSchema));
	}

}

int main( int argc, char * argv[] )	{
	
	if ( argc < 3 )	{
		printf("Please supply a dtd and a document to load\n");
		return 1;
	}

	TSchemaPtr schema = TSchemaPtr(new TSchema());
	schema->setup();
	
	SGMLScanner * scanner = new SGMLScanner();
	SGMLParser parser(scanner, schema);
	parser.parse(argv[1], argv[2]);
	
	return 0;
	
}
