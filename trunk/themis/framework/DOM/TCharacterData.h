/*	CharacterData implementation of the DOM core
	See the DOM core specification for more information
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	10-02-2002
*/

#ifndef TCHARACTERDATA_H
#define TCHARACTERDATA_H

#include "DOMSupport.h"
#include "TNode.h"

class TCharacterData	:	public TNode	{
	
	private:
		TDOMString mData;
		// Not implemented variable. Implemented using function of mData
		// int length;
		// ---------------------------------------------------------------------------
		
	public:
		TCharacterData( const unsigned short aNodeType,
								 const TDocumentWeak aOwnerDocument,
								 const TDOMString aData = "" );
		~TCharacterData();
		TDOMString getData() const;
		void setData( const TDOMString aData );
		int getLength() const;
		TDOMString substringData( const unsigned long aOffset, const unsigned long aCount ) const;
		void appendData( const TDOMString aArg );
		void insertData( const unsigned long aOffset, const TDOMString aArg );
		void deleteData( const unsigned long aOffset, const unsigned long aCount );
		void replaceData( const unsigned long aOffset, const unsigned long aCount, const TDOMString aArg );
		
};

#endif
