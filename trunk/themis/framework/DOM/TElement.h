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
		TElement();
		~TElement();
		TDOMString getTagName() const;
		
};

#endif
