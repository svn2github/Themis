/*	Node implementation of the DOM core
	See the DOM core specification for more information
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	11-12-2001
*/

#ifndef TNODE_H
#define TNODE_H

#include <List.h>

#include <stdio.h>

#include "DOMSupport.h"

class TNodeList;
class TNamedNodeMap;

class TNode	{
	
	private:
		unsigned short mNodeType;

		TDOMString * mNodeName;
		TDOMString * mNodeValue;
		
		TNodeList * mChildNodes;
		TNode * mParentNode;
		TNode * mNextSibling;
		TNode * mPreviousSibling;
		// Not implemented variables. Implemented using functions of mNodeList
		// const TNode * firstChild;
		// const TNode * LastChild;
		// ---------------------------------------------------------------------------------

		TNamedNodeMap * mAttributes;
		// TDocument ownerDocument;		Not yet implemented
		// TDOMString namespaceURI;			Not yet implemented
		// TDOMString prefix;						Not yet implemented
		// TDOMString localName;				Not yet implemented
		// TDOMString baseURI;					Not yet implemented
		// TDOMString textContent;			Not yet implemented

		// Support attributes
		BList * mNodeList;
		BList * mAttributeList;
		TDOMString mNodeTypeString;

		// Support function
		bool isAncestor( const TNode * aNode ) const;
		
	protected:
		// Support function
		bool isChildAllowed( const TNode * aNewChild ) const;
		void setNodeName( const TDOMString aValue );
		
	public:
		TNode( const unsigned short aNodeType, const TDOMString aNodeName = "", const TDOMString aNodeValue = "" );
		~TNode();
		unsigned short getNodeType() const;
		const TDOMString * getNodeName() const;
		const TDOMString * getNodeValue() const;
		void setNodeValue( const TDOMString aNodeValue );
		TNodeList * getChildNodes() const;
		TNode * getParentNode() const;
		TNode * getFirstChild() const;
		TNode * getLastChild() const;
		TNode * getNextSibling() const;
		TNode * getPreviousSibling() const;
		TNamedNodeMap * getAttributes() const;
		TNode * insertBefore( TNode * aNewChild, TNode * aRefChild );	// Not yet fully implemented
		TNode * replaceChild( TNode * aNewChild, TNode * aOldChild);	// Not yet fully implemented
		TNode * removeChild( TNode * aOldChild );	// Not yet fully implemented
		TNode * appendChild( TNode * aNodeChild );		// Not yet fully implemented
		bool hasChildNodes() const;
		TNode * cloneNode( bool aDeep ) const;	// Not yet fully implemented
		// void normalize();		Not yet implemented
		// bool isSupported( TDOMString aFeature, TDOMString aVersion );	Not yet implemented
		bool hasAttributes() const;
		// unsigned short compareTreePosition( TNode aOther );	Not yet implemented
		bool isSameNode( const TNode * aOther ) const;	// Not yet fully implemented
		// TDOMString lookupNamespacePrefix( TDOMString namespaceURI );	Not yet implemented
		// TDOMString loopupNamespaceURI( TDOMString prefix );	Not yet implemented
		// void normalizeNS();		Not yet implemented
		// bool isEqualNode( TNode aArg, bool aDeep );		Not yet implemented
		// TNode * getInterface( TDOMString feature );		Not yet implemented
		// TDOMObject * setUserData( TDOMString aKey, TDOMObject aData, TUserDataHandler aHandler );	Not yet implemented
		// TDOMObject * getUserData( TDOMString aKey );	Not yet implemented

		// Support functions		
		const char * getNodeTypeString() const;

};

#endif
