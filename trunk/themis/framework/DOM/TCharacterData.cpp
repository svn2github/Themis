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
	Class Start Date: February 10, 2002
*/

/*
	CharacterData implementation
	See TCharacterData.h for some more information
*/

// Standard C++ headers
#include <string>

// DOM headers
#include "TCharacterData.h"
#include "TDOMException.h"

// Namespaces used
using namespace std;

TCharacterData	::	TCharacterData( const unsigned short aNodeType,
													 const TDocumentWeak aOwnerDocument,
													 const TDOMString aData )	:
													 		TNode( aNodeType, aOwnerDocument, "", aData )	{

	mData = aData;
	
}

TCharacterData	::	~TCharacterData()	{
	
}

TDOMString TCharacterData	::	getData() const	{
	
	return mData;
	
}

void TCharacterData	::	setData( const TDOMString aData )	{

	mData = aData;
	
}

int TCharacterData	::	getLength() const	{
	
	return mData.size();
	
}

TDOMString TCharacterData	::	substringData( const unsigned long aOffset, const unsigned long aCount ) const	{
	
	// Check if requested substring can be returned.
	// Negative values are impossible and are not checked
	if ( aOffset > (unsigned long) ( mData.size() - 1 ) )	{
		throw TDOMException( INDEX_SIZE_ERR );
	}

	return mData.substr( aOffset, aCount );
	
}

void TCharacterData	::	appendData( const TDOMString aArg )	{

	mData += aArg;
	
}

void TCharacterData	::	insertData( const unsigned long aOffset, const TDOMString aArg )	{
	
	// Check if string can be inserted at the requested offset.
	// Negative values are impossible and are not checked
	if ( aOffset > (unsigned long) ( mData.size() - 1 ) )	{
		throw TDOMException( INDEX_SIZE_ERR );
	}

	mData.insert( aOffset, aArg );
	
}

void TCharacterData	::	deleteData( const unsigned long aOffset, const unsigned long aCount )	{
	
	// Check if string can be deleted at the requested offset.
	// Negative values are impossible and are not checked
	if ( aOffset > (unsigned long) ( mData.size() - 1 ) )	{
		throw TDOMException( INDEX_SIZE_ERR );
	}

	mData.erase( aOffset, aCount );
	
}

void TCharacterData	::	replaceData( const unsigned long aOffset, const unsigned long aCount, const TDOMString aArg )	{

	// Check if string can be replaced at the requested offset.
	// Negative values are impossible and are not checked
	if ( aOffset > (unsigned long) ( mData.size() - 1 ) )	{
		throw TDOMException( INDEX_SIZE_ERR );
	}
	
	deleteData( aOffset, aCount );
	mData.insert( aOffset, aArg, 0, aCount );
	
}
