/* Node implementation
	See TNode.h for some more information
*/

#include "TNode.h"
#include "TNamedNodeMap.h"
#include "TNodeList.h"
#include "TNodeListContainer.h"
#include "TDOMException.h"

TNode	::	TNode( const unsigned short aNodeType, const TDOMString aNodeName = "", const TDOMString aNodeValue = "" )	{
	
	mNodeType = aNodeType;

	mParentNode = NULL;
	mNextSibling = NULL;
	mPreviousSibling = NULL;
	
	mNodeList = new BList();
	mChildNodes = new TNodeList( mNodeList );
	mNodeListContainers = new BList();

	switch ( mNodeType )	{
		case ELEMENT_NODE:	{
			mNodeName.SetTo( aNodeName );
			mAttributeList = new BList();
			mAttributes = new TNamedNodeMap( mAttributeList, this );
			mNodeTypeString.SetTo( "ELEMENT_NODE" );
			break;
		}
		case ATTRIBUTE_NODE:	{
			mNodeName.SetTo( aNodeName );
			mNodeValue.SetTo( aNodeValue );
			mNodeTypeString.SetTo( "ATTRIBUTE_NODE" );
			break;
		}
		case TEXT_NODE:	{
			mNodeName.SetTo( "#text" );
			mNodeValue.SetTo( aNodeValue );
			mNodeTypeString.SetTo( "TEXT_NODE" );
			break;
		}
		case CDATA_SECTION_NODE:	{
			mNodeName.SetTo( "cdata-section" );
			mNodeValue.SetTo( aNodeValue );
			mNodeTypeString.SetTo( "CDATA_SECTION_NODE" );
			break;
		}
		case ENTITY_REFERENCE_NODE:	{
			mNodeTypeString.SetTo( "ENTITY_REFERENCE_NODE" );
			break;
		}
		case ENTITY_NODE:	{
			mNodeTypeString.SetTo( "ENTITY_NODE" );
			break;
		}
		case PROCESSING_INSTRUCTION_NODE:	{
			mNodeName.SetTo( aNodeValue );
			mNodeTypeString.SetTo( "PROCESSSING_INSTRUCTION_NODE" );
			break;
		}
		case COMMENT_NODE:	{
			mNodeName.SetTo( "#comment" );
			mNodeValue.SetTo( aNodeValue );
			mNodeTypeString.SetTo( "COMMENT_NODE" );
			break;
		}
		case DOCUMENT_NODE:	{
			mNodeName.SetTo( "#document" );
			mNodeTypeString.SetTo( "DOCUMENT_NODE" );
			break;
		}
		case DOCUMENT_TYPE_NODE:	{
			mNodeTypeString.SetTo( "DOCUMENT_TYPE_NODE" );
			break;
		}
		case DOCUMENT_FRAGMENT_NODE:	{
			mNodeName.SetTo( "#document-fragment" );
			mNodeTypeString.SetTo( "DOCUMENT_FRAGMENT_NODE" );
			break;
		}
		case NOTATION_NODE:	{
			mNodeTypeString.SetTo( "NOTATION_NODE" );
			break;
		}
		default:	{
			mNodeTypeString.SetTo( "Undefined node" );
			break;
		}
	}
	
}

TNode	::	~TNode()	{

	printf( "Destructor called of %s\n", mNodeName.String() );

	if ( mParentNode != NULL )	{
		// Still attached to the tree
		mParentNode->removeChild( this );
	}

	if ( mNodeType == ELEMENT_NODE )	{
		// Has a NodeMap
		delete mAttributes;
	}

	TNode * item = NULL;

	// Remove all items that are attached to this node
	for ( int i = mNodeList->CountItems() - 1; i >= 0; i-- )	{
		item = (TNode *) mNodeList->ItemAt( i );
		removeChild( item );
		delete item;
	}

	delete mChildNodes;
	delete mNodeList;
	delete mNodeListContainers;

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
			mNodeValue.SetTo( aNodeValue );
			break;
		}
	}
	
}

TNodeList * TNode	::	getChildNodes() const	{
	
	return mChildNodes;
	
}

TNode * TNode	::	getParentNode() const	{
	
	return mParentNode;
	
}

TNode * TNode	::	getFirstChild() const	{
	
	return (TNode *) mNodeList->FirstItem();
	
}

TNode * TNode	::	getLastChild() const	{
	
	return (TNode *) mNodeList->LastItem();
	
}

TNode * TNode	::	getNextSibling() const	{
	
	return mNextSibling;
	
}

TNode * TNode	::	getPreviousSibling() const	{
	
	return mPreviousSibling;
	
}

TNamedNodeMap * TNode	::	getAttributes() const	{
	
	return mAttributes;
	
}

TNode * TNode	::	insertBefore( TNode * aNewChild, TNode * aRefChild )	{
	
	if ( !aNewChild )	{
		return NULL;
	}

	if ( this == aNewChild || isAncestor( aNewChild ) )	{
		throw TDOMException( HIERARCHY_REQUEST_ERR );
	}

	if ( !isChildAllowed( aNewChild ) )	{
		throw TDOMException( HIERARCHY_REQUEST_ERR );
	}
	
	if ( !aRefChild )	{
		// Just append it to the end of the list of children
		appendChild( aRefChild );
	}
	else	{
		if ( mNodeList->HasItem( (void *) aRefChild ) )	{
			int j = mNodeList->CountItems();
			for ( int i = 0; i < j; i++ )	{
				if ( mNodeList->ItemAt( i ) == aRefChild )	{

					// If aNewChild is in the tree already remove it there first.
					if ( aNewChild->mParentNode != NULL )	{
						aNewChild->mParentNode->removeChild( aNewChild );
					}

					// Insert aNewChild in the tree and adjust the pointers
					mNodeList->AddItem( (void *) aNewChild, i );
					aNewChild->mParentNode = this;
					aNewChild->mPreviousSibling = aRefChild->mPreviousSibling;
					aNewChild->mNextSibling = aRefChild;
					aRefChild->mPreviousSibling = aNewChild;

					// Update the previousSibling's nextSibling
					if ( aNewChild->mPreviousSibling != NULL )	{
						aNewChild->mPreviousSibling->mNextSibling = aNewChild;
					}
				}
			}
			aNewChild->notifyNodeChange( this, NODE_ADDED );
		}
		else	{
			return NULL;
		}
	}
	
	return aNewChild;
	
}						

TNode * TNode	::	replaceChild( TNode * aNewChild, TNode * aOldChild)	{
	
	if ( !aNewChild )	{
		return NULL;
	}

	if ( this == aNewChild || isAncestor( aNewChild ) )	{
		throw TDOMException( HIERARCHY_REQUEST_ERR );
	}

	if ( !isChildAllowed( aNewChild ) )	{
		throw TDOMException( HIERARCHY_REQUEST_ERR );
	}

	if ( !aOldChild )	{
		// NULL is technically not in the tree
		throw TDOMException( NOT_FOUND_ERR );
	}
	else	{
		if ( mNodeList->HasItem( (void *) aOldChild ) )	{
			int j = mNodeList->CountItems();
			for ( int i = 0; i < j; i++ )	{
				if ( mNodeList->ItemAt( i ) == aOldChild )	{
					// If aNewChild is in the tree already remove it there first.
					if ( aNewChild->mParentNode != NULL )	{
						aNewChild->mParentNode->removeChild( aNewChild );
					}

					// Remove aOldChild from list and add aNewChild in its place
					mNodeList->RemoveItem( i );
					mNodeList->AddItem( (void *) aNewChild, i );

					// Notify parent that node is removed
					aOldChild->notifyNodeChange( this, NODE_REMOVED );
					
					// Set aNewChild's parentNode, previous- and nextSibling to aOldChild's values
					aNewChild->mParentNode = this;
					aNewChild->mPreviousSibling = aOldChild->mPreviousSibling;
					aNewChild->mNextSibling = aOldChild->mNextSibling;

					// Set aOldChild's values to NULL, so it isn't in the tree anymore.
					aOldChild->mPreviousSibling = NULL;
					aOldChild->mNextSibling = NULL;
					aOldChild->mParentNode = NULL;

					// Set neighbour siblings to point to aNewChild
					if ( aNewChild->mPreviousSibling != NULL )	{
						aNewChild->mPreviousSibling->mNextSibling = aNewChild;
					}
					if ( aNewChild->mNextSibling != NULL )	{
						aNewChild->mNextSibling->mPreviousSibling = aNewChild;
					}
				}
			}
			// Notify parent that node is added
			aNewChild->notifyNodeChange( this, NODE_ADDED );
		}
		else	{
			throw TDOMException( NOT_FOUND_ERR );
		}
	}
	
	return aOldChild;
	
}

TNode * TNode	::	 removeChild( TNode * aOldChild )	{
	
	if ( !aOldChild )	{
		// NULL is technically not in the tree
		throw TDOMException( NOT_FOUND_ERR );
	}
	else	{
		if ( mNodeList->RemoveItem( aOldChild ) )	{
			// Link up next and previous siblings
			if ( aOldChild->mPreviousSibling != NULL )	{
				aOldChild->mPreviousSibling->mNextSibling = aOldChild->mNextSibling;
			}
			if ( aOldChild->mNextSibling != NULL )	{
				aOldChild->mNextSibling->mPreviousSibling = aOldChild->mPreviousSibling;
			}

			// Notify parent that node is removed
			aOldChild->notifyNodeChange( this, NODE_REMOVED );

			// Remove references to position in tree
			aOldChild->mParentNode = NULL;
			aOldChild->mPreviousSibling = NULL;
			aOldChild->mNextSibling = NULL;

			return aOldChild;
		}
		else	{
			throw TDOMException( NOT_FOUND_ERR );
		}
	}
	
	return NULL;
	
}

TNode * TNode	::	appendChild( TNode * aNodeChild )	{

	if ( !aNodeChild )	{
		return NULL;
	}

	if ( this == aNodeChild || isAncestor( aNodeChild ) )	{
		throw TDOMException( HIERARCHY_REQUEST_ERR );
	}

	if ( !isChildAllowed( aNodeChild ) )	{
		throw TDOMException( HIERARCHY_REQUEST_ERR );
	}

	// If aNodeChild is in the tree already remove it there first.
	if ( aNodeChild->mParentNode != NULL )	{
		aNodeChild->mParentNode->removeChild( aNodeChild );
	}

	TNode * lastNode = (TNode *) mNodeList->LastItem();
	// If available, update the last node
	if ( lastNode != NULL )	{
		lastNode->mNextSibling = aNodeChild;
	}
	// Add aNodeChild in the tree
	aNodeChild->mPreviousSibling = lastNode;
	aNodeChild->mParentNode = this;
	aNodeChild->mNextSibling = NULL;
	mNodeList->AddItem( aNodeChild );
	
	// Notify parent that node is added
	aNodeChild->notifyNodeChange( this, NODE_ADDED );

	return aNodeChild;
	
}

bool TNode	::	hasChildNodes() const	{
	
	return !( mNodeList->IsEmpty() );
	
}

TNode * TNode	::	cloneNode( bool aDeep ) const	{
	// Still need to implement a deep clone
	
	TNode * clonedNode = new TNode( mNodeType );
	
	return clonedNode;
	
}

bool TNode	::	hasAttributes() const	{
	
	if ( !mAttributes && hasChildNodes() )	{
		return true;
	}
	
	return false;

}

bool TNode	::	isSameNode( const TNode * aOther ) const	{
	
	if ( this == aOther )	{
		return true;
	}
	
	return false;

}

bool TNode	::	isAncestor( const TNode * aNode ) const	{
	
	const TNode * ancestorNode = this;
	while ( ( ancestorNode = ancestorNode->mParentNode ) != NULL )	{
		if ( ancestorNode == aNode )	{
			return true;
		}
	}
	
	return false;
	
}	

bool TNode	::	isChildAllowed( const TNode * aNewChild ) const	{
	
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
	
	mNodeName.SetTo( aValue );
	
}

void TNode	::	nodeChanged( TNode * aNode, NodeChange aChange )	{
	
	TNodeListContainer * current;
	int itemsInList = mNodeListContainers->CountItems();
	for ( int i = 0; i < itemsInList; i++ )	{
		current = (TNodeListContainer *) mNodeListContainers->ItemAt( i );
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

	if ( mParentNode != NULL )	{
		mParentNode->nodeChanged( aNode, NODE_ADDED );
	}

}

void TNode	::	notifyNodeChange( TNode * aNotifyNode, NodeChange aChange )	{
	
	aNotifyNode->nodeChanged( this, aChange );
	int itemsInList = mNodeList->CountItems();
	TNode * current = NULL;
	for ( int i = 0; i < itemsInList; i++ )	{
		current = (TNode *) mNodeList->ItemAt( i );
		current->notifyNodeChange( aNotifyNode, aChange );
	}

}

BList * TNode	::	collectNodes( TDOMString aName, unsigned short aNodeType )	{
	
	BList * result = new BList();
	int itemsInList = mNodeList->CountItems();
	TNode * current;
	for ( int i = 0; i < itemsInList; i++ )	{
		current = (TNode *) mNodeList->ItemAt( i );
		if ( current->getNodeType() == aNodeType && ( current->getNodeName() == aName || aName == "*" ) )	{
			// This node must be added to the list.
			result->AddItem( current );
		}
		BList * subResult =  current->collectNodes( aName, aNodeType );
		result->AddList( subResult );
		delete subResult;
	}
	
	return result;
	
}

const char * TNode	::	getNodeTypeString() const	{
	
	return mNodeTypeString.String();
	
}
