/* NodeListContainer implementation
	See TNodeListContainer.h for some more information
*/

#include <string>

#include "TNodeListContainer.h"
#include "TNode.h"
#include "TNodeList.h"

TNodeListContainer	::	TNodeListContainer( const TDOMString aQueryString, vector<TNodeShared> * aNodes, unsigned short aNodeType )	{

	mNodes = aNodes;
	mNodeList = TNodeListShared( new TNodeList( mNodes ) );
	mQueryString = aQueryString;
	mNodeType = aNodeType;
	
}

TNodeListContainer	::	~TNodeListContainer()	{

	delete mNodes;

}

TDOMString TNodeListContainer	::	getQueryString() const	{
	
	return mQueryString;
	
}

unsigned short TNodeListContainer	::	getNodeType() const	{
	
	return mNodeType;
	
}

void TNodeListContainer	::	addNode( TNodeShared aNode )	{

	vector<TNodeShared>::iterator iter;
	iter = find( mNodes->begin(), mNodes->end(), aNode );
	if ( iter != mNodes->end() )	{
		mNodes->push_back( aNode );
	}
	
}

TNodeWeak TNodeListContainer	::	removeNode( TNodeShared aNode )	{
	
	vector<TNodeShared>::iterator iter;
	iter = find( mNodes->begin(), mNodes->end(), aNode );
	if ( iter != mNodes->end() )	{
		mNodes->erase( iter );
	}

	return aNode;

}

TNodeListShared TNodeListContainer	::	getNodeList()	{
	
	return mNodeList;
	
}
