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
	
	Original Author: 	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	Project Start Date: October 18, 2000
	Class Start Date: February 03, 2002
*/

#ifndef TNAMEDNODEMAP_H
#define TNAMEDNODEMAP_H

// Standard C++ headers
#include <vector>

// DOM headers
#include "DOMSupport.h"

// Namespaces used
using namespace std;

/// NamedNodeMap implementation of the DOM core.

/**
	The NamedNodeMap class stores a collection of nodes which can be accessed by name.
	It does not inherit from NodeList as the nodes it contains do not have an order.
	For an extensive explanation, see the DOM core pages at http://www.w3.org/DOM
*/

class TNamedNodeMap	{
	
	private:
		// Not implemented variable. Implemented using function in mNodeList
		// unsigned long length;
		// ------------------------------------------------------------------------------ 

		/// A list of nodes.
 		/// This is a support variable and is not in the DOM core specification.
		vector<TNodePtr> * mNodeList;

		/// The node to which this map belongs.
 		/// This is a support variable and is not in the DOM core specification.
		TNodeWeak mMappedNode;
		
	public:
		/// Constructor of the TNamedNodeMap.
		/**
			The constructor of the TNamedNodeMap. It gets a list of nodes and the node
			the map will belong to as arguments.
			
			@param aNodeList	The list of nodes to put in the node map.
			@param aMappedNode The node to which to map will belong.
			
			@todo	Have to look if it is beneficial to use a TNodeListContainer to store the list of nodes.
						I don't like the pointer.
		*/
		TNamedNodeMap( vector<TNodePtr> * aNodeList, TNodePtr aMappedNode );
		/// Destructor of the TNamedNodeMap class.
		/**
			The destructor of the TNamedNodeMap class does nothing.
			Everything is cleaned up automatically.
		*/
		~TNamedNodeMap();

		/// A function to get the length of the NamedNodeMap.
		unsigned long getLength();

		/// A function to get a named item.
		/**
			This function gets the named item with the name supplied by the first argument and returns it.
			It returns a null node if the item can't be found.
			
			@param	aName	The name of the item to retrieve.
		*/
		TNodePtr getNamedItem( const TDOMString aName );
		
		/// A function to set a named item.
		/**
			This function sets a named item to the supplied argument. If there already is a item
			with this name, it is replaced by the supplied argument.
			
			@param	aArg	The item to set.
			
			@exception	HIERARCHY_REQUEST_ERR	Thrown if the item to be added doesn't belong
																		in the named node map.
			@exception	INUSE_ATTRIBUTE_ERR		Thrown if the item to be added is an attribute and is already
																	an attribute of another element.

			@todo	@li Have to check if the node to be added is from a different document
						than this one.
						@li Have to check if this map is readonly.
		*/
		TNodePtr setNamedItem( TNodePtr aArg );
		
		/// A function to remove a named item.
		/**
			This function removes a named item whose name is the value of the supplied argument.
			If this NamedNodeMap contains the attributes for an element and the attribute that is removed
			has a default value a new one is immediately created.
			
			@param	aName	The name of the item to remove.
			
			@exceptions	NOT_FOUND_ERR	Thrown if the named item can't be found.
			
			@todo	@li Have to check if this map is readonly.
						@li Have to create a new attribute if there is a default value.
			
		*/
		TNodePtr removeNamedItem( const TDOMString aName );
		
		/// A function to get an item.
		/**
			This function gets the item that is indexed at the value of the supplied argument.
			If the index is greater than the number of items in the map, a null item is returned.
			
			@param	aIndex	The index at which to get the item.
		*/
		TNodePtr item( unsigned long aIndex );
		// TNode * getNamedItemNS( const DOMString aNameSpaceURI, const DOMString aLocalName ); // Not yet implemented
		// TNode * setNamedItemNS( TNode aArg ); // Not yet implemented
		// TNode * removeNamedItemNS( const DOMString aNameSpaceURI, const DOMString aLocalName ); // Not yet implemented
		
};

#endif
