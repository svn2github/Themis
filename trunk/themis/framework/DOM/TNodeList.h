/*	NodeList implementation of the DOM core
	See the DOM core specification for more information
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	12-12-2001
*/

#ifndef TNODELIST_H
#define TNODELIST_H

#include <List.h>

#include "DOMSupport.h"

class TNode;
class TNodeListContainer;

class TNodeList	{
	
	private:
		const BList * mNodeList;
		// Not implemented variable. Implemented using function in mNodeList
		// unsigned long length;
		// ---------------------------------------------------------------------------------
		
		// Suuport attribute
		TNodeListContainer * mContainer;
		
	public:
		TNodeList( const void * aNodeList = NULL, TNodeListContainer * aContainer = NULL );
		~TNodeList();
		unsigned long getLength();
		TNode * item( unsigned long aIndex );
		
};

#endif
