/* Document implementation
	See TDocument.h for some more information
*/

#include "TDocument.h"
#include "TElement.h"
#include "TText.h"
#include "TAttr.h"
#include "TComment.h"

TDocument	::	TDocument()	:	TNode( DOCUMENT_NODE )	{
	
}

TDocument	::	~TDocument()	{
	
}

TElementShared TDocument	::	createElement( const TDOMString aTagName )	{
	
	TElementShared result( new TElement( aTagName ) );
	result->setSmartPointer( result );
	
	return result;
	
}

TTextShared TDocument	::	createText( const TDOMString aData )	{
	
	TTextShared result( new TText( aData ) );
	result->setSmartPointer( result );
	
	return result;
	
}

TCommentShared TDocument	::	createComment( const TDOMString aData )	{
	
	TCommentShared result( new TComment( aData ) );
	result->setSmartPointer( result );
	
	return result;
	
}

TAttrShared TDocument	::	createAttribute( const TDOMString aName )	{
	
	TAttrShared result( new TAttr( aName ) );
	result->setSmartPointer( result );
	
	return result;
	
}

