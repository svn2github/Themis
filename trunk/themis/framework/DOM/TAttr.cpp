/* Attr implementation
	See TAttr.h for some more information
*/

#include "TAttr.h"

TAttr	::	TAttr( const TDOMString aName, const bool aSpecified, const TDOMString aValue, TElement * aOwnerElement )	:	TNode( ATTRIBUTE_NODE )	{

	mName = aName;
	if ( !aOwnerElement )	{
		mSpecified = true;
	}
	else	{
		mSpecified = aSpecified;
	}
	mValue = aValue;
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
	
	mValue = aValue;
	mSpecified = true;
	
}

TElement * TAttr	::	getOwnerElement() const	{
	
	return mOwnerElement;
	
}