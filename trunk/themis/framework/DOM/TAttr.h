/*	Attr implementation of the DOM core
	See the DOM core specification for more information
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	14-02-2002
*/

#ifndef TATTR_H
#define TATTR_H

#include "DOMSupport.h"
#include "TNode.h"

class TElement;

class TAttr		:	public TNode	{
	
	private:
		TDOMString mName;
		bool mSpecified;
		TDOMString mValue;
		TElement * mOwnerElement;
		
		// Support variable
		TDOMString mDefaultValue;
		
	public:
		TAttr( const TDOMString aName, const bool aSpecified, const TDOMString aValue, TElement * aOwnerElement );
		~TAttr();
		TDOMString getName() const;
		bool getSpecified() const ;
		TDOMString getValue() const ;
		void setValue( const TDOMString aValue );
		TElement * getOwnerElement() const;
		friend class TNamedNodeMap;

		// Suuport function
		TDOMString getDefaultValue() const;

};

#endif
