/*
	Copyright (c) 2006 Mark Hellegers. All Rights Reserved.
	
	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or
	sell copies of the Software, and to permit persons to whom
	the Software is furnished to do so, subject to the following
	conditions:
	
	   The above copyright notice and this permission notice
	   shall be included in all copies or substantial portions
	   of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
	KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
	WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
	OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
	OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
	OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
	
	Original Author: 	Mark Hellegers (mark@firedisk.net)
	Project Start Date: October 18, 2000
	Class Start Date: June 12, 2006
*/

/*
	Schema implementation
	See TSchema.hpp for some more information
*/

// SGMLParser headers
#include "TSchema.hpp"
#include "TSchemaRule.hpp"
#include "TElementDeclaration.hpp"
#include "ElementDeclException.hpp"

// DOM headers
#include "TNode.h"
#include "TNodeList.h"
#include "TElement.h"

// Standard C headers
#include <stdio.h>

TSchema::	TSchema()
		:	TDocument()	{

}

TSchema	::	~TSchema()	{
	
}

void TSchema	::	setup()	{

	// Element to store parameter entities
	mParEntities = createElement( "parEntities" );
	appendChild(mParEntities);

	// Element to store character entities
	mCharEntities = createElement( "charEntities" );
	appendChild(mCharEntities);

	// Element to store character entities
	mElements = createElement( "elements" );
	appendChild(mElements);
	
	// Element to store attribute lists.
	mAttrLists = createElement( "attrLists" );
	appendChild(mAttrLists);

}

TElementDeclarationPtr	TSchema	::	createElementDeclaration()	{

	TDocumentPtr thisDocument = shared_static_cast<TDocument>( shared_from_this() );
	TElementDeclarationPtr result( new TElementDeclaration( thisDocument ) );

	return result;

}

TSchemaRulePtr TSchema :: createSchemaRule(const TDOMString aTagName)	{

	TDocumentPtr thisDocument = shared_static_cast<TDocument>( shared_from_this() );
	TSchemaRulePtr result(new TSchemaRule(thisDocument, aTagName));

	return result;

}

TElementPtr	TSchema	::	getElements() {

	return mElements;
}

TElementPtr	TSchema	::	getAttrLists() {

	return mAttrLists;
}

TElementPtr	TSchema	::	getCharEntities() {

	return mCharEntities;
}

TElementPtr	TSchema	::	getParEntities() {

	return mParEntities;
}

void TSchema :: addEntity(const TDOMString & aName, const Position & aPosition) {

	mEntityTexts.insert(
		map<string, Position>::value_type( aName, aPosition ) );
	
}

Position TSchema :: getEntityPosition(const TDOMString & aName) {

	return mEntityTexts[ aName ];

}

void TSchema	::	computeEmpty()	{

	printf("Computing empty set\n");
	
	TNodeListPtr declarations = mElements->getChildNodes();

	unsigned int length = declarations->getLength();	
	for (unsigned int i = 0; i < length; i++)	{
		TNodePtr declarationNode = declarations->item(i);
		TElementDeclarationPtr declaration = shared_static_cast<TElementDeclaration>(declarationNode);
		declaration->computeEmpty();
	}
	
}

void TSchema	::	computeFirst()	{

	printf("Computing first set\n");
	
	bool changed = true;
	
	TNodeListPtr declarations = mElements->getChildNodes();
	unsigned int length = declarations->getLength();
	while (changed) {
		printf("Doing a compute first round...\n");
		clock_t start = clock();
		changed = false;
		for (unsigned int i = 0; i < length; i++)	{
			TNodePtr declarationNode = declarations->item(i);
			TSchemaRulePtr declaration = shared_static_cast<TSchemaRule>(declarationNode);
			changed |= declaration->computeFirst();
		}
		clock_t end = clock();
		printf("Time taken for round: %f\n", (double)(end - start)/CLOCKS_PER_SEC);
	}
}

void TSchema	::	computeLA()	{

	printf("Computing Lookahead set\n");
	
	computeEmpty();
	computeFirst();
	
	TNodeListPtr declarations = mElements->getChildNodes();

	unsigned int length = declarations->getLength();	
	for (unsigned int i = 0; i < length; i++)	{
		TNodePtr declarationNode = declarations->item(i);
		TSchemaRulePtr declaration = shared_static_cast<TSchemaRule>(declarationNode);
		declaration->computeLA();
	}
		
}

void TSchema :: computeSchema() {
	
	printf("Computing Schema\n");
	clock_t start = clock();
	computeEmpty();
	clock_t end = clock();
	printf("Time taken: %f\n", (double)(end - start)/CLOCKS_PER_SEC);
	start = clock();
	computeFirst();
	end = clock();
	printf("Time taken: %f\n", (double)(end - start)/CLOCKS_PER_SEC);

}

TElementDeclarationPtr TSchema	::	getDeclaration(const TDOMString & aName) {

	TNodeListPtr declarations = mElements->getChildNodes();

	unsigned int length = declarations->getLength();
	unsigned int i = 0;
	bool foundDeclaration = false;
	TElementDeclarationPtr declaration;
	while (i < length && !foundDeclaration) {
		TNodePtr declarationNode = declarations->item(i);
		declaration = shared_static_cast<TElementDeclaration>(declarationNode);
		if (declaration->hasRule(aName)) {
			foundDeclaration = true;
		}
		i++;
	}

	if (!foundDeclaration) {
		throw ElementDeclException();
	}
	
	return declaration;
	
}

bool TSchema	::	hasDeclaration(const TDOMString & aName) {

	TNodeListPtr declarations = mElements->getChildNodes();

	unsigned int length = declarations->getLength();
	unsigned int i = 0;
	bool foundDeclaration = false;
	TElementDeclarationPtr declaration;
	while (i < length && !foundDeclaration) {
		TNodePtr declarationNode = declarations->item(i);
		declaration = shared_static_cast<TElementDeclaration>(declarationNode);
		if (declaration->hasRule(aName)) {
			foundDeclaration = true;
		}
		i++;
	}

	return foundDeclaration;
	
}
