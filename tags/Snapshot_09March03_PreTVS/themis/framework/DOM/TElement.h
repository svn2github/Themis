/*	Element implementation of the DOM core
	See the DOM core specification for more information
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	03-02-2002
*/

#ifndef TELEMENT_H
#define TELEMENT_H

#include "DOMSupport.h"
#include "TNode.h"

class TElement	:	public TNode	{
	
	private:
		TDOMString mTagName;
		
	public:
		TElement( const TDocumentWeak aOwnerDocument,
					   const TDOMString aTagName );
		~TElement();
		TDOMString getTagName() const;
		
		TDOMString getAttribute( const TDOMString aName ) const;
		void setAttribute( const TDOMString aName, const TDOMString aValue );
		void removeAttribute( const TDOMString aName );
		TAttrWeak getAttributeNode( const TDOMString aName ) const;
		TAttrWeak setAttributeNode( TAttrShared aNewAttr );
		TAttrShared removeAttributeNode( TAttrShared aOldAttr );
		TNodeListShared getElementsByTagName( const TDOMString aName );
		bool hasAttribute( const TDOMString aName ) const;
		
};

#endif
