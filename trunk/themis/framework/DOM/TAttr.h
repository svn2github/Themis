/*	Attr implementation of the DOM core
	See the DOM core specification for more information
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	14-02-2002
*/

#ifndef TATTR_H
#define TATTR_H

#include "DOMSupport.h"
#include "TNode.h"

class TAttr		:	public TNode	{
	
	private:
		TDOMString mName;
		bool mSpecified;
		TDOMString mValue;
		TElementWeak mOwnerElement;
		
		// Support variable
		TDOMString mDefaultValue;
		
	public:
		TAttr( const TDOMString aName, const TDocumentWeak aOwnerDocument,
				  const bool aSpecified = false, const TDOMString aValue = "",
				  TElementWeak aOwnerElement = TElementWeak() );
		~TAttr();
		TDOMString getName() const;
		bool getSpecified() const ;
		TDOMString getValue() const ;
		void setValue( const TDOMString aValue );
		TElementWeak getOwnerElement() const;
		friend class TNamedNodeMap;

		// Suuport function
		TDOMString getDefaultValue() const;

};

#endif
