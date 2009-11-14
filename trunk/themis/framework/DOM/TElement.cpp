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
	Class Start Date: February 03, 2002
*/

/*
	Element implementation
	See TElement.h for some more information
*/

// Standard C++ headers
#include <string>

// DOM headers
#include "TElement.h"
#include "TNamedNodeMap.h"
#include "TAttr.h"
#include "TNodeListContainer.h"
#include "TNodeList.h"

// Namespaces used
using namespace std;

TElement	::	TElement( const TDocumentPtr aOwnerDocument, const TDOMString aTagName )
				:	TNode( ELEMENT_NODE, aOwnerDocument, aTagName )	{
	
	mTagName = aTagName;
	
}

TElement	::	~TElement()	{
	
}

TDOMString TElement	::	getTagName() const	{
	
	return mTagName;
	
}

TDOMString TElement	::	getAttribute( const TDOMString aName )	{
	
	TNodePtr attribute = getAttributes()->getNamedItem( aName );
	
	if ( !attribute.get() )	{
		return TDOMString( "" );
	}
	
	return attribute->getNodeValue();
	
}
	
void TElement	::	setAttribute( const TDOMString aName, const TDOMString aValue )	{
	
	TAttrPtr attribute = shared_static_cast<TAttr> ( getAttributes()->getNamedItem( aName ) );
	if ( ! attribute.get() )	{
		// No attribute with that name yet, so create one.
		attribute = TAttrPtr( new TAttr( aName, getOwnerDocument(), true, aValue,
									   shared_static_cast<TElement>( shared_from_this() ) ) );
		getAttributes()->setNamedItem( attribute );
	}
	else	{
		attribute->setValue( aValue );
	}
}

void TElement	::	removeAttribute( const TDOMString aName )	{

	TAttrPtr attribute = shared_static_cast<TAttr> ( getAttributes()->getNamedItem( aName ) );
	if ( attribute.get() == NULL )	{
		// No attribute exists with this name. Do nothing.
		return;
	}
	TDOMString defaultValue = attribute->getDefaultValue();
	if ( defaultValue != "" )	{
		attribute->setValue( defaultValue );
	}
	else	{
		getAttributes()->removeNamedItem( aName );
	}

}

TAttrPtr TElement	::	getAttributeNode( const TDOMString aName )	{
	
	return shared_static_cast<TAttr> ( getAttributes()->getNamedItem( aName ) );
	
}

TAttrPtr TElement	::	setAttributeNode( TAttrPtr aNewAttr )	{
	
	return shared_static_cast<TAttr> ( getAttributes()->setNamedItem( aNewAttr ) );
	
}

TAttrPtr TElement	::	removeAttributeNode( TAttrPtr aOldAttr )	{
	
	getAttributes()->removeNamedItem( aOldAttr->getNodeName() );
	TDOMString defaultValue = aOldAttr->getDefaultValue();
	if ( defaultValue != "" )	{
		TAttrPtr newAttribute = TAttrPtr( new TAttr( aOldAttr->getName(), aOldAttr->getOwnerDocument(),
														   aOldAttr->getSpecified(), aOldAttr->getValue(), aOldAttr->getOwnerElement() ) );
		getAttributes()->setNamedItem( newAttribute );
	}

	return aOldAttr;

}

TNodeListPtr TElement	::	getElementsByTagName( const TDOMString aName )	{
	
	// Look at this one some more.
	
	vector<TNodeListContainerPtr>::iterator current;
	for ( current = mNodeListContainers.begin(); current != mNodeListContainers.end(); current++ )	{
		if ( current->get()->getNodeType() == ELEMENT_NODE )	{
			if ( current->get()->getQueryString() == aName )	{
				// There already is a NodeList with this querystring
				// Get a pointer to the nodelist
				return current->get()->getNodeList();
			}
		}
	}
		
	// Not one NodeList with query aName exists yet. Create it.
	vector<TNodePtr> * resultList = new vector<TNodePtr>( collectNodes( aName, ELEMENT_NODE ) );
	TNodeListContainerPtr container( new TNodeListContainer( aName, resultList, ELEMENT_NODE ) );
	mNodeListContainers.push_back( container );
	
	return container->getNodeList();
	
}

bool TElement	::	hasAttribute( const TDOMString aName )	{

	TNodePtr node = getAttributes()->getNamedItem( aName );
	if ( node.get() )	{
		return true;
	}
	
	return false;
	
}
