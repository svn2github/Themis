/* NodeList implementation
	See TNodeList.h for some more information
*/

#include "TNodeList.h"
#include "TNode.h"
#include "TNodeListContainer.h"

#include <vector>

TNodeList	::	TNodeList( const vector<TNodeShared> * aNodeList )	{
	
	mNodeList = aNodeList;
	
}

TNodeList	::	~TNodeList()	{

}

unsigned long TNodeList	::	getLength()	{
	
	if ( mNodeList )	{
		return mNodeList->size();
	}
	
	return 0;
	
}

TNodeWeak TNodeList	::	item( unsigned long aIndex )	{
	
	if ( mNodeList )	{
		return (*mNodeList)[ aIndex ];
	}
	
	return TNodeWeak();
	
}
