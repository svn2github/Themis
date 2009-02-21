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

/*	TSchema
	Stores an XML Schema (DTD).

	Mark Hellegers (mark@firedisk.net)
	12-06-2006
*/

#ifndef TSCHEMA_HPP
#define TSCHEMA_HPP

// Standard C++ headers
#include <string>
#include <map>

// DOM headers
#include "TDocument.h"
#include "TElement.h"

// SGMLParser headers
#include "SGMLSupport.hpp"
#include "Position.hpp"

// Namespaces used
using std::map;
using std::string;

/// Class to store a schema.

/**
	This class stores an XML schema.
	It is based on a DOM document and provides
	a few functions to make it easy to parse an SGML document.
	It currently only directly supports a DTD schema.
*/

class TSchema	:	public TDocument	{

	private:
		TElementPtr mElements;
		TElementPtr mAttrLists;
		TElementPtr mCharEntities;
		TElementPtr mParEntities;
		map<string, Position> mEntityTexts;

	public:
		TSchema();
		~TSchema();
		
		void setup();
		TElementDeclarationPtr createElementDeclaration();
		TSchemaRulePtr createSchemaRule(const TDOMString aTagName);
		TElementPtr getElements();
		TElementPtr	getAttrLists();
		TElementPtr getCharEntities();
		TElementPtr getParEntities();
		void addEntity(const TDOMString & aName, const Position & aPosition);
		Position getEntityPosition(const TDOMString & aName);
		void computeEmpty();
		void computeFirst();
		void computeLA();
		void computeSchema();
		TElementDeclarationPtr getDeclaration(const TDOMString & aName);
		bool hasDeclaration(const TDOMString & aName);
	
};

#endif
