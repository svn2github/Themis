/*	Node implementation of the DOM core
	See the DOM core specification for more information
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	11-12-2001
*/

#ifndef TNODE_H
#define TNODE_H

#include <stdio.h>

#include <vector>

#include "DOMSupport.h"

using namespace std;
using namespace boost;

class TNode	{

	private:
		unsigned short mNodeType;

		TDOMString mNodeName;
		TDOMString mNodeValue;
		
		TNodeListShared mChildNodes;
		TNodeWeak mParentNode;
		TNodeWeak mNextSibling;
		TNodeWeak mPreviousSibling;
		// Not implemented variables. Implemented using functions of mNodeList
		// const TNode * firstChild;
		// const TNode * LastChild;
		// ---------------------------------------------------------------------------------

		TNamedNodeMapShared mAttributes;
		TDocumentWeak mOwnerDocument;
		// TDOMString namespaceURI;			Not yet implemented
		// TDOMString prefix;						Not yet implemented
		// TDOMString localName;				Not yet implemented
		// TDOMString baseURI;					Not yet implemented
		// TDOMString textContent;			Not yet implemented

		// Support attributes
		vector<TNodeShared> mNodeList;
		vector<TNodeShared> mAttributeList;
		TDOMString mNodeTypeString;

		// Support function
		bool isAncestor( const TNodeShared aNode ) const;
		
	protected:
		// Support attribute
		TNodeWeak mThisPointer;
		vector<TNodeListContainerShared> mNodeListContainers;
		// Support functions
		bool isChildAllowed( const TNodeShared aNewChild ) const;
		void setNodeName( const TDOMString aValue );
		void nodeChanged( TNodeShared aNode, NodeChange aChange );
		void notifyNodeChange( TNodeShared aNotifyNode, NodeChange aChange );
		vector<TNodeShared> collectNodes( TDOMString aName, unsigned short aNodeType );
		
	public:
		TNode( const unsigned short aNodeType,
				   const TDocumentWeak aOwnerDocument,
				   const TDOMString aNodeName = "",
				   const TDOMString aNodeValue = "" );
		~TNode();
		unsigned short getNodeType() const;
		const TDOMString getNodeName() const;
		const TDOMString getNodeValue() const;
		void setNodeValue( const TDOMString aNodeValue );
		TNodeListShared getChildNodes() const;
		TNodeWeak getParentNode() const;
		TNodeWeak getFirstChild() const;
		TNodeWeak getLastChild() const;
		TNodeWeak getNextSibling() const;
		TNodeWeak getPreviousSibling() const;
		TNamedNodeMapShared getAttributes() const;
		TDocumentWeak getOwnerDocument() const;
		TNodeWeak insertBefore( TNodeShared aNewChild, TNodeShared aRefChild );	// Not yet fully implemented
		TNodeWeak replaceChild( TNodeShared aNewChild, TNodeShared aOldChild);	// Not yet fully implemented
		TNodeWeak removeChild( TNodeShared aOldChild );	// Not yet fully implemented
		TNodeWeak appendChild( TNodeShared aNodeChild );		// Not yet fully implemented
		bool hasChildNodes() const;
		TNodeShared cloneNode( bool aDeep ) const;	// Not yet fully implemented
		// void normalize();		Not yet implemented
		// bool isSupported( TDOMString aFeature, TDOMString aVersion );	Not yet implemented
		bool hasAttributes() const;
		// unsigned short compareTreePosition( TNode aOther );	Not yet implemented
		bool isSameNode( const TNodeShared aOther ) const;	// Not yet fully implemented
		// TDOMString lookupNamespacePrefix( TDOMString namespaceURI );	Not yet implemented
		// TDOMString loopupNamespaceURI( TDOMString prefix );	Not yet implemented
		// void normalizeNS();		Not yet implemented
		// bool isEqualNode( TNode aArg, bool aDeep );		Not yet implemented
		// TNode * getInterface( TDOMString feature );		Not yet implemented
		// TDOMObject * setUserData( TDOMString aKey, TDOMObject aData, TUserDataHandler aHandler );	Not yet implemented
		// TDOMObject * getUserData( TDOMString aKey );	Not yet implemented

		// Support functions		
		const char * getNodeTypeString() const;
		void setSmartPointer( TNodeShared pointer );

};

#endif
