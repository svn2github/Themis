/*	NodeList implementation of the DOM core
	See the DOM core specification for more information
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	12-12-2001
*/

#ifndef TNODELIST_H
#define TNODELIST_H

#include <vector>

#include "DOMSupport.h"

class TNode;
class TNodeListContainer;

class TNodeList	{
	
	private:
		const vector<TNodeShared> * mNodeList;
		// Not implemented variable. Implemented using function in mNodeList
		// unsigned long length;
		// ---------------------------------------------------------------------------------
		
	public:
		TNodeList( const vector<TNodeShared> * aNodeList );
		~TNodeList();
		unsigned long getLength();
		TNodeWeak item( unsigned long aIndex );
		
};

#endif
