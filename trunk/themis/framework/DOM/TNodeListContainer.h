/*	NodeListContainer implementation
	Support class for the DOM core
	This is not part of the DOM spec
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	25-03-2002
*/

#ifndef TNODELISTCONTAINER_H
#define TNODELISTCONTAINER_H

#include "DOMSupport.h"

class TNodeList;
class TNode;

class TNodeListContainer	{
	
	private:
		BList * mNodes;
		BList * mNodeLists;
		TDOMString mQueryString;
				
	public:
		TNodeListContainer( const TDOMString aQueryString);
		~TNodeListContainer();
		TDOMString getQueryString();
		void addNode( TNode * aNode );
		TNode * removeNode( TNode * aNode );
		void addNodeList( TNodeList * aNodeList );
		TNodeList * removeNodeList( TNodeList * aNodeList );
};

#endif
