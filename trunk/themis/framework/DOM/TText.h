/* Text implementation of the DOM core
	See the DOM core specification for more information
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	01-04-2002
*/

#ifndef TTEXT_H
#define TTEXT_H

#include "DOMSupport.h"
#include "TCharacterData.h"

class TText	:	public TCharacterData	{
	
	private:
		// Not implemented variables. Are constructed on the fly
		// TDOMString wholeText;
		// bool mIsWhiteSpaceInElement;
	
	public:
		TText( const TDocumentWeak aOwnerDocument, const TDOMString aText );
		~TText();
		bool getIsWhiteSpaceInElement() const; // Still have to look at this. Is not implemented well
		TDOMString getWholeText() const;
		TTextShared splitText( const unsigned long aOffset );
		TTextWeak replaceWholeText( const TDOMString aContent ); // Not done yet. Returns NULL
		
};

#endif
