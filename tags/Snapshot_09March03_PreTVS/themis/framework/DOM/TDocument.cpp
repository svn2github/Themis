/* Document implementation
	See TDocument.h for some more information
*/

#include "TDocument.h"
#include "TElement.h"
#include "TText.h"
#include "TAttr.h"
#include "TComment.h"

TDocument	::	TDocument()	:	TNode( DOCUMENT_NODE, TDocumentWeak() )	{
	
}

TDocument	::	~TDocument()	{
	
}

TElementShared TDocument	::	createElement( const TDOMString aTagName )	{
	
	TDocumentWeak thisDocument = shared_static_cast<TDocument>( make_shared( mThisPointer ) );
	TElementShared result( new TElement( thisDocument, aTagName ) );
	result->setSmartPointer( result );
	
	return result;
	
}

TTextShared TDocument	::	createText( const TDOMString aData )	{
	
	TDocumentWeak thisDocument = shared_static_cast<TDocument>( make_shared( mThisPointer ) );
	TTextShared result( new TText( thisDocument, aData ) );
	result->setSmartPointer( result );
	
	return result;
	
}

TCommentShared TDocument	::	createComment( const TDOMString aData )	{
	
	TDocumentWeak thisDocument = shared_static_cast<TDocument>( make_shared( mThisPointer ) );
	TCommentShared result( new TComment( thisDocument, aData ) );
	result->setSmartPointer( result );
	
	return result;
	
}

TAttrShared TDocument	::	createAttribute( const TDOMString aName )	{
	
	TDocumentWeak thisDocument = shared_static_cast<TDocument>( make_shared( mThisPointer ) );
	TAttrShared result( new TAttr( aName, thisDocument ) );
	result->setSmartPointer( result );
	
	return result;
	
}

