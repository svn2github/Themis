/*	NodeListContainer implementation
	Support class for the DOM core
	This is not part of the DOM spec
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	25-03-2002
*/

#ifndef TNODELISTCONTAINER_H
#define TNODELISTCONTAINER_H

#include "DOMSupport.h"
#include <List.h>

#include <vector>

class TNodeList;
class TNode;

class TNodeListContainer	{
	
	private:
		vector<TNodeShared> * mNodes;
		TNodeListShared mNodeList;
		TDOMString mQueryString;
		unsigned short mNodeType;
				
	public:
		TNodeListContainer( const TDOMString aQueryString, vector<TNodeShared> * aNodes, unsigned short aNodeType );
		~TNodeListContainer();
		TDOMString getQueryString() const;
		unsigned short getNodeType() const;
		void addNode( TNodeShared aNode );
		TNodeWeak removeNode( TNodeShared aNode );
		TNodeListShared getNodeList();
};

#endif
