/* NamedNodeMap implementation
	See TNamedNodeMap.h for some more information
*/

#include "TNamedNodeMap.h"
#include "TDOMException.h"
#include "TNode.h"
#include "TAttr.h"

TNamedNodeMap	::	TNamedNodeMap( void * aNodeList = NULL, const TNode * aMappedNode = NULL )	{
	
	mNodeList = (BList *) aNodeList;
	mMappedNode = aMappedNode;
	
}

TNamedNodeMap	::	~TNamedNodeMap()	{

}

unsigned long TNamedNodeMap	::	getLength()	{
	
	if ( mNodeList )	{
		return mNodeList->CountItems();
	}
	
	return 0;
	
}


TNode * TNamedNodeMap	::	getNamedItem( const TDOMString aName )	{
	
	int length = getLength();
	if ( length == 0 )	{
		return NULL;
	}
	else	{		
		for ( int i = 0; i < length; i++ )	{
			TNode * node = (TNode *) mNodeList->ItemAt( i );
			if ( *( node->getNodeName() ) == aName )	{
				return node;
			}
		}
	}
	
	return NULL;
}

TNode * TNamedNodeMap	::	setNamedItem( TNode * aArg )	{
	
	// Check to see if it can be added to this element
	if ( aArg->getNodeType() == ATTRIBUTE_NODE )	{
		TAttr * arg = (TAttr *) aArg;
		if ( arg->getOwnerElement() != NULL && arg->getOwnerElement() != (const TElement *) mMappedNode && mMappedNode )	{
			printf( "Err, oops\n" );
			throw TDOMException( INUSE_ATTRIBUTE_ERR );
		}
	}

	if ( mMappedNode )	{
		if ( !( mMappedNode->getNodeType() == ELEMENT_NODE && aArg->getNodeType() == ATTRIBUTE_NODE ) )	{
			throw TDOMException( HIERARCHY_REQUEST_ERR );
		}
	}

	TNode * node = getNamedItem( *( aArg->getNodeName() ) );
	if ( !node )	{
		mNodeList->AddItem( (void *) aArg );
	}
	else	{
		removeNamedItem( *( aArg->getNodeName() ) );
		setNamedItem( aArg );
	}
	
	return node;
	
}

TNode * TNamedNodeMap	::	removeNamedItem( const TDOMString aName )	{
	
	TNode * node = getNamedItem( aName );
	if ( !( mNodeList->RemoveItem( node ) ) )	{
		throw TDOMException( NOT_FOUND_ERR );
	}

	return node;

}

TNode * TNamedNodeMap	::	item( unsigned long aIndex )	{
	
	if ( !mNodeList )	{
		return (TNode *) mNodeList->ItemAt( aIndex );
	}
	
	return NULL;
	
}

