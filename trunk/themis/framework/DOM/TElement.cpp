/* Element implementation
	See TElement.h for some more information
*/

#include "TElement.h"

TElement	::	TElement( const TDOMString aTagName )	:	TNode( ELEMENT_NODE )	{
	
	mTagName.SetTo( aTagName );
	
}

TElement	::	~TElement()	{
	
}

TDOMString TElement	::	getTagName() const	{
	
	return mTagName;
	
}

TDOMString TElement	::	getAttribute( const TDOMString aName ) const	{
	
	TNode * attribute = getAttributes()->getNamedItem( aName );

	return *attribute->getNodeValue();
	
}
	
