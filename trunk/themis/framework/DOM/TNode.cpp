/* Node implementation
	See TNode.h for some more information
*/

#include "TNode.h"
#include "TNamedNodeMap.h"
#include "TNodeList.h"
#include "TDOMException.h"

TNode	::	TNode( const unsigned short aNodeType, const TDOMString aNodeName = "", const TDOMString aNodeValue = "" )	{
	
	mNodeType = aNodeType;

	mNodeName = new TDOMString();
	
	mParentNode = NULL;
	mNextSibling = NULL;
	mPreviousSibling = NULL;

	mNodeList = new BList();

	switch ( mNodeType )	{
		case ELEMENT_NODE:	{
			mNodeName->SetTo( aNodeName );
			mAttributeList = new BList();
			mAttributes = new TNamedNodeMap( mAttributeList, this );
			mNodeTypeString.SetTo( "ELEMENT_NODE" );
			break;
		}
		case ATTRIBUTE_NODE:	{
			mNodeName->SetTo( aNodeName );
			mNodeValue = new TDOMString();
			mNodeValue->SetTo( aNodeValue );
			mNodeTypeString.SetTo( "ATTRIBUTE_NODE" );
			break;
		}
		case TEXT_NODE:	{
			mNodeName->SetTo( "#text" );
			mNodeValue = new TDOMString();
			mNodeTypeString.SetTo( "TEXT_NODE" );
			break;
		}
		case CDATA_SECTION_NODE:	{
			mNodeName->SetTo( "cdata-section" );
			mNodeValue = new TDOMString();
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
			mNodeValue = new TDOMString();
			mNodeTypeString.SetTo( "PROCESSSING_INSTRUCTION_NODE" );
			break;
		}
		case COMMENT_NODE:	{
			mNodeName->SetTo( "#comment" );
			mNodeValue = new TDOMString();
			mNodeTypeString.SetTo( "COMMENT_NODE" );
			break;
		}
		case DOCUMENT_NODE:	{
			mNodeName->SetTo( "#document" );
			mNodeTypeString.SetTo( "DOCUMENT_NODE" );
			break;
		}
		case DOCUMENT_TYPE_NODE:	{
			mNodeTypeString.SetTo( "DOCUMENT_TYPE_NODE" );
			break;
		}
		case DOCUMENT_FRAGMENT_NODE:	{
			mNodeName->SetTo( "#document-fragment" );
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

	printf( "Destructor called\n" );

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

	delete mNodeList;
}

unsigned short TNode	::	getNodeType() const	{
	
	return mNodeType;
	
}

const TDOMString * TNode	::	getNodeName() const	{
	
	return mNodeName;
	
}

const TDOMString * TNode	::	getNodeValue() const	{
	
	return mNodeValue;
	
}

void TNode	::	setNodeValue( const TDOMString aNodeValue )	{
	
	if ( mNodeValue )	{
		// Node can have a value
		mNodeValue->SetTo( aNodeValue );
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
		// If aNewChild is in the tree already remove it there first.
		if ( aNewChild->mParentNode != NULL )	{
			aNewChild->mParentNode->removeChild( aNewChild );
		}

		mNodeList->AddItem( (void *) aNewChild );
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
		return NULL;
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
		}
		else	{
			throw TDOMException( NOT_FOUND_ERR );
		}
	}
	
	return aOldChild;
	
}

TNode * TNode	::	 removeChild( TNode * aOldChild )	{
	
	if ( !aOldChild )	{
		return NULL;
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
	
	mNodeName->SetTo( aValue );
	
}

const char * TNode	::	getNodeTypeString() const	{
	
	return mNodeTypeString.String();
	
}
