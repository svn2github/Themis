/*	NamedNodeMap implementation of the DOM core
	See the DOM core specification for more information
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	03-02-2002
*/

#ifndef TNAMEDNODEMAP_H
#define TNAMEDNODEMAP_H

#include <vector>

#include "DOMSupport.h"

class TNamedNodeMap	{
	
	private:
		// Not implemented variable. Implemented using function in mNodeList
		// unsigned long length;
		// ------------------------------------------------------------------------------ 

		// Support variables
		vector<TNodeShared> * mNodeList;
		TNodeWeak mMappedNode;
		
	public:
		TNamedNodeMap( vector<TNodeShared> * aNodeList, TNodeWeak aMappedNode );
		~TNamedNodeMap();

		unsigned long getLength();
		TNodeWeak getNamedItem( const TDOMString aName ); // Still have to check if exceptions work right
		TNodeWeak setNamedItem( TNodeShared aArg ); // Still have to check if exceptions work right
		TNodeShared removeNamedItem( const TDOMString aName ); // Still have to check if exceptions work right
		TNodeWeak item( unsigned long aIndex );
		// TNode * getNamedItemNS( const DOMString aNameSpaceURI, const DOMString aLocalName ); // Not yet implemented
		// TNode * setNamedItemNS( TNode aArg ); // Not yet implemented
		// TNode * removeNamedItemNS( const DOMString aNameSpaceURI, const DOMString aLocalName ); // Not yet implemented
		
};

#endif
