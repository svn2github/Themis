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
	Class Start Date: February 14, 2002
*/

#ifndef TATTR_H
#define TATTR_H

// DOM headers
#include "DOMSupport.h"
#include "TNode.h"

/// Attr implementation of the DOM core.

/**
	The Attr class represents an attribute in an Element in a document tree.
	For an extensive explanation, see the DOM core pages at http://www.w3.org/DOM
*/

class TAttr		:	public TNode	{
	
	private:
		/// The name of the attribute.
		TDOMString mName;
		/// Boolean which represents wether the value of the attribute is specified.
		bool mSpecified;
		/// The value of the attribute.
		TDOMString mValue;
		/// The owner element of the attribute.
		TElementWeak mOwnerElement;
		
		/// The default value of the attribute.
 		/// This is a support variable and is not in the DOM core specification.
		TDOMString mDefaultValue;
		
	public:
		/// Constructor of the TAttr class.
		/**
			The constructor of the TAttr class first calls the TNode constructor and
			passes part of its' arguments to it. All supplied arguments but the second supplied
			argument are used to set values in the TAttr constructor.
			
			@param	aName	The name of the attribute.
			@param	aOwnerDocument	The owner document of this attribute.
			@param	aSpecified	A boolean that indicates wether the attribute is specified.
			@param	aValue	The value of the attribute.
			@param	aOwnerElement	The owner element of this attribute.
			
			@todo	Have a look at how the specified attribute is supposed to work. The documentation
						is a bit fuzzy.
		*/
		TAttr( const TDOMString aName, const TDocumentPtr aOwnerDocument,
				  const bool aSpecified = false, const TDOMString aValue = "",
				  TElementPtr aOwnerElement = TElementPtr() );
		
		/// Destructor of the TAttr class.
		/**
			The destructor of the TAttr class does nothing. Everything is cleaned up
			automatically.
		*/
		~TAttr();

		/// A function to get the name of the attribute.
		TDOMString getName() const;
		
		/// A function to get the specified value of the attribute.
		bool getSpecified() const ;
		
		/// A function to get the value of the attribute.
		TDOMString getValue() const ;
		
		/// A function to set the value of the attribute.
		/**
			This function sets the value of the attribute to the value supplied of the argument.
			
			@param	aValue	The value to set the value of the attribute to.
			
			@todo	@li Have to create a text node when setting the value, apparently.
			@todo	@li Have to check if this attribute is readonly.
		*/
		void setValue( const TDOMString aValue );
		
		/// A function to get the owner element of this attribute.
		TElementPtr getOwnerElement() const;
		
		/// A declaration to indicate TNamedNodeMap is a friend of this class.
		friend class TNamedNodeMap;

		/// A function to get the default value of this attribute.
		/// This is a support function and is not in the DOM core specification.
		TDOMString getDefaultValue() const;

};

#endif
