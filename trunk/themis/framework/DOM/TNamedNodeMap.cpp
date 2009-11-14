/*
	Copyright (c) 2002 Mark Hellegers. All Rights Reserved.
	
	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or
	sell copies of the Software, and to permit persons to whom
	the Software is furnished to do so, subject to the following
	conditions:
	
	   The above copyright notice and this permission notice
	   shall be included in all copies or substantial portions
	   of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
	KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
	WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
	OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
	OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
	OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
	
	Original Author: 	Mark Hellegers (mark@firedisk.net)
	Project Start Date: October 18, 2000
	Class Start Date: February 03, 2002
*/

/*
	NamedNodeMap implementation
	See TNamedNodeMap.h for some more information
*/

// Standard C++ headers
#include <string>

// DOM headers
#include "TNamedNodeMap.h"
#include "TDOMException.h"
#include "TNode.h"
#include "TAttr.h"
#include "TElement.h"

TNamedNodeMap	::	TNamedNodeMap( vector<TNodePtr> * aNodeList, TNodePtr aMappedNode )	{
	
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


TNodePtr TNamedNodeMap	::	getNamedItem( const TDOMString aName )	{
	
	int length = getLength();
	if ( length == 0 )	{
		return TNodePtr();
	}
	else	{		
		for ( int i = 0; i < length; i++ )	{
			TNodePtr node = (*mNodeList)[ i ];
			if ( node->getNodeName() == aName )	{
				return node;
			}
		}
	}
	
	return TNodePtr();
}

TNodePtr TNamedNodeMap	::	setNamedItem( TNodePtr aArg )	{
	
	// Check to see if it can be added to this element
	TNodePtr mappedNode = make_shared( mMappedNode );
	if ( mappedNode.get() != NULL )	{
		if ( ! ( mappedNode->getNodeType() == ELEMENT_NODE && aArg->getNodeType() == ATTRIBUTE_NODE ) )	{
			throw TDOMException( HIERARCHY_REQUEST_ERR );
		}
	}

	if ( aArg->getNodeType() == ATTRIBUTE_NODE )	{
		TAttrPtr arg = shared_static_cast<TAttr>( aArg );
		TNodePtr owner = shared_static_cast<TNode>( arg->getOwnerElement() );
		if ( arg->getOwnerElement().get() != NULL &&
			 owner != mappedNode &&
			 mappedNode.get() != NULL )	{
			throw TDOMException( INUSE_ATTRIBUTE_ERR );
		}
		else	{
			// Set the ownerElement of the attribute to the mapped node
			arg->mOwnerElement = shared_static_cast<TElement>( mappedNode );
		}			
	}

	TNodePtr node = getNamedItem( aArg->getNodeName() );
	if ( node.get() == NULL )	{
		mNodeList->push_back( aArg );
	}
	else	{
		removeNamedItem( aArg->getNodeName() );
		setNamedItem( aArg );
	}
	
	return node;
	
}

TNodePtr TNamedNodeMap	::	removeNamedItem( const TDOMString aName )	{
	
	TNodePtr node = getNamedItem( aName );
	vector<TNodePtr>::iterator iter;
	iter = find( mNodeList->begin(), mNodeList->end(), node );
	if ( iter == mNodeList->end() )	{
		throw TDOMException( NOT_FOUND_ERR );
	}
	else	{
		mNodeList->erase( iter );
	}

	return node;

}

TNodePtr TNamedNodeMap	::	item( unsigned long aIndex )	{
	
	if ( mNodeList && aIndex < mNodeList->size() )	{
		return (*mNodeList)[ aIndex ];
	}
	
	return TNodePtr();
	
}

