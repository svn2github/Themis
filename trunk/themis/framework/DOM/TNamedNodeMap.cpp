/* NamedNodeMap implementation
	See TNamedNodeMap.h for some more information
*/

#include <string>

#include "TNamedNodeMap.h"
#include "TDOMException.h"
#include "TNode.h"
#include "TAttr.h"
#include "TElement.h"

TNamedNodeMap	::	TNamedNodeMap( vector<TNodeShared> * aNodeList, TNodeWeak aMappedNode )	{
	
	mNodeList = aNodeList;
	mMappedNode = aMappedNode;
	
}

TNamedNodeMap	::	~TNamedNodeMap()	{

}

unsigned long TNamedNodeMap	::	getLength()	{
	
	if ( mNodeList )	{
		return mNodeList->size();
	}
	
	return 0;
	
}


TNodeWeak TNamedNodeMap	::	getNamedItem( const TDOMString aName )	{
	
	int length = getLength();
	if ( length == 0 )	{
		return TNodeWeak();
	}
	else	{		
		for ( int i = 0; i < length; i++ )	{
			TNodeWeak node = (*mNodeList)[ i ];
			if ( node.get()->getNodeName() == aName )	{
				return node;
			}
		}
	}
	
	return TNodeWeak();
}

TNodeWeak TNamedNodeMap	::	setNamedItem( TNodeShared aArg )	{
	
	// Check to see if it can be added to this element
	if ( mMappedNode.get() )	{
		if ( !( mMappedNode.get()->getNodeType() == ELEMENT_NODE && aArg->getNodeType() == ATTRIBUTE_NODE ) )	{
			throw TDOMException( HIERARCHY_REQUEST_ERR );
		}
	}

	TNodeShared tempMappedNode = make_shared( mMappedNode );
	if ( aArg->getNodeType() == ATTRIBUTE_NODE )	{
		TAttrShared arg = shared_static_cast<TAttr>( aArg );
		TNodeShared tempOwner = shared_static_cast<TNode>( make_shared( arg->getOwnerElement() ) );
		if ( arg->getOwnerElement().get() != NULL &&
			 tempOwner !=  tempMappedNode &&
			 mMappedNode.get() )	{
			throw TDOMException( INUSE_ATTRIBUTE_ERR );
		}
		else	{
			// Set the ownerElement of the attribute to the mapped node
			arg->mOwnerElement = shared_static_cast<TElement>( tempMappedNode );
		}			
	}

	TNodeWeak node = getNamedItem( aArg->getNodeName() );
	if ( node.get() == NULL )	{
		mNodeList->push_back( aArg );
	}
	else	{
		removeNamedItem( aArg->getNodeName() );
		setNamedItem( aArg );
	}
	
	return node;
	
}

TNodeShared TNamedNodeMap	::	removeNamedItem( const TDOMString aName )	{
	
	TNodeShared node = getNamedItem( aName );
	vector<TNodeShared>::iterator iter;
	iter = find( mNodeList->begin(), mNodeList->end(), node );
	if ( iter == mNodeList->end() )	{
		throw TDOMException( NOT_FOUND_ERR );
	}
	else	{
		mNodeList->erase( iter );
	}

	return node;

}

TNodeWeak TNamedNodeMap	::	item( unsigned long aIndex )	{
	
	if ( mNodeList )	{
		return (*mNodeList)[ aIndex ];
	}
	
	return TNodeWeak();
	
}

