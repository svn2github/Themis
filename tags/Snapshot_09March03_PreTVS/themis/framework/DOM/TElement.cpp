/* Element implementation
	See TElement.h for some more information
*/

#include <string>

#include "TElement.h"
#include "TNamedNodeMap.h"
#include "TAttr.h"
#include "TNodeListContainer.h"
#include "TNodeList.h"

TElement	::	TElement( const TDocumentWeak aOwnerDocument,
								   const TDOMString aTagName )	:
										TNode( ELEMENT_NODE, aOwnerDocument, aTagName )	{
	
	mTagName = aTagName;
	
}

TElement	::	~TElement()	{
	
}

TDOMString TElement	::	getTagName() const	{
	
	return mTagName;
	
}

TDOMString TElement	::	getAttribute( const TDOMString aName ) const	{
	
	TNodeWeak attribute = getAttributes().get()->getNamedItem( aName );
	
	if ( !attribute.get() )	{
		return TDOMString( "" );
	}
	
	return attribute.get()->getNodeValue();
	
}
	
void TElement	::	setAttribute( const TDOMString aName, const TDOMString aValue )	{
	
	TAttrShared attribute = shared_static_cast<TAttr> ( make_shared( getAttributes().get()->getNamedItem( aName ) ) );
	if ( !attribute.get() )	{
		// No attribute with that name yet, so create one.
		
		attribute = TAttrShared( new TAttr( aName, getOwnerDocument(), true, aValue, shared_static_cast<TElement>( make_shared( mThisPointer ) ) ) );
		attribute->setSmartPointer( attribute );
		getAttributes().get()->setNamedItem( attribute );
	}
	else	{
		attribute->setValue( aValue );
	}
}

void TElement	::	removeAttribute( const TDOMString aName )	{

	TAttrShared attribute = shared_static_cast<TAttr> ( make_shared( getAttributes().get()->getNamedItem( aName ) ) );
	TDOMString defaultValue = attribute->getDefaultValue();
	if ( defaultValue != "" )	{
		attribute->setValue( defaultValue );
	}		

}

TAttrWeak TElement	::	getAttributeNode( const TDOMString aName ) const	{
	
	return shared_static_cast<TAttr> ( make_shared( getAttributes().get()->getNamedItem( aName ) ) );
	
}

TAttrWeak TElement	::	setAttributeNode( TAttrShared aNewAttr )	{
	
	return shared_static_cast<TAttr> ( make_shared( getAttributes().get()->setNamedItem( aNewAttr ) ) );
	
}

TAttrShared TElement	::	removeAttributeNode( TAttrShared aOldAttr )	{
	
	getAttributes().get()->removeNamedItem( aOldAttr->getNodeName() );
	TDOMString defaultValue = aOldAttr->getDefaultValue();
	if ( defaultValue != "" )	{
		TAttrShared newAttribute = TAttrShared( new TAttr( aOldAttr->getName(), aOldAttr->getOwnerDocument(), aOldAttr->getSpecified(), aOldAttr->getValue(), aOldAttr->getOwnerElement() ) );
		newAttribute->setSmartPointer( newAttribute );
		getAttributes().get()->setNamedItem( newAttribute );
	}

	return aOldAttr;

}

TNodeListShared TElement	::	getElementsByTagName( const TDOMString aName )	{
	
	vector<TNodeListContainerShared>::iterator current;
	for ( current = mNodeListContainers.begin(); current != mNodeListContainers.end(); current++ )	{
		if ( current->get()->getNodeType() == ELEMENT_NODE )	{
			if ( current->get()->getQueryString() == aName )	{
				// There already is a NodeList with this querystring
				// Get a pointer to the nodelist
				return current->get()->getNodeList();
			}
		}
	}
		
	// Not one NodeList with query aName exists yet. Create it.
	vector<TNodeShared> * resultList = new vector<TNodeShared>( collectNodes( aName, ELEMENT_NODE ) );
	TNodeListContainerShared container( new TNodeListContainer( aName, resultList, ELEMENT_NODE ) );
	mNodeListContainers.push_back( container );
	
	return container->getNodeList();
	
}

bool TElement	::	hasAttribute( const TDOMString aName ) const	{

	TNodeWeak node = getAttributes()->getNamedItem( aName );
	if ( node.get() )	{
		return true;
	}
	
	return false;
	
}
