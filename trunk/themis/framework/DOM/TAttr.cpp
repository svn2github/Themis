/* Attr implementation
	See TAttr.h for some more information
*/

#include "TAttr.h"
#include "TElement.h"

TAttr	::	TAttr( const TDOMString aName, const bool aSpecified, const TDOMString aValue, TElementWeak aOwnerElement )	:	TNode( ATTRIBUTE_NODE, aName, aValue )	{

	mName = aName;
	if ( aOwnerElement.get() == NULL )	{
		mSpecified = true;
	}
	else	{
		mSpecified = aSpecified;
	}
	mValue = aValue;
	mDefaultValue = aValue;
	mOwnerElement = aOwnerElement;
	
}

TAttr	::	~TAttr()	{
	
}

TDOMString TAttr	::	getName()	const	{
	
	return mName;
	
}

bool TAttr	::	getSpecified() const	{
	
	return mSpecified;
	
}

TDOMString TAttr	::	getValue() const	{
	
	return mValue;
	
}

void TAttr	::	setValue( const TDOMString aValue )	{
	
	// Partly vague stuff in the DOM spec. Got this part from the mailing list.
	// Add a text node as a child to the attr node. Still to implement.
	
	mValue = aValue;
	setNodeName( aValue );
	mSpecified = true;
	
}


TElementWeak TAttr	::	getOwnerElement()	 const	{
	
	return mOwnerElement;
	
}

TDOMString TAttr	::	getDefaultValue() const	{
	
	return mDefaultValue;
	
}
