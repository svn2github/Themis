/*
	Copyright (c) 2002 Mark Hellegers. All Rights Reserved.
	
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
	
	Original Author: 	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	Project Start Date: October 18, 2000
	Class Start Date: February 03, 2002
*/

/*
	Document implementation
	See TDocument.h for some more information
*/

// DOM headers
#include "TDocument.h"
#include "TElement.h"
#include "TText.h"
#include "TAttr.h"
#include "TComment.h"

TDocument	::	TDocument()	:	TNode( DOCUMENT_NODE, TDocumentPtr() )	{
	
}

TDocument	::	~TDocument()	{
	
}

TElementPtr TDocument	::	createElement( const TDOMString aTagName )	{
	
	TDocumentPtr thisDocument = shared_static_cast<TDocument>( shared_from_this() );
	TElementPtr result( new TElement( thisDocument, aTagName ) );
	
	return result;
	
}

TTextPtr TDocument	::	createText( const TDOMString aData )	{
	
	TDocumentPtr thisDocument = shared_static_cast<TDocument>( shared_from_this() );
	TTextPtr result( new TText( thisDocument, aData ) );
	
	return result;
	
}

TCommentPtr TDocument	::	createComment( const TDOMString aData )	{
	
	TDocumentPtr thisDocument = shared_static_cast<TDocument>( shared_from_this() );
	TCommentPtr result( new TComment( thisDocument, aData ) );
	
	return result;
	
}

TAttrPtr TDocument	::	createAttribute( const TDOMString aName )	{
	
	TDocumentPtr thisDocument = shared_static_cast<TDocument>( shared_from_this() );
	TAttrPtr result( new TAttr( aName, thisDocument ) );
	
	return result;
	
}

void TDocument	::	setDocumentURI( const TDOMString aURI )	{
	
	mDocumentURI = aURI;
	
}

TDOMString TDocument	::	getDocumentURI() const	{
	
	return mDocumentURI;
	
}
