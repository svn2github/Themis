/*	Comment implementation of the DOM core
	See the DOM core specification for more information
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	29-06-2002
*/

#ifndef TCOMMENT_H
#define TCOMMENT_H

#include "DOMSupport.h"
#include "TCharacterData.h"

class TComment	:	public TCharacterData	{
	
	public:
		TComment( const TDocumentWeak aOwnerDocument, const TDOMString aData )	:
				TCharacterData( COMMENT_NODE, aOwnerDocument, aData )	{};
		~TComment()	{};
		
};

#endif
