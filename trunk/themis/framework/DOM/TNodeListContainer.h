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
		unsigned short mNodeType;
				
	public:
		TNodeListContainer( const TDOMString aQueryString, BList * aNodes, BList * aNodeLists, unsigned short aNodeType );
		~TNodeListContainer();
		TDOMString getQueryString() const;
		unsigned short getNodeType() const;
		void addNode( TNode * aNode );
		TNode * removeNode( TNode * aNode );
		TNodeList * addNodeList();
		TNodeList * removeNodeList( TNodeList * aNodeList );
};

#endif
