/*	Document implementation of the DOM core
	See the DOM core specification for more information
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	03-02-2002
*/

#ifndef TDOCUMENT_H
#define TDOCUMENT_H

#include "DOMSupport.h"
#include "TNode.h"

class TDocument	:	public TNode	{
	
	private:
		// DocumentType doctype // Not yet implemented
		// DOMImplementation implementation // Not yet implemented
		// Element documentElement // Not yet implemented

	public:
		TDocument();
		~TDocument();
		TElementShared createElement( const TDOMString aTagName );
		//DocumentFragment createDocumentFragment(); // Not yet implemented
		TTextShared createText( const TDOMString aData );
		TCommentShared createComment( const TDOMString aData );
		//CDataSection createCDataSection( const TDOMString aData ); // Not yet implemented
		//ProcessingInstruction createProcessingInstruction( const TDOMString aTarget, const TDOMString aData ); // Not yet implemented
		TAttrShared createAttribute( const TDOMString aName );
		//EntityReference createEntityReference( const TDOMString aName ); // Not yet implemented
		//TNodeListShared getElementsByTagName( const TDOMString aTagName ); // Not yet implemented

};

#endif
