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
	Class Start Date: January 15, 2011
*/

/*	DOMEntry implementation
	See DOMEntry.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// DOM headers
#include "TDocument.h"

// SGMLParser headers
#include "DOMEntry.hpp"

// Constants declared
const string kDOMDocument = "DOM Document";

DOMEntry :: DOMEntry(int32 aId, TDocumentPtr aDocument)
		 : BaseEntry(aId) {

	set(kDOMDocument, shared_static_cast<void>(aDocument));

}

TDocumentPtr DOMEntry :: getDocument() {
	
	return shared_static_cast<TDocument>(getSharedPtr(kDOMDocument));
}
