/* Text implementation
	See TText.h for some more information
*/

#include <string>

#include "TText.h"
#include "TDOMException.h"

TText	::	TText( const TDocumentWeak aOwnerDocument,
						  const TDOMString aText )	:
						  		TCharacterData( TEXT_NODE, aOwnerDocument )	{
	
	setData( aText );
	
}

TText	::	~TText()	{
	
}

bool TText	::	getIsWhiteSpaceInElement() const	{
	
	// I think they mean two spaces here, instead of just one.
	if ( getData().find( "  " )	== string::npos )	{
		// No ignorable whitespace
		return false;
	}
	
	return true;
	
}

TDOMString TText	::	getWholeText() const	{
	
	TDOMString wholeText;

	TNodeShared startNode = make_shared( mThisPointer );
	TNodeShared tempNode = make_shared( mThisPointer );
	while ( tempNode.get() && tempNode->getNodeType() == TEXT_NODE )	{
		startNode = tempNode;
		tempNode = make_shared( tempNode->getPreviousSibling() );
	}

	TNodeShared endNode = make_shared( mThisPointer );
	tempNode = make_shared( mThisPointer );

	while ( tempNode.get() && tempNode->getNodeType() == TEXT_NODE )	{
		endNode = tempNode;
		tempNode = make_shared( tempNode->getNextSibling() );
	}

	TTextShared startText = shared_static_cast<TText> ( startNode );
	TTextShared endText = shared_static_cast<TText> ( endNode );
		
	wholeText = startText->getData();
	while ( startText != endText )	{
		startText = shared_static_cast<TText> ( make_shared( startText->getNextSibling() ) );
		wholeText += startText->getData();
	}
	
	return wholeText;
	
}

TTextShared TText	::	splitText( const unsigned long aOffset )	{
	
	if ( aOffset > (unsigned long) ( getLength() - 1 ) )	{
		// Offset is larget than the actual length of the text. No go
		// Negative values are impossible and are not checked
		throw TDOMException( INDEX_SIZE_ERR );
	}
	
	 TDOMString resultData = substringData( aOffset, getLength() - aOffset );
	 TTextShared result = TTextShared( new TText( getOwnerDocument(), resultData ) );
	 result->setSmartPointer( result );
	 deleteData( aOffset, getLength() - aOffset );
	 
	 return result;
	 
}

TTextWeak TText	::	replaceWholeText( const TDOMString aContent )	{
	
	TNodeShared startNode = make_shared( mThisPointer );
	TNodeShared tempNode = make_shared( mThisPointer );
	while ( tempNode.get() && tempNode->getNodeType() == TEXT_NODE )	{
		startNode = tempNode;
		tempNode = make_shared( tempNode->getPreviousSibling() );
	}
	
	TNodeShared endNode = make_shared( mThisPointer );
	tempNode = make_shared( mThisPointer );
	while ( tempNode.get() && tempNode->getNodeType() == TEXT_NODE )	{
		endNode = tempNode;
		tempNode = make_shared( tempNode->getNextSibling() );
	}
	
	TDOMString currentText = getWholeText();
	TNodeShared parentNode = make_shared( getParentNode() );

	while ( startNode != endNode )	{
		tempNode = make_shared( startNode->getNextSibling() );
		if ( startNode.get() != mThisPointer.get() )	{
			parentNode.get()->removeChild( startNode );
			//delete temp;
		}
		startNode = tempNode;
	}
	
	if ( currentText == "" )	{
		// No text at all. Remove this node and return NULL
		TNodeShared parent = make_shared( getParentNode() );
		parent->removeChild( make_shared( mThisPointer ) );
		return TTextWeak();
	}
	
	setData( aContent );
	
	return shared_static_cast<TText> ( make_shared( mThisPointer ) );
	
}
