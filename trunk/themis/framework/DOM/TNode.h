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
	
	Original Author: 	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	Project Start Date: October 18, 2000
	Class Start Date: December 11, 2001
*/

#ifndef TNODE_H
#define TNODE_H

// Standard C++ headers
#include <vector>

// Boost headers
#include "boost/enable_shared_from_this.hpp"

// DOM headers
#include "DOMSupport.h"

// Namespaces used
using namespace std;
using namespace boost;

/// Node implementation of the DOM core.

/**
	The Node class forms the basis for most of the classes
	of the DOM core. It represents a node in a document tree.
	For an extensive explanation, see the DOM core pages at http://www.w3.org/DOM
*/

class TNode	:	public enable_shared_from_this<TNode>	{

	private:
		/// The type of the node.
		/**
			The type of the node which can currently be one of the following:
			- ELEMENT_NODE
			- ATTRIBUTE_NODE
			- TEXT_NODE
			- CDATA_SECTION_NODE
			- ENTITY_REFERENCE_NODE
			- ENTITY_NODE
			- PROCESSING_INSTRUCTION_NODE
			- COMMENT_NODE
			- DOCUMENT_NODE
			- DOCUMENT_TYPE_NODE
			- DOCUMENT_FRAGMENT_NODE
			- NOTATION_NODE
		*/
		unsigned short mNodeType;

		/// The name of the node.
		TDOMString mNodeName;
		/// The value of the node.
		TDOMString mNodeValue;
		
		/// The children of the node.
		TNodeListPtr mChildNodes;
		/// The parent of the node.
		TNodeWeak mParentNode;
		/// The next sibling of the node.
		TNodeWeak mNextSibling;
		/// The previous sibling of the node.
		TNodeWeak mPreviousSibling;
		// Not implemented variables. Implemented using functions of mNodeList
		// const TNode * firstChild;
		// const TNode * LastChild;
		// ---------------------------------------------------------------------------------

		/// The attributes ot the node.
		TNamedNodeMapPtr mAttributes;
		/// The owner document of the node.
		TDocumentWeak mOwnerDocument;
		// TDOMString namespaceURI;			Not yet implemented
		// TDOMString prefix;						Not yet implemented
		// TDOMString localName;				Not yet implemented
		// TDOMString baseURI;					Not yet implemented
		// TDOMString textContent;			Not yet implemented

		/// The list of children of the node.
 		/// This is a support variable and is not in the DOM core specification.
 		vector<TNodePtr> mNodeList;
		/// The list of attributes of the node.
		/// This is a support variable and is not in the DOM core specification.
		vector<TNodePtr> mAttributeList;
		/// The type of the node stored as a string.
		/// This is a support variable and is not in the DOM core specification.
 		TDOMString mNodeTypeString;

		/// A function to check if the supplied node is an ancestor of this node.
		/**
			This function checks if the supplied node is an ancestor of this node.
			It does this by checking the parent of the node to the supplied node and then
			recursively going up the tree and making the check, until either a parent
			is the same as the supplied node or the root of the tree is reached.
			If a parent is the same it will return true, else it will return false.
			This is a support function and is not in the DOM core specification.
			
			@param	aNode	The supplied node.
		*/
		bool isAncestor( const TNodePtr aNode ) const;
		
	protected:
		/// List of containers to put TNodeLists in.
		/// This is a support variable and is not in the DOM core specification.
		vector<TNodeListContainerPtr> mNodeListContainers;
		
		/// A function to check if a child is allowed for the node.
		/**
			This function checks if the supplied node is allowed as
			a child of the node. This depends on the node type of both
			the supplied node and this node. Some node types can't have
			children and some node types are not allowed as children of a
			particular node type. Check the DOM core specification for the details
			or look at the source code for this function.
			This is a support function and is not in the DOM core specification.
			
			@param	aNewChild	The supplied node.
		*/
		bool isChildAllowed( const TNodePtr aNewChild ) const;
		
		/// A function to set the node name of the node.
		/**
			This function sets the name of the node to the value supplied.
			This is a support function and is not in the DOM core specification.
			
			@param	aValue	The new value for the name.
		*/
		void setNodeName( const TDOMString aValue );
		
		/// A function to notify the node and its' parents that another node's status has changed
		/**
			This function notifies this node and all it's parents that the supplied node
			has either been added or removed.
			This is used to keep track of nodes which match a query requested with
			the getElementsByTagName function of the TElement class.
			This is a support function and is not in the DOM core specification.
			
			@param	aNode	The node that has changed status.
			@param	aChange	The status change of the node.
											Either NODE_ADDED or NODE_REMOVED.
		*/
		void nodeChanged( TNodePtr aNode, NodeChange aChange );
		
		/// A function to to notify the node and its' parents that another node and
		/// its' children have changed status
		/**
			This function notifies this node and all it's parents that the supplied node
			and its' children has either been added or removed.
			This is used to keep track of nodes which match a query requested with
			the getElementsByTagName function of the TElement class.
			This is a support function and is not in the DOM core specification.
			
			@param	aNode	The node that has changed status.
			@param	aChange	The status change of the node.
											Either NODE_ADDED or NODE_REMOVED.
		*/
		void notifyNodeChange( TNodePtr aNode, NodeChange aChange );
		
		/// A function to collect the nodes in the subtree of this node of a certain type and name.
		/**
			This function collects all the nodes in the subtree of this node which correspond to
			the supplied name and are of the supplied type. The supplied name can be the
			exact name to look for or '*' which collects all the nodes of the supplied type.
			This is a support function and is not in the DOM core specification.
			
			@param	aName	The name of the nodes to collect.
			@param	aNodeType	The type of node to look for.
		*/
		vector<TNodePtr> collectNodes( TDOMString aName, unsigned short aNodeType );
		
	public:
		/// Constructor of the TNode class.
		/**
			The constructor of the TNode class. It sets up all the necessary resources.
			Should not be called directly, but used as a base class for the possible node types.
			
			@param	aNodeType	The type the node will have.
			@param	aOwnerDocument	The owner of the node.
			@param	aNodeName	The name of the node.
			@param	aNodeValue	The value of the node
		*/
		TNode( const unsigned short aNodeType,
				   const TDocumentPtr aOwnerDocument,
				   const TDOMString aNodeName = "",
				   const TDOMString aNodeValue = "" );
		
		/// Destructor of the TNode class.
		/**
			The destructor of the TNode class. It destroys all the resources used by the instance
			and destroys all the children of the instance.
			
			@todo	Look at the removeChild call. It might make unneeded calls to notifyNodeChange
		*/
		~TNode();
		
		/// A function to get the node type of the node.
		/**
			This function returns the node type of the node.
			The possible values are listed in the documentation for mNodeType.
		*/
		unsigned short getNodeType() const;
		
		/// A function to get the name of the node.
		const TDOMString getNodeName() const;
		
		/// A function to get the value of the node.
		/**
			This function gets the value of the node. Only certain types of nodes can
			have a value. These are:
			- ATTRIBUTE_NODE
			- CDATA_SECTION_NODE
			- COMMENT_NODE
			- PROCESSING_INSTRUCTION_NODE
			- TEXT_NODE
		*/
		const TDOMString getNodeValue() const;
		
		/// A function to set the value of the node.
		/**
			This function sets the value of the node to the supplied value. This is only
			done if this type of node is allowed to have a value. See the documentation
			for getNodeValue for which types of node can have a value.
			
			@param	aNodeValue	The value of the node.
		*/
		void setNodeValue( const TDOMString aNodeValue );
		
		/// A function to get a list of the child nodes of this node.
		TNodeListPtr getChildNodes() const;
		
		/// A function to get the parent of this node.
		TNodePtr getParentNode() const;
		
		/// A function to get the first child of this node.
		TNodePtr getFirstChild() const;
		
		/// A function to get the last child of this node.
		TNodePtr getLastChild() const;
		
		/// A function to get the next sibling of this node.
		TNodePtr getNextSibling() const;

		/// A function to get the previous sibling of this node.
		TNodePtr getPreviousSibling() const;
		
		/// A function to get the attributes of this node.
		TNamedNodeMapPtr getAttributes();
		
		/// A function to get the owner document of this node.
		TDocumentPtr getOwnerDocument() const;
		
		/// A function to insert a node before another node in the list of children.
		/**
			This function inserts the first supplied node before the second supplied
			node in the list of children for this node. If the first supplied node is already in the
			tree, it is removed first. If the second node is NULL the first node is added at the
			end of the list of children.
			
			@param	aNewChild	The node to add to the list
			@param	aRefChild	The node before which the first node must be added
			
			@exception	HIERARCHY_REQUEST_ERR	Thrown if this node does not allow children
																		or if this node or an ancestor is the same
																		as the node to insert or if this node is a
																		TDocument and the node to be inserted is
																		the second TDocumentType or Element node.
			@exception	NOT_FOUND_ERR	Thrown if the second supplied node is not found
															in the list of children of this node.
			
			@todo	@li Have to check if the node to be inserted is a TDocumentFragment node
						and only add its' children then.
						@li Have to check if the node is a TDocument and then check if the node
						to be inserted is the second TDocumentType or Element node.
						@li Have to check if the node to be added is from a different document
						than this one.
						@li Have to check if this node is readonly or if the parent of the node to be
						inserted is readonly.
			
		*/
		TNodePtr insertBefore( TNodePtr aNewChild, TNodePtr aRefChild );
		
		/// A function to replace a child of this node.
		/**
			This function replaces the second supplied node with the first supplied node.
			If the first supplied node is already in the tree, it is removed first.
			
			@param	aNewChild	The node to replace the old node with.
			@param	aOldChild	The old node that needs to be replaced.
			
			@exception	HIERARCHY_REQUEST_ERR	Thrown if this node does not allow children
																		or if this node or an ancestor is the same
																		as the node to insert or if this node is a
																		TDocument and the node to be inserted is
																		the second TDocumentType or Element node.
			@exception	NOT_FOUND_ERR	Thrown if the second supplied node is not found
															in the list of children of this node.

			@todo	@li Have to check if the node to be inserted is a TDocumentFragment node
						and only replace its' children then for the old node.
						@li Have to check if the node is a TDocument and then check if the node
						to be inserted is the second TDocumentType or Element node.
						Warning about the last one. Isn't actually in the DOM core spec.
						Have to ask about it, I think.
						@li Have to check if the node to be added is from a different document
						than this one.
						@li Have to check if this node is readonly or if the parent of the node to be
						inserted is readonly.
		*/
		TNodePtr replaceChild( TNodePtr aNewChild, TNodePtr aOldChild);
		
		/// A function to remove a child from this node.
		/**
			This function removes the supplied node from the list of children and returns it.
			
			@param	aOldChild	The node to remove from the list of children.
			
			@exception	NOT_FOUND_ERR	Thrown if the supplied node is not found
															in the list of children of this node.
			
			@todo	@li Have to check if this node is readonly or if the parent of the node to be
						inserted is readonly.
		*/
		TNodePtr removeChild( TNodePtr aOldChild );
		
		/// A function to append a node to the list of children.
		/**
			This function appends the supplied node to the end of the list of children.
			If the first supplied node is already in the tree, it is removed first.
			
			@param	aNodeChild	The node to append to the end of the list of children.
			
			@exception	HIERARCHY_REQUEST_ERR	Thrown if this node does not allow children
																		or if this node or an ancestor is the same
																		as the node to insert or if this node is a
																		TDocument and the node to be inserted is
																		the second TDocumentType or Element node.
			@todo	@li Have to check if the node to be inserted is a TDocumentFragment node
						and only replace its' children then for the old node.
						@li Have to check if the node is a TDocument and then check if the node
						to be inserted is the second TDocumentType or Element node.
						Warning about the last one. Isn't actually in the DOM core spec.
						Have to ask about it, I think.
						@li Have to check if the node to be added is from a different document
						than this one.
						@li Have to check if this node is readonly or if the parent of the node to be
						inserted is readonly.
		*/
		TNodePtr appendChild( TNodePtr aNodeChild );
		
		/// A function to check if this node has child nodes.
		bool hasChildNodes() const;
		
		/// A function to clone the node.
		/**
			This function clones this node. If the supplied boolean is true, a deep copy of the
			node will be made. A deep copy will also copy all of the children of the node.
			The copied node does not have a parent and no user data.
			There are some rules on what will be copied if the supplied boolean is false.
			For a detailed explanation of these rules, see the DOM3 core specifcation.
			
			@param	aDeep	The boolean that indicates if a deep copy must be made or not.
			
			@todo	Currently makes a basic deep copy of this node,
						but will need to check if more values need to be set.
		*/
		TNodePtr cloneNode( bool aDeep ) const;
		// void normalize();		Not yet implemented
		// bool isSupported( TDOMString aFeature, TDOMString aVersion );	Not yet implemented
		
		/// A function to check if this node has attributes.
		bool hasAttributes();
		// unsigned short compareTreePosition( TNode aOther );	Not yet implemented
		
		/// A function to check if two nodes are the same.
		/**
			This function checks if the supplied node is the same as this node.
			If they are the same, using a function on one node has the same effect
			on the other one.
			
			@param	aOther	The node to compare to this node.
		*/
		bool isSameNode( const TNodePtr aOther ) const;

		// TDOMString lookupNamespacePrefix( TDOMString namespaceURI );	Not yet implemented
		// TDOMString loopupNamespaceURI( TDOMString prefix );	Not yet implemented
		// void normalizeNS();		Not yet implemented
		// bool isEqualNode( TNode aArg, bool aDeep );		Not yet implemented
		// TNode * getInterface( TDOMString feature );		Not yet implemented
		// TDOMObject * setUserData( TDOMString aKey, TDOMObject aData, TUserDataHandler aHandler );	Not yet implemented
		// TDOMObject * getUserData( TDOMString aKey );	Not yet implemented

		/// A function that returns the node type as a string.
		/**
			This function returns the node type of this node as a string.
			This has no other purpose than debugging.
			This is a support function and is not in the DOM core specification.
		*/
		const char * getNodeTypeString() const;

};

#endif
