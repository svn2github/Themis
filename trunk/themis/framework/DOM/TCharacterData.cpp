/* CharacterData implementation
	See TCharacterData.h for some more information
*/

#include <string>

#include "TCharacterData.h"
#include "TDOMException.h"

TCharacterData	::	TCharacterData( const unsigned short aNodeType, const TDOMString aData )	:	TNode( aNodeType )	{
	
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
