/* NodeList implementation
	See TNodeList.h for some more information
*/

#include "TNodeList.h"

TNodeList	::	TNodeList( const void * aNodeList = NULL )	{
	
	mNodeList = (BList *) aNodeList;
	
}

TNodeList	::	~TNodeList()	{

}

unsigned long TNodeList	::	getLength()	{
	
	if ( !mNodeList )	{
		return mNodeList->CountItems();
	}
	
	return 0;
	
}

TNode * TNodeList	::	item( unsigned long aIndex )	{
	
	if ( !mNodeList )	{
		return (TNode *) mNodeList->ItemAt( aIndex );
	}
	
	return NULL;
	
}
