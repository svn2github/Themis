/*
	Copyright (c) 2011 Mark Hellegers. All Rights Reserved.
	
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
	Class Start Date: April 10, 2011
*/

/*	CSSStyleContainer
	Contains the CSS Stylesheets for a document
	and provides a function to compute the style for an element of the document
	
	Mark Hellegers (mark@firedisk.net)
	10-04-2011
	
*/

#ifndef CSSSTYLECONTAINER_HPP
#define CSSSTYLECONTAINER_HPP

// Standard C++ headers
#include <vector>

// DOM headers
#include "TElement.h"

// DOM Style headers
#include "CSSStyleSheet.hpp"
#include "CSSStyleDeclaration.hpp"

// Namespaces used
using std::vector;

class CSSStyleContainer {
	
	private:
		vector<CSSStyleSheetPtr> mStyleSheetList;
	
	public:
		CSSStyleContainer();
		~CSSStyleContainer();
		void addStyleSheet(const CSSStyleSheetPtr aStyleSheet);

		/// A function to compute the style based on the stylesheets in the list.
		/**
			This functions computes the style for a given element by looking in the list
			of stylesheets to see what applies to the element.
		*/
		CSSStyleDeclarationPtr getComputedStyle(const TElementPtr aElement);
};

#endif
