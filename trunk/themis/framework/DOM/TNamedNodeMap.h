/*	NamedNodeMap implementation of the DOM core
	See the DOM core specification for more information
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	03-02-2002
*/

#ifndef TNAMEDNODEMAP_H
#define TNAMEDNODEMAP_H

#include <List.h>

#include "DOMSupport.h"
#include "TNode.h"

class TNode;

class TNamedNodeMap	{
	
	private:
		// Not implemented variable. Implemented using function in mNodeList
		// unsigned long length;
		// ------------------------------------------------------------------------------ 

		// Support variables
		BList * mNodeList;
		TNode * mMappedNode;
		
	public:
		TNamedNodeMap( void * aNodeList = NULL );
		~TNamedNodeMap();
		unsigned long getLength();
		TNode * getNamedItem( const TDOMString aName ); // Still have to check if exceptions work right
		TNode * setNamedItem( TNode * aArg ); // Still have to check if exceptions work right
		TNode * removeNamedItem( const TDOMString aName ); // Still have to check if exceptions work right
		TNode * item( unsigned long aIndex );
		// TNode * getNamedItemNS( const DOMString aNameSpaceURI, const DOMString aLocalName ); // Not yet implemented
		// TNode * setNamedItemNS( TNode aArg ); // Not yet implemented
		// TNode * removeNamedItemNS( const DOMString aNameSpaceURI, const DOMString aLocalName ); // Not yet implemented
		
};

#endif
