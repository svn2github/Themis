/* Element implementation
	See TElement.h for some more information
*/

#include "TElement.h"
#include "TNamedNodeMap.h"
#include "TAttr.h"

TElement	::	TElement( const TDOMString aTagName )	:	TNode( ELEMENT_NODE, aTagName )	{
	
	mTagName.SetTo( aTagName );
	
}

TElement	::	~TElement()	{
	
}

TDOMString TElement	::	getTagName() const	{
	
	return mTagName;
	
}

TDOMString TElement	::	getAttribute( const TDOMString aName ) const	{
	
	TNode * attribute = getAttributes()->getNamedItem( aName );
	
	if ( !attribute )	{
		return TDOMString( "" );
	}
	
	return *( attribute->getNodeValue() );
	
}
	
void TElement	::	setAttribute( const TDOMString aName, const TDOMString aValue )	{
	
	TAttr * attribute = (TAttr *) getAttributes()->getNamedItem( aName );
	if ( !attribute )	{
		// No attribute with that name yet, so create one.
		attribute = new TAttr( aName, true, aValue, this );
		getAttributes()->setNamedItem( (TNode *) attribute );
	}
	else	{
		attribute->setValue( aValue );
	}
}

void TElement	::	removeAttribute( const TDOMString aName )	{

	TAttr * attribute = (TAttr *) getAttributes()->getNamedItem( aName );
	TDOMString defaultValue = attribute->getDefaultValue();
	if ( defaultValue != "" )	{
		attribute->setValue( defaultValue );
	}		

}

TAttr * TElement	::	getAttributeNode( const TDOMString aName ) const	{
	
	return (TAttr *) getAttributes()->getNamedItem( aName );
	
}

TAttr * TElement	::	setAttributeNode( TAttr * aNewAttr )	{
	
	return (TAttr *) getAttributes()->setNamedItem( aNewAttr );
	
}

TAttr * TElement	::	removeAttributeNode( TAttr * aOldAttr )	{
	
	getAttributes()->removeNamedItem( *( aOldAttr->getNodeName() ) );
	TDOMString defaultValue = aOldAttr->getDefaultValue();
	if ( defaultValue != "" )	{
		TAttr * newAttribute = new TAttr( aOldAttr->getName(), aOldAttr->getSpecified(), aOldAttr->getValue(), aOldAttr->getOwnerElement() );
		getAttributes()->setNamedItem( newAttribute );
	}

	return aOldAttr;

}
