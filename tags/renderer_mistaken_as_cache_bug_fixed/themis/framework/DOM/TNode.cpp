/*
	Copyright (c) 2001 Mark Hellegers. All Rights Reserved.
	
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
	
	Original Author: 	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	Project Start Date: October 18, 2000
	Class Start Date: December 11, 2001
*/

/*
	TNode implementation
	See TNode.h for some more information
*/

// Standard C headers
#include <stdio.h>

// Standard C++ headers
#include <string>

// DOM headers
#include "TNode.h"
#include "TNamedNodeMap.h"
#include "TNodeList.h"
#include "TNodeListContainer.h"
#include "TDOMException.h"

TNode	::	TNode( const unsigned short aNodeType,
						   const TDocumentPtr aOwnerDocument,
						   const TDOMString aNodeName,
						   const TDOMString aNodeValue )	{

	mNodeType = aNodeType;
	mOwnerDocument = aOwnerDocument;

	mParentNode.reset();
	//mThisPointer.reset();
	mNextSibling.reset();
	mPreviousSibling.reset();
	
	mNodeList = vector<TNodePtr>();
	mChildNodes = TNodeListPtr( new TNodeList( &mNodeList ) );
	mNodeListContainers = vector<TNodeListContainerPtr>();

	switch ( mNodeType )	{
		case ELEMENT_NODE:	{
			mNodeName = aNodeName;
			mNodeTypeString = "ELEMENT_NODE";
			mAttributeList = vector<TNodePtr>();
			// Can't do this here. Do it where it is accessed. Not ideal, I know
			//mAttributes = TNamedNodeMapPtr( new TNamedNodeMap( &mAttributeList, shared_from_this() ) );
			break;
		}
		case ATTRIBUTE_NODE:	{
			mNodeName = aNodeName;
			mNodeValue = aNodeValue;
			mNodeTypeString = "ATTRIBUTE_NODE";
			break;
		}
		case TEXT_NODE:	{
			mNodeName = "#text";
			mNodeValue = aNodeValue;
			mNodeTypeString = "TEXT_NODE";
			break;
		}
		case CDATA_SECTION_NODE:	{
			mNodeName = "cdata-section";
			mNodeValue = aNodeValue;
			mNodeTypeString = "CDATA_SECTION_NODE";
			break;
		}
		case ENTITY_REFERENCE_NODE:	{
			mNodeTypeString = "ENTITY_REFERENCE_NODE";
			break;
		}
		case ENTITY_NODE:	{
			mNodeTypeString = "ENTITY_NODE";
			break;
		}
		case PROCESSING_INSTRUCTION_NODE:	{
			mNodeName = aNodeValue;
			mNodeTypeString = "PROCESSSING_INSTRUCTION_NODE";
			break;
		}
		case COMMENT_NODE:	{
			mNodeName = "#comment";
			mNodeValue = aNodeValue;
			mNodeTypeString = "COMMENT_NODE";
			break;
		}
		case DOCUMENT_NODE:	{
			mNodeName = "#document";
			mNodeTypeString = "DOCUMENT_NODE";
			break;
		}
		case DOCUMENT_TYPE_NODE:	{
			mNodeTypeString = "DOCUMENT_TYPE_NODE";
			break;
		}
		case DOCUMENT_FRAGMENT_NODE:	{
			mNodeName = "#document-fragment";
			mNodeTypeString = "DOCUMENT_FRAGMENT_NODE";
			break;
		}
		case NOTATION_NODE:	{
			mNodeTypeString = "NOTATION_NODE";
			break;
		}
		default:	{
			mNodeTypeString = "Undefined node";
			break;
		}
	}
	
}

TNode	::	~TNode()	{

	TNodePtr parent = make_shared( mParentNode );
	if ( parent.get() != NULL )	{
		// Still attached to the tree
		// Hmm, watch this code. Seems odd
		parent->removeChild( shared_from_this() );
	}

	if ( mNodeType == ELEMENT_NODE )	{
		// Has a NodeMap
		mAttributes.reset();
	}

	// Remove all items that are attached to this node
	for ( int i = mNodeList.size() - 1; i >= 0; i-- )	{
		TNodePtr item = mNodeList[ i ];
		removeChild( item );
	}
	
}

unsigned short TNode	::	getNodeType() const	{
	
	return mNodeType;
	
}

const TDOMString TNode	::	getNodeName() const	{
	
	return mNodeName;
	
}

const TDOMString TNode	::	getNodeValue() const	{
	
	return mNodeValue;
	
}

void TNode	::	setNodeValue( const TDOMString aNodeValue )	{
	
	switch ( mNodeType )	{
		case ATTRIBUTE_NODE:
		case CDATA_SECTION_NODE:
		case COMMENT_NODE:
		case PROCESSING_INSTRUCTION_NODE:
		case TEXT_NODE:	{
			// Node can have a value
			mNodeValue = aNodeValue;
			break;
		}
	}
	
}

TNodeListPtr TNode	::	getChildNodes() const	{
	
	return mChildNodes;
	
}

TNodePtr TNode	::	getParentNode() const	{
	
	return make_shared( mParentNode );
	
}

TNodePtr TNode	::	getFirstChild() const	{
	
	return mNodeList.front();
	
}

TNodePtr TNode	::	getLastChild() const	{
	
	return mNodeList.back();
	
}

TNodePtr TNode	::	getNextSibling() const	{
	
	return make_shared( mNextSibling );
	
}

TNodePtr TNode	::	getPreviousSibling() const	{
	
	return make_shared( mPreviousSibling );
	
}

TNamedNodeMapPtr TNode	::	getAttributes()	{

	if ( mNodeType == ELEMENT_NODE && mAttributes.get() == NULL )	{
		mAttributes = TNamedNodeMapPtr( new TNamedNodeMap( &mAttributeList, shared_from_this() ) );
	}
	
	return mAttributes;
	
}

TDocumentPtr TNode	::	getOwnerDocument() const	{
	
	return make_shared( mOwnerDocument );
	
}

TNodePtr TNode	::	insertBefore( TNodePtr aNewChild, TNodePtr aRefChild )	{
	
	if ( aNewChild.get() == NULL )	{
		return TNodePtr();
	}

	if ( this == aNewChild.get() || isAncestor( aNewChild ) )	{
		throw TDOMException( HIERARCHY_REQUEST_ERR );
	}

	if ( !isChildAllowed( aNewChild ) )	{
		throw TDOMException( HIERARCHY_REQUEST_ERR );
	}
	
	if ( aRefChild.get() == NULL )	{
		// Just append it to the end of the list of children
		appendChild( aRefChild );
	}
	else	{
		vector<TNodePtr>::iterator iter;
		iter = find( mNodeList.begin(), mNodeList.end(), aRefChild );
		if ( iter != mNodeList.end() )	{
			int j = mNodeList.size();
			for ( int i = 0; i < j; i++ )	{
				if ( mNodeList[ i ] == aRefChild )	{

					// If aNewChild is in the tree already remove it there first.
					TNodePtr parent = aNewChild->getParentNode();
					if ( parent.get() != NULL )	{
						parent->removeChild( aNewChild );
					}

					// Insert aNewChild in the tree and adjust the pointers
					mNodeList.insert( &mNodeList[ i ], aNewChild );
					aNewChild->mParentNode = shared_from_this();
					aNewChild->mPreviousSibling = aRefChild->mPreviousSibling;
					aNewChild->mNextSibling = aRefChild;
					aRefChild->mPreviousSibling = aNewChild;

					// Update the previousSibling's nextSibling
					TNodePtr previous = aNewChild->getPreviousSibling();
					if ( previous.get() != NULL )	{
						previous->mNextSibling = aNewChild;
					}
				}
			}
			notifyNodeChange( aNewChild, NODE_ADDED );
		}
		else	{
			throw TDOMException( NOT_FOUND_ERR );
		}
	}
	
	return aNewChild;
	
}						

TNodePtr TNode	::	replaceChild( TNodePtr aNewChild, TNodePtr aOldChild)	{
	
	if ( aNewChild.get() == NULL )	{
		return TNodePtr();
	}

	if ( this == aNewChild.get() || isAncestor( aNewChild ) )	{
		throw TDOMException( HIERARCHY_REQUEST_ERR );
	}

	if ( !isChildAllowed( aNewChild ) )	{
		throw TDOMException( HIERARCHY_REQUEST_ERR );
	}

	if ( aOldChild.get() == NULL )	{
		// NULL is technically not in the tree
		throw TDOMException( NOT_FOUND_ERR );
	}
	else	{
		vector<TNodePtr>::iterator iter;
		iter = find( mNodeList.begin(), mNodeList.end(), aOldChild );
		if ( iter != mNodeList.end() )	{
			int j = mNodeList.size();
			for ( int i = 0; i < j; i++ )	{
				if ( mNodeList[ i ] == aOldChild )	{
					// If aNewChild is in the tree already remove it there first.
					TNodePtr parent = aNewChild->getParentNode();
					if ( parent.get() != NULL )	{
						parent->removeChild( aNewChild );
					}

					// Remove aOldChild from list and add aNewChild in its place
					mNodeList.erase( &mNodeList[ i ] );
					mNodeList.insert( &mNodeList[ i ], aNewChild );

					// Notify parent that node is removed
					notifyNodeChange( aOldChild, NODE_REMOVED );
					
					// Set aNewChild's parentNode, previous- and nextSibling to aOldChild's values
					aNewChild->mParentNode = shared_from_this();
					aNewChild->mPreviousSibling = aOldChild->mPreviousSibling;
					aNewChild->mNextSibling = aOldChild->mNextSibling;

					// Set aOldChild's values to NULL, so it isn't in the tree anymore.
					aOldChild->mPreviousSibling.reset();
					aOldChild->mNextSibling.reset();
					aOldChild->mParentNode.reset();

					// Set neighbour siblings to point to aNewChild
					TNodePtr previous = aNewChild->getPreviousSibling();
					if ( previous.get() != NULL )	{
						previous->mNextSibling = aNewChild;
					}
					TNodePtr next = aNewChild->getNextSibling();
					if ( next.get() != NULL )	{
						next->mPreviousSibling = aNewChild;
					}
				}
			}
			// Notify parent that node is added
			notifyNodeChange( aNewChild, NODE_ADDED );
		}
		else	{
			throw TDOMException( NOT_FOUND_ERR );
		}
	}
	
	return aOldChild;
	
}

TNodePtr TNode	::	 removeChild( TNodePtr aOldChild )	{
	
	if ( aOldChild.get() == NULL )	{
		// NULL is technically not in the tree
		throw TDOMException( NOT_FOUND_ERR );
	}
	else	{
		vector<TNodePtr>::iterator iter = find( mNodeList.begin(), mNodeList.end(), aOldChild );
		if ( iter != mNodeList.end() )	{
			// Remove node
			mNodeList.erase( iter );
			
			// Link up next and previous siblings
			TNodePtr previous = aOldChild->getPreviousSibling();
			if ( previous.get() != NULL )	{
				previous->mNextSibling = aOldChild->mNextSibling;
			}
			TNodePtr next = aOldChild->getNextSibling();
			if ( next.get() != NULL )	{
				next->mPreviousSibling = aOldChild->mPreviousSibling;
			}

			// Notify parent that node is removed
			notifyNodeChange( aOldChild, NODE_REMOVED );

			// Remove references to position in tree
			aOldChild->mParentNode.reset();
			aOldChild->mPreviousSibling.reset();
			aOldChild->mNextSibling.reset();

			return aOldChild;
		}
		else	{
			throw TDOMException( NOT_FOUND_ERR );
		}
	}
	
}

TNodePtr TNode	::	appendChild( TNodePtr aNodeChild )	{

	if ( aNodeChild.get() == NULL )	{
		return TNodePtr();
	}

	if ( this == aNodeChild.get() || isAncestor( aNodeChild ) )	{
		throw TDOMException( HIERARCHY_REQUEST_ERR );
	}

	if ( ! isChildAllowed( aNodeChild ) )	{
		throw TDOMException( HIERARCHY_REQUEST_ERR );
	}

	// If aNodeChild is in the tree already remove it there first.
	TNodePtr parent = aNodeChild->getParentNode();
	if ( parent.get() != NULL )	{
		parent->removeChild( aNodeChild );
	}

	TNodePtr lastNode;
	if ( mNodeList.size() > 0 )	{
		lastNode = mNodeList.back();
	}
	// If available, update the last node
	if ( lastNode.get() != NULL )	{
		lastNode->mNextSibling = aNodeChild;
	}

	// Add aNodeChild in the tree
	aNodeChild->mPreviousSibling = lastNode;
	aNodeChild->mParentNode = shared_from_this();
	aNodeChild->mNextSibling.reset();
	mNodeList.push_back( aNodeChild );
	
	// Notify parent that node is added
	notifyNodeChange( aNodeChild, NODE_ADDED );

	return aNodeChild;
	
}

bool TNode	::	hasChildNodes() const	{
	
	return ! ( mNodeList.empty() );
	
}

TNodePtr TNode	::	cloneNode( bool aDeep ) const	{

	// Still need to implement a deep clone
	
	TNodePtr clonedNode( new TNode( mNodeType, mOwnerDocument ) );
	
	return clonedNode;
	
}

bool TNode	::	hasAttributes()	{

	if ( mNodeType == ELEMENT_NODE && mAttributes.get() == NULL )	{
		mAttributes = TNamedNodeMapPtr( new TNamedNodeMap( &mAttributeList, shared_from_this() ) );
	}
	
	if ( mAttributes.get() && mAttributes->getLength() )	{
		return true;
	}
	
	return false;

}

bool TNode	::	isSameNode( const TNodePtr aOther ) const	{
	
	if ( this == aOther.get() )	{
		return true;
	}
	
	return false;

}

bool TNode	::	isAncestor( const TNodePtr aNode ) const	{

	TNodePtr ancestorNode = getParentNode();
	if ( ancestorNode.get() != NULL )	{
		if ( ancestorNode == aNode )	{
			return true;
		}
		else	{
			ancestorNode->isAncestor( aNode );
		}
	}
	
	return false;
	
}	

bool TNode	::	isChildAllowed( const TNodePtr aNewChild ) const	{
	
	switch ( mNodeType )	{
		case DOCUMENT_NODE:	{
			switch ( aNewChild->mNodeType )	{
				case ELEMENT_NODE:
				case PROCESSING_INSTRUCTION_NODE:
				case COMMENT_NODE:
				case DOCUMENT_TYPE_NODE:	{
					// This is allowed
					return true;
				}
				default:	{
					// Not allowed
					return false;
				}
			}
		}
		case DOCUMENT_FRAGMENT_NODE:
		case ENTITY_REFERENCE_NODE:
		case ELEMENT_NODE:
		case ENTITY_NODE:	{
			switch ( aNewChild->mNodeType )	{
				case ELEMENT_NODE:
				case PROCESSING_INSTRUCTION_NODE:
				case COMMENT_NODE:
				case TEXT_NODE:
				case CDATA_SECTION_NODE:
				case ENTITY_REFERENCE_NODE:	{
					// This is allowed
					return true;
				}
				default:	{
					// Not allowed
					return false;
				}
			}
		}
		case ATTRIBUTE_NODE:	{
			switch ( aNewChild->mNodeType )	{
				case TEXT_NODE:
				case ENTITY_REFERENCE_NODE:	{
					// This is allowed
					return true;
				}
				default:	{
					// Not allowed
					return false;
				}
			}
		}
		case DOCUMENT_TYPE_NODE:
		case PROCESSING_INSTRUCTION_NODE:
		case COMMENT_NODE:
		case TEXT_NODE:
		case CDATA_SECTION_NODE:
		case NOTATION_NODE:	{
			// Not allowed
			return false;
		}
	}

	return false;

}

void TNode	::	setNodeName( const TDOMString aValue )	{
	
	mNodeName = aValue;
	
}

void TNode	::	nodeChanged( TNodePtr aNode, NodeChange aChange )	{
	
	// Check the tracking lists to see if this node fits in one of them
	vector<TNodeListContainerPtr>::iterator iter;
	for ( iter = mNodeListContainers.begin(); iter != mNodeListContainers.end(); iter++ )	{
		TNodeListContainerPtr current = *iter;
		if ( current->getNodeType() == aNode->getNodeType() )	{
			if ( current->getQueryString() == aNode->getNodeName() || current->getQueryString() == "*" )	{
				if ( aChange == NODE_ADDED )	{
					current->addNode( aNode );
				}
				if ( aChange == NODE_REMOVED )	{
					current->removeNode( aNode );
				}
			}
		}
	}

	// Check the parent's tracking lists
	TNodePtr parent = getParentNode();
	if ( parent.get() != NULL )	{
		parent->nodeChanged( aNode, aChange );
	}

}

void TNode	::	notifyNodeChange( TNodePtr aNode, NodeChange aChange )	{
	
	nodeChanged( aNode, aChange );
	
	TNodeListPtr children = aNode->getChildNodes();
	int length = children->getLength();
	for ( int i = 0; i < length; i++ )	{
		TNodePtr node = children->item( i );
		notifyNodeChange( node, aChange );
	}

}

vector<TNodePtr> TNode	::	collectNodes( TDOMString aName, unsigned short aNodeType )	{
	
	vector<TNodePtr> result;
	int itemsInList = mNodeList.size();
	for ( int i = 0; i < itemsInList; i++ )	{
		TNodePtr current = mNodeList[ i ];
		if ( current->getNodeType() == aNodeType && ( current->getNodeName() == aName || aName == "*" ) )	{
			// This node must be added to the list.
			result.push_back( current );
		}
		// Collect the nodes of the children
		vector<TNodePtr> temp = current->collectNodes( aName, aNodeType );
		for ( int j = 0; j < (int) temp.size(); j++ )	{
			result.push_back( temp[ j ] );
		}
	}
	
	return result;
	
}

const char * TNode	::	getNodeTypeString() const	{
	
	return mNodeTypeString.c_str();
	
}
