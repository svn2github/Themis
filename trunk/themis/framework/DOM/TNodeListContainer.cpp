/* NodeListContainer implementation
	See TNodeListContainer.h for some more information
*/

#include "TNodeListContainer.h"
#include <List.h>
#include "TNode.h"

TNodeListContainer	::	TNodeListContainer( const TDOMString aQueryString )	{

	mNodes = new BList();
	mNodeLists = new BList();
	mQueryString = aQueryString;
	
}

TNodeListContainer	::	~TNodeListContainer()	{

	delete mNodes;
	delete mNodeLists;

}

TDOMString TNodeListContainer	::	getQueryString()	{
	
	return mQueryString;
	
}

void TNodeListContainer	::	addNode( TNode * aNode )	{
	
	if ( !mNodes->HasItem( aNode ) )	{
		mNodes->AddItem( aNode );
	}
	
}

TNode * TNodeListContainer	::	removeNode( TNode * aNode )	{
	
	if ( mNodes->RemoveItem( aNode ) )	{
		return aNode;
	}

	return NULL;
	
}

void TNodeListContainer	::	addNodeList( TNodeList * aNodeList )	{
	
	if ( !mNodeLists->HasItem( aNodeList ) )	{
		mNodeLists->AddItem( aNodeList );
	}
	
}

TNodeList * TNodeListContainer	::	removeNodeList( TNodeList * aNodeList )	{
	
	if ( mNodeLists->RemoveItem( aNodeList ) )	{
		return aNodeList;
	}
	
	return NULL;
	
}	
