/* NodeListContainer implementation
	See TNodeListContainer.h for some more information
*/

#include "TNodeListContainer.h"
#include <List.h>
#include "TNode.h"
#include "TNodeList.h"

TNodeListContainer	::	TNodeListContainer( const TDOMString aQueryString, BList * aNodes, BList * aNodeLists, unsigned short aNodeType )	{

	mNodes = aNodes;
	mNodeLists = aNodeLists;
	mQueryString = aQueryString;
	mNodeType = aNodeType;
	
}

TNodeListContainer	::	~TNodeListContainer()	{

	delete mNodes;
	delete mNodeLists;

}

TDOMString TNodeListContainer	::	getQueryString() const	{
	
	return mQueryString;
	
}

unsigned short TNodeListContainer	::	getNodeType() const	{
	
	return mNodeType;
	
}

void TNodeListContainer	::	addNode( TNode * aNode )	{
	
	if ( !mNodes->HasItem( aNode ) )	{
		mNodes->AddItem( aNode );
	}
	
}

TNode * TNodeListContainer	::	removeNode( TNode * aNode )	{
	
	mNodes->RemoveItem( aNode );
	return aNode;

}

TNodeList * TNodeListContainer	::	addNodeList()	{
	
	TNodeList * nodeList = new TNodeList( mNodes, this );
	mNodeLists->AddItem( nodeList );
	return nodeList;
	
}

TNodeList * TNodeListContainer	::	removeNodeList( TNodeList * aNodeList )	{
	
	mNodeLists->RemoveItem( aNodeList );
	if ( mNodeLists->IsEmpty() )	{
		delete this;
	}

	return aNodeList;
	
}	
