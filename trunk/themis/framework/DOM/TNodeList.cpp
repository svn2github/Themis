/* NodeList implementation
	See TNodeList.h for some more information
*/

#include "TNodeList.h"
#include "TNode.h"
#include "TNodeListContainer.h"

TNodeList	::	TNodeList( const void * aNodeList = NULL, TNodeListContainer * aContainer = NULL )	{
	
	mNodeList = (BList *) aNodeList;
	mContainer = aContainer;
	
}

TNodeList	::	~TNodeList()	{

	if ( mContainer )	{
		mContainer->removeNodeList( this );
	}

}

unsigned long TNodeList	::	getLength()	{
	
	if ( mNodeList )	{
		return mNodeList->CountItems();
	}
	
	return 0;
	
}

TNode * TNodeList	::	item( unsigned long aIndex )	{
	
	if ( mNodeList )	{
		return (TNode *) mNodeList->ItemAt( aIndex );
	}
	
	return NULL;
	
}
