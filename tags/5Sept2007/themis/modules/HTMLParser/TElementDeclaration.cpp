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
	Class Start Date: June 05, 2006
*/

/*
	ElementDeclaration implementation
	See TElementDeclaration.hpp for some more information
*/

// SGMLParser headers
#include "TElementDeclaration.hpp"

TElementDeclaration::	TElementDeclaration( const TDocumentPtr aOwnerDocument )
	:	TElement(aOwnerDocument, "declaration" )	{
	
}

TElementDeclaration	::	~TElementDeclaration()	{
	
}

void TElementDeclaration	::	setMinimization( bool aStart, bool aEnd )	{
	
	if ( aStart ) {
		setAttribute( "start", "true" );
	}
	else {
		setAttribute( "start", "false" );
	}
	if ( aEnd ) {
		setAttribute( "end", "true" );
	}
	else {
		setAttribute( "end", "false" );
	}

}

void TElementDeclaration	::	getMinimization( bool & aStart, bool & aEnd )	{
	
	if ( getAttribute( "start" ) == "true" )	{
		aStart = true;
	}
	else	{
		aStart = false;
	}

	if ( getAttribute( "end" ) == "true" )	{
		aEnd = true;
	}
	else	{
		aEnd = false;
	}
	
}
