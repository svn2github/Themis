/*	Element implementation of the DOM core
	See the DOM core specification for more information
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	03-02-2002
*/

#ifndef TELEMENT_H
#define TELEMENT_H

#include "DOMSupport.h"
#include "TNode.h"
#include "TAttr.h"

class TAttr;

class TElement	:	public TNode	{
	
	private:
		TDOMString mTagName;
		
	public:
		TElement( const TDOMString aTagName );
		~TElement();
		TDOMString getTagName() const;
		
		TDOMString getAttribute( const TDOMString aName ) const;
		void setAttribute( const TDOMString aName, const TDOMString aValue );
		
};

#endif
