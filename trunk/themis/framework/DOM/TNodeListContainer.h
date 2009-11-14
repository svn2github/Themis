/*
	Copyright (c) 2002 Mark Hellegers. All Rights Reserved.
	
	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or
	sell copies of the Software, and to permit persons to whom
	the Software is furnished to do so, subject to the following
	conditions:
	
	   The above copyright notice and this permission notice
	   shall be included in all copies or substantial portions
	   of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
	KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
	WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
	OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
	OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
	OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
	
	Original Author: 	Mark Hellegers (mark@firedisk.net)
	Project Start Date: October 18, 2000
	Class Start Date: March 25, 2002
*/

#ifndef TNODELISTCONTAINER_H
#define TNODELISTCONTAINER_H

// Standard C++ headers
#include <vector>

// DOM headers
#include "DOMSupport.h"

// Namespaces used
using namespace std;

/// NodeListContainer implementation. Support class.

/**
	The NodeListContainer class is used a support class to help
	keep NodeLists up to date.
 	This is a support class and is not in the DOM core specification.
 	
 	@todo	Have to look if this can't be structured any simpler and if it can
 				be used in more places.
*/

class TNodeListContainer	{
	
	private:
		/// The list of nodes.
		vector<TNodePtr> * mNodes;
		
		/// The list.
		TNodeListPtr mNodeList;
		
		/// The query string.
		TDOMString mQueryString;
		
		/// The type of the nodes.
		unsigned short mNodeType;
				
	public:
		/// Constructor of the TNodeListContainer class.
		/**
			The constructor of the TNodeListContainer class. It stores the arguments it gets
			and creates a new NodeList to store nodes.
			
			@param	aQueryString	The query string for the container.
			@param	aNodes	The collection of nodes to store.
			@param	aNodeType	The type of nodes to store.
		*/
		TNodeListContainer( const TDOMString aQueryString, vector<TNodePtr> * aNodes, unsigned short aNodeType );
		
		/// Destructor of the TNodeListContainer class.
		/**
			The destructor of the TNodeListContainer class. It deletes the list of nodes that are stored.
		*/
		~TNodeListContainer();
		
		/// A function to get the query string.
		TDOMString getQueryString() const;
		
		/// A function to get the node type.
		unsigned short getNodeType() const;
		
		/// A function to add a node.
		/**
			This function adds the node supplied by the argument to the list.
			If it already is in the list, nothing happens.
			
			@param	aNode	The node to add to the list.
			
			@todo	Have a look at the code for this function.
						I think I fixed a bug, but I'm not sure if it's correct now.
		*/
		void addNode( TNodePtr aNode );
		
		/// A function to remove a node.
		/**
			This function removes the node supplied by the argument from the list and returns it.
			If the node is not in the list, nothing happens.
			
			@param	aNode	The node to remove from the list.
		*/
		TNodePtr removeNode( TNodePtr aNode );
		
		/// A function to get the node list.
		TNodeListPtr getNodeList();
		
};

#endif
