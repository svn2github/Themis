/* Text implementation
	See TText.h for some more information
*/

#include "TText.h"
#include "TDOMException.h"

TText	::	TText( const TDOMString aText )	:	TCharacterData( TEXT_NODE )	{
	
	setData( aText );
	
}

TText	::	~TText()	{
	
}

bool TText	::	getIsWhiteSpaceInElement() const	{
	
	// I think they mean two spaces here, instead of just one.
	if ( getData().FindFirst( "  " )	== B_ERROR )	{
		// No ignorable whitespace
		return false;
	}
	
	return true;
	
}

TDOMString TText	::	getWholeText() const	{
	
	TDOMString wholeText;

	const TNode * startNode = this;
	const TNode * tempNode = this;
	while ( tempNode && tempNode->getNodeType() == TEXT_NODE )	{
		startNode = tempNode;
		tempNode = tempNode->getPreviousSibling().get();
	}
	
	const TNode * endNode = this;
	tempNode = this;
	while ( tempNode && tempNode->getNodeType() == TEXT_NODE )	{
		endNode = tempNode;
		tempNode = tempNode->getNextSibling().get();
	}
	
	const TText * startText = (const TText *) startNode;
	const TText * endText = (const TText *) endNode;
		
	wholeText = startText->getData();
	while ( startText != endText )	{
		startText = (TText *) startText->getNextSibling().get();
		wholeText += startText->getData();
	}
	
	return wholeText;
	
}

TText * TText	::	splitText( const unsigned long aOffset )	{
	
	if ( aOffset > (unsigned long) ( getLength() - 1 ) )	{
		// Offset is larget than the actual length of the text. No go
		// Negative values are impossible and are not checked
		throw TDOMException( INDEX_SIZE_ERR );
	}
	
	 TDOMString resultData = substringData( aOffset, getLength() - aOffset );
	 TText * result = new TText( resultData );
	 deleteData( aOffset, getLength() - aOffset );
	 
	 return result;
	 
}

TText * TText	::	replaceWholeText( const TDOMString aContent )	{
	
	TNodeShared startNode = mThisPointer;
	TNodeShared tempNode = mThisPointer;
	while ( tempNode.get() && tempNode->getNodeType() == TEXT_NODE )	{
		startNode = tempNode;
		tempNode = make_shared( tempNode->getPreviousSibling() );
	}
	
	TNodeShared endNode = mThisPointer;
	tempNode = mThisPointer;
	while ( tempNode.get() && tempNode->getNodeType() == TEXT_NODE )	{
		endNode = tempNode;
		tempNode = make_shared( tempNode->getNextSibling() );
	}
	
	TDOMString currentText = getWholeText();
	TNodeShared parentNode = make_shared( getParentNode() );

	while ( startNode != endNode )	{
		tempNode = startNode->getNextSibling();
		if ( startNode.get() != mThisPointer.get() )	{
			parentNode.get()->removeChild( startNode );
			//delete temp;
		}
		startNode = tempNode;
	}
	
	if ( currentText == "" )	{
		// No text at all. Remove this node and return NULL
		delete this;
		return NULL;
	}
	
	setData( aContent );
	
	return this;
	
}
