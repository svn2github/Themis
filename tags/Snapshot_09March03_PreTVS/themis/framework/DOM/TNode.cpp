/* Node implementation
	See TNode.h for some more information
*/

#include <string>

#include "TNode.h"
#include "TNamedNodeMap.h"
#include "TNodeList.h"
#include "TNodeListContainer.h"
#include "TDOMException.h"

TNode	::	TNode( const unsigned short aNodeType,
						   const TDocumentWeak aOwnerDocument,
						   const TDOMString aNodeName,
						   const TDOMString aNodeValue )	{
	
	mNodeType = aNodeType;
	mOwnerDocument = aOwnerDocument;

	mParentNode.reset();
	mThisPointer.reset();
	mNextSibling.reset();
	mPreviousSibling.reset();
	
	mNodeList = vector<TNodeShared>();
	mChildNodes = TNodeListShared( new TNodeList( &mNodeList ) );
	mNodeListContainers = vector<TNodeListContainerShared>();

	switch ( mNodeType )	{
		case ELEMENT_NODE:	{
			mNodeName = aNodeName;
			mNodeTypeString = "ELEMENT_NODE";
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

	//printf( "Destructor called of %s: %s\n", getNodeTypeString(), getNodeName().c_str() );

	if ( mParentNode.get() != NULL )	{
		// Still attached to the tree
		mParentNode.get()->removeChild( mThisPointer );
	}

	if ( mNodeType == ELEMENT_NODE )	{
		// Has a NodeMap
		mAttributes.reset();
	}

	TNodeShared item;

	// Remove all items that are attached to this node
	for ( int i = mNodeList.size() - 1; i >= 0; i-- )	{
		item = mNodeList[ i ];
		removeChild( item );
	}
	
	//delete mChildNodes;

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

TNodeListShared TNode	::	getChildNodes() const	{
	
	return mChildNodes;
	
}

TNodeWeak TNode	::	getParentNode() const	{
	
	return mParentNode;
	
}

TNodeWeak TNode	::	getFirstChild() const	{
	
	return mNodeList.front();
	
}

TNodeWeak TNode	::	getLastChild() const	{
	
	return mNodeList.back();
	
}

TNodeWeak TNode	::	getNextSibling() const	{
	
	return mNextSibling;
	
}

TNodeWeak TNode	::	getPreviousSibling() const	{
	
	return mPreviousSibling;
	
}

TNamedNodeMapShared TNode	::	getAttributes() const	{
	
	return mAttributes;
	
}

TDocumentWeak TNode	::	getOwnerDocument() const	{
	
	return mOwnerDocument;
	
}

TNodeWeak TNode	::	insertBefore( TNodeShared aNewChild, TNodeShared aRefChild )	{
	
	if ( aNewChild.get() == NULL )	{
		return TNodeWeak();
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
		vector<TNodeShared>::iterator iter;
		iter = find( mNodeList.begin(), mNodeList.end(), aRefChild );
		if ( iter != mNodeList.end() )	{
			int j = mNodeList.size();
			for ( int i = 0; i < j; i++ )	{
				if ( mNodeList[ i ] == aRefChild )	{

					// If aNewChild is in the tree already remove it there first.
					if ( aNewChild->mParentNode.get() != NULL )	{
						aNewChild->mParentNode.get()->removeChild( aNewChild );
					}

					// Insert aNewChild in the tree and adjust the pointers
					mNodeList.insert( &mNodeList[ i - 1 ], aNewChild );
					aNewChild->mParentNode = mThisPointer;
					aNewChild->mPreviousSibling = aRefChild->mPreviousSibling;
					aNewChild->mNextSibling = aRefChild;
					aRefChild->mPreviousSibling = aNewChild;

					// Update the previousSibling's nextSibling
					if ( aNewChild->mPreviousSibling.get() != NULL )	{
						aNewChild->mPreviousSibling.get()->mNextSibling = aNewChild;
					}
				}
			}
			aNewChild->notifyNodeChange( mThisPointer, NODE_ADDED );
		}
		else	{
			return TNodeWeak();
		}
	}
	
	return aNewChild;
	
}						

TNodeWeak TNode	::	replaceChild( TNodeShared aNewChild, TNodeShared aOldChild)	{
	
	if ( aNewChild.get() == NULL )	{
		return TNodeWeak();
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
		vector<TNodeShared>::iterator iter;
		iter = find( mNodeList.begin(), mNodeList.end(), aOldChild );
		if ( iter != mNodeList.end() )	{
			int j = mNodeList.size();
			for ( int i = 0; i < j; i++ )	{
				if ( mNodeList[ i ] == aOldChild )	{
					// If aNewChild is in the tree already remove it there first.
					if ( aNewChild->mParentNode.get() != NULL )	{
						aNewChild->mParentNode.get()->removeChild( aNewChild );
					}

					// Remove aOldChild from list and add aNewChild in its place
					mNodeList.erase( &mNodeList[ i ] );
					mNodeList.insert( &mNodeList[ i ], aNewChild );

					// Notify parent that node is removed
					aOldChild->notifyNodeChange( mThisPointer, NODE_REMOVED );
					
					// Set aNewChild's parentNode, previous- and nextSibling to aOldChild's values
					aNewChild->mParentNode = mThisPointer;
					aNewChild->mPreviousSibling = aOldChild->mPreviousSibling;
					aNewChild->mNextSibling = aOldChild->mNextSibling;

					// Set aOldChild's values to NULL, so it isn't in the tree anymore.
					aOldChild->mPreviousSibling.reset();
					aOldChild->mNextSibling.reset();
					aOldChild->mParentNode.reset();

					// Set neighbour siblings to point to aNewChild
					if ( aNewChild->mPreviousSibling.get() != NULL )	{
						aNewChild->mPreviousSibling.get()->mNextSibling = aNewChild;
					}
					if ( aNewChild->mNextSibling.get() != NULL )	{
						aNewChild->mNextSibling.get()->mPreviousSibling = aNewChild;
					}
				}
			}
			// Notify parent that node is added
			aNewChild->notifyNodeChange( mThisPointer, NODE_ADDED );
		}
		else	{
			throw TDOMException( NOT_FOUND_ERR );
		}
	}
	
	return aOldChild;
	
}

TNodeWeak TNode	::	 removeChild( TNodeShared aOldChild )	{
	
	if ( aOldChild.get() == NULL )	{
		// NULL is technically not in the tree
		throw TDOMException( NOT_FOUND_ERR );
	}
	else	{
		vector<TNodeShared>::iterator iter = find( mNodeList.begin(), mNodeList.end(), aOldChild );
		if ( iter != mNodeList.end() )	{
			// Remove node
			mNodeList.erase( iter );
			
			// Link up next and previous siblings
			if ( aOldChild->mPreviousSibling.get() != NULL )	{
				aOldChild->mPreviousSibling.get()->mNextSibling = aOldChild->mNextSibling;
			}
			if ( aOldChild->mNextSibling.get() != NULL )	{
				aOldChild->mNextSibling.get()->mPreviousSibling = aOldChild->mPreviousSibling;
			}

			// If mThisPointer is expired, all copies are removed and don't have to be checked anymore
			if ( !mThisPointer.expired() )	{
				// Notify parent that node is removed
				aOldChild->notifyNodeChange( make_shared( mThisPointer ), NODE_REMOVED );
			}

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
	
	return TNodeWeak();
	
}

TNodeWeak TNode	::	appendChild( TNodeShared aNodeChild )	{

	if ( aNodeChild.get() == NULL )	{
		return TNodeWeak();
	}

	if ( this == aNodeChild.get() || isAncestor( aNodeChild ) )	{
		throw TDOMException( HIERARCHY_REQUEST_ERR );
	}

	if ( !isChildAllowed( aNodeChild ) )	{
		throw TDOMException( HIERARCHY_REQUEST_ERR );
	}

	// If aNodeChild is in the tree already remove it there first.
	if ( aNodeChild->mParentNode.get() != NULL )	{
		aNodeChild->mParentNode.get()->removeChild( aNodeChild );
	}

	TNodeWeak lastNode;
	if ( mNodeList.size() > 0 )	{
		lastNode = (TNodeWeak) mNodeList.back();
	}
	// If available, update the last node
	if ( lastNode.get() != NULL )	{
		lastNode.get()->mNextSibling = aNodeChild;
	}

	// Add aNodeChild in the tree
	aNodeChild->mPreviousSibling = lastNode;
	aNodeChild->mParentNode = mThisPointer;
	aNodeChild->mNextSibling.reset();
	mNodeList.push_back( aNodeChild );
	
	// Notify parent that node is added
	aNodeChild->notifyNodeChange( mThisPointer, NODE_ADDED );

	return aNodeChild;
	
}

bool TNode	::	hasChildNodes() const	{
	
	return !( mNodeList.empty() );
	
}

TNodeShared TNode	::	cloneNode( bool aDeep ) const	{
	// Still need to implement a deep clone
	
	TNodeShared clonedNode( new TNode( mNodeType, mOwnerDocument ) );
	
	return clonedNode;
	
}

bool TNode	::	hasAttributes() const	{
	
	if ( mAttributes.get() && mAttributes->getLength() )	{
		return true;
	}
	
	return false;

}

bool TNode	::	isSameNode( const TNodeShared aOther ) const	{
	
	if ( this == aOther.get() )	{
		return true;
	}
	
	return false;

}

bool TNode	::	isAncestor( const TNodeShared aNode ) const	{
	
	TNodeWeak ancestorNode = mThisPointer;
	ancestorNode = ancestorNode.get()->mParentNode;
	while ( ancestorNode.get() != NULL )	{
		if ( ancestorNode.get() == aNode.get() )	{
			return true;
		}
		ancestorNode = ancestorNode.get()->mParentNode;
	}
	
	return false;
	
}	

bool TNode	::	isChildAllowed( const TNodeShared aNewChild ) const	{
	
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

void TNode	::	nodeChanged( TNodeShared aNode, NodeChange aChange )	{
	
	vector<TNodeListContainerShared>::iterator current;
	for ( current = mNodeListContainers.begin(); current != mNodeListContainers.end(); current++ )	{
		if ( current->get()->getNodeType() == aNode->getNodeType() )	{
			if ( current->get()->getQueryString() == aNode->getNodeName() || current->get()->getQueryString() == "*" )	{
				if ( aChange == NODE_ADDED )	{
					current->get()->addNode( aNode );
				}
				if ( aChange == NODE_REMOVED )	{
					current->get()->removeNode( aNode );
				}
			}
		}
	}

	if ( mParentNode.get() != NULL )	{
		mParentNode.get()->nodeChanged( aNode, NODE_ADDED );
	}

}

void TNode	::	notifyNodeChange( TNodeShared aNotifyNode, NodeChange aChange )	{
	
	aNotifyNode->nodeChanged( make_shared( mThisPointer ), aChange );
	int itemsInList = mNodeList.size();
	TNodeWeak current;
	for ( int i = 0; i < itemsInList; i++ )	{
		current = mNodeList[ i ];
		current.get()->notifyNodeChange( aNotifyNode, aChange );
	}

}

vector<TNodeShared> TNode	::	collectNodes( TDOMString aName, unsigned short aNodeType )	{
	
	vector<TNodeShared> result;
	int itemsInList = mNodeList.size();
	TNodeShared current;
	for ( int i = 0; i < itemsInList; i++ )	{
		current = mNodeList[ i ];
		if ( current.get()->getNodeType() == aNodeType && ( current.get()->getNodeName() == aName || aName == "*" ) )	{
			// This node must be added to the list.
			result.push_back( current );
		}
		vector<TNodeShared> temp = current.get()->collectNodes( aName, aNodeType );
		for ( int j = 0; j < (int) temp.size(); j++ )	{
			result.push_back( temp[ j ] );
		}
	}
	
	return result;
	
}

const char * TNode	::	getNodeTypeString() const	{
	
	return mNodeTypeString.c_str();
	
}

void TNode	::	setSmartPointer( TNodeShared pointer )	{
	
	mThisPointer = pointer;
	if ( mNodeType == ELEMENT_NODE )	{
		mAttributeList = vector<TNodeShared>();
		mAttributes = TNamedNodeMapShared( new TNamedNodeMap( &mAttributeList, mThisPointer ) );
	}
	
}
