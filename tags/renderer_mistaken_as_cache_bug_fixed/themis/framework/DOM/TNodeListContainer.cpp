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
	Class Start Date: March 25, 2002
*/

/*
	NodeListContainer implementation
	See TNodeListContainer.h for some more information
*/

// Standard C++ headers
#include <string>

// DOM headers
#include "TNodeListContainer.h"
#include "TNode.h"
#include "TNodeList.h"

TNodeListContainer	::	TNodeListContainer( const TDOMString aQueryString, vector<TNodePtr> * aNodes, unsigned short aNodeType )	{

	mNodes = aNodes;
	mNodeList = TNodeListPtr( new TNodeList( mNodes ) );
	mQueryString = aQueryString;
	mNodeType = aNodeType;
	
}

TNodeListContainer	::	~TNodeListContainer()	{

	delete mNodes;

}

TDOMString TNodeListContainer	::	getQueryString() const	{
	
	return mQueryString;
	
}

unsigned short TNodeListContainer	::	getNodeType() const	{
	
	return mNodeType;
	
}

void TNodeListContainer	::	addNode( TNodePtr aNode )	{

	vector<TNodePtr>::iterator iter;
	iter = find( mNodes->begin(), mNodes->end(), aNode );
	if ( iter == mNodes->end() )	{
		mNodes->push_back( aNode );
	}
	
}

TNodePtr TNodeListContainer	::	removeNode( TNodePtr aNode )	{
	
	vector<TNodePtr>::iterator iter;
	iter = find( mNodes->begin(), mNodes->end(), aNode );
	if ( iter != mNodes->end() )	{
		mNodes->erase( iter );
	}

	return aNode;

}

TNodeListPtr TNodeListContainer	::	getNodeList()	{
	
	return mNodeList;
	
}
