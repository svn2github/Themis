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
	Class Start Date: April 01, 2002
*/

/*
	Text implementation
	See TText.h for some more information
*/

// Standard C++ headers
#include <string>

// DOM headers
#include "TText.h"
#include "TDocument.h"
#include "TDOMException.h"

// Namespaces used
using namespace std;

TText	::	TText( const TDocumentPtr aOwnerDocument,
						  const TDOMString aText )	:
						  		TCharacterData( TEXT_NODE, aOwnerDocument, aText )	{
	
}

TText	::	~TText()	{
	
}

bool TText	::	isWhiteSpaceInElement() const	{
	
	// I think they mean two spaces here, instead of just one.
	if ( getData().find( "  " ) == string::npos )	{
		// No ignorable whitespace
		return false;
	}
	
	return true;
	
}

TDOMString TText	::	getWholeText()	{
	
	TDOMString wholeText;

	TNodePtr startNode = shared_from_this();
	TNodePtr tempNode = shared_from_this();
	while ( tempNode.get() && tempNode->getNodeType() == TEXT_NODE )	{
		startNode = tempNode;
		tempNode = tempNode->getPreviousSibling();
	}

	TNodePtr endNode = shared_from_this();
	tempNode = shared_from_this();

	while ( tempNode.get() && tempNode->getNodeType() == TEXT_NODE )	{
		endNode = tempNode;
		tempNode = tempNode->getNextSibling();
	}

	TTextPtr startText = shared_static_cast<TText> ( startNode );
	TTextPtr endText = shared_static_cast<TText> ( endNode );
		
	wholeText = startText->getData();
	while ( startText != endText )	{
		startText = shared_static_cast<TText> ( startText->getNextSibling() );
		wholeText += startText->getData();
	}
	
	return wholeText;
	
}

TTextPtr TText	::	splitText( const unsigned long aOffset )	{
	
	if ( aOffset > (unsigned long) ( getLength() - 1 ) )	{
		// Offset is larget than the actual length of the text. No go
		// Negative values are impossible and are not checked
		throw TDOMException( INDEX_SIZE_ERR );
	}
	
	TDOMString resultData = substringData( aOffset, getLength() - aOffset );
	TTextPtr result = getOwnerDocument()->createText( resultData );
	getParentNode()->insertBefore( result, getNextSibling() );
	
	deleteData( aOffset, getLength() - aOffset );
	 
	return result;
	 
}

TTextPtr TText	::	replaceWholeText( const TDOMString aContent )	{
	
	TNodePtr startNode = shared_from_this();
	TNodePtr tempNode = shared_from_this();
	while ( tempNode.get() && tempNode->getNodeType() == TEXT_NODE )	{
		startNode = tempNode;
		tempNode = tempNode->getPreviousSibling();
	}
	
	TNodePtr endNode = shared_from_this();
	tempNode = shared_from_this();
	while ( tempNode.get() && tempNode->getNodeType() == TEXT_NODE )	{
		endNode = tempNode;
		tempNode = tempNode->getNextSibling();
	}
	
	TDOMString currentText = getWholeText();
	TNodePtr parentNode = getParentNode();

	while ( startNode != endNode )	{
		tempNode = startNode->getNextSibling();
		if ( startNode.get() != this )	{
			parentNode->removeChild( startNode );
			//delete temp;
		}
		startNode = tempNode;
	}
	
	if ( currentText == "" )	{
		// No text at all. Remove this node and return NULL
		TNodePtr parent = getParentNode();
		parent->removeChild( shared_from_this() );
		return TTextPtr();
	}
	
	setData( aContent );
	
	return shared_static_cast<TText> ( shared_from_this() );
	
}
