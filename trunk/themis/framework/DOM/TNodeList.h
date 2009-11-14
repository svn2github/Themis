/*
	Copyright (c) 2001 Mark Hellegers. All Rights Reserved.
	
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
	Class Start Date: December 12, 2001
*/

#ifndef TNODELIST_H
#define TNODELIST_H

// Standard C++ headers
#include <vector>

// DOM headers
#include "DOMSupport.h"

/// NodeList implementation of the DOM core.

/**
	The NodeList class stores a collection of nodes.
	For an extensive explanation, see the DOM core pages at http://www.w3.org/DOM
*/

class TNodeList	{
	
	private:
		/// The list of nodes.
 		/// This is a support variable and is not in the DOM core specification.
		const vector<TNodePtr> * mNodeList;
		// Not implemented variable. Implemented using function in mNodeList
		// unsigned long length;
		// ---------------------------------------------------------------------------------
		
	public:
		/// Constructor of the TNodeList class.
		/**
			The constructor of the TNodeList class. It stores the pointer to
			the node list.
			
			@param	aNodeList	The list of nodes to store.
		*/
		TNodeList( const vector<TNodePtr> * aNodeList );
		
		/// Destructor of the TNodeList class.
		/**
			The destructor of the TNodeList class does nothing. Everything is cleaned up
			automatically.
		*/
		~TNodeList();
		
		/// A function to get the number of nodes in the list.
		unsigned long getLength();
		
		/// A function to get an item.
		/**
			This function gets the item that is indexed at the value of the supplied argument.
			If the index is greater than the number of items in the map, a null item is returned.

			@param	aIndex	The index at which to get the item.
		*/
		TNodePtr item( unsigned long aIndex );
		
};

#endif
