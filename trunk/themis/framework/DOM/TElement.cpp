/* Element implementation
	See TElement.h for some more information
*/

#include "TElement.h"

TElement	::	TElement()	:	TNode( ELEMENT_NODE )	{
	
	mTagName.SetTo( "" );
	
}

TElement	::	~TElement()	{
	
}

TDOMString TElement	::	getTagName() const	{
	
	return mTagName;
	
}
