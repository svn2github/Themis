/* NamedNodeMap implementation
	See TNamedNodeMap.h for some more information
*/

#include "TNamedNodeMap.h"

TNamedNodeMap	::	TNamedNodeMap( void * aNodeList = NULL )	{
	
	mNodeList = (BList *) aNodeList;
	
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
	mNodeList->RemoveItem( node );

	return node;

}

TNode * TNamedNodeMap	::	item( unsigned long aIndex )	{
	
	if ( !mNodeList )	{
		return (TNode *) mNodeList->ItemAt( aIndex );
	}
	
	return NULL;
	
}
