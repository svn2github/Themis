/* CharacterData implementation
	See TCharacterData.h for some more information
*/

#include "TCharacterData.h"
#include "TDOMException.h"

TCharacterData	::	TCharacterData( const unsigned short aNodeType )	:	TNode( aNodeType )	{
	
}

TCharacterData	::	~TCharacterData()	{
	
}

TDOMString TCharacterData	::	getData() const	{
	
	return mData;
	
}

void TCharacterData	::	setData( const TDOMString aData )	{

	mData.SetTo( aData );
	
}

int TCharacterData	::	getLength() const	{
	
	return mData.CountChars();
	
}

TDOMString TCharacterData	::	substringData( const unsigned long aOffset, const unsigned long aCount ) const	{
	
	// Check if requested substring can be returned.
	// Negative values are impossible and are not checked
	if ( aOffset > (unsigned long) ( mData.CountChars() - 1 ) )	{
		throw TDOMException( INDEX_SIZE_ERR );
	}

	TDOMString result;
	return mData.CopyInto( result, aOffset, aCount );
	
}

void TCharacterData	::	appendData( const TDOMString aArg )	{

	mData.Append( aArg );
	
}

void TCharacterData	::	insertData( const unsigned long aOffset, const TDOMString aArg )	{
	
	// Check if string can be inserted at the requested offset.
	// Negative values are impossible and are not checked
	if ( aOffset > (unsigned long) ( mData.CountChars() - 1 ) )	{
		throw TDOMException( INDEX_SIZE_ERR );
	}

	mData.Insert( aArg, aOffset );
	
}

void TCharacterData	::	deleteData( const unsigned long aOffset, const unsigned long aCount )	{
	
	// Check if string can be deleted at the requested offset.
	// Negative values are impossible and are not checked
	if ( aOffset > (unsigned long) ( mData.CountChars() - 1 ) )	{
		throw TDOMException( INDEX_SIZE_ERR );
	}

	mData.Remove( aOffset, aCount );
	
}

void TCharacterData	::	replaceData( const unsigned long aOffset, const unsigned long aCount, const TDOMString aArg )	{

	// Check if string can be replaced at the requested offset.
	// Negative values are impossible and are not checked
	if ( aOffset > (unsigned long) ( mData.CountChars() - 1 ) )	{
		throw TDOMException( INDEX_SIZE_ERR );
	}
	
	deleteData( aOffset, aCount );
	mData.Insert( aArg, aCount, aOffset );
	
}
