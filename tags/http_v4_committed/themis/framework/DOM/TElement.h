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
	Class Start Date: February 03, 2002
*/

#ifndef TELEMENT_H
#define TELEMENT_H

// DOM headers
#include "DOMSupport.h"
#include "TNode.h"

/// Element implementation of the DOM core.

/**
	The Element class represents an element in a document tree.
	For an extensive explanation, see the DOM core pages at http://www.w3.org/DOM
*/

class TElement	:	public TNode	{
	
	private:
		/// Tag name of the element.
		TDOMString mTagName;
		
	public:
		/// Constructor of the TElement class.
		/**
			The constructor of the TElement class first calls the TNode constructor and
			passes all its' arguments to it. The second supplied argument is set to the
			mTagName member of this class.
			
			@param	aOwnerDocument	The owner document of this element.
			@param	aTagName	The tag name of this element.
		*/
		TElement( const TDocumentPtr aOwnerDocument,
					   const TDOMString aTagName );

		/// Destructor of the TElement class.
		/**
			The destructor of the TElement class does nothing. Everything is cleaned up
			automatically.
		*/
		~TElement();
		
		/// A function to get the tag name of the element.
		TDOMString getTagName() const;
		
		/// A function to get an attribute of the element.
		/**
			This function gets the attribute of this element which has the value of the
			supplied argument as a name and returns it. It will return an empty string if
			the attribute does not exist or if it does not have a specified or default value.
			
			@param	aName	The name of the attribute to return the value of.
		*/
		TDOMString getAttribute( const TDOMString aName );
		
		/// A function to set an attribute of the element.
		/**
			This function sets the attribute which has the value of the first supplied argument as
			a name and sets its' value to the value of the second supplied argument.
			If the attribute already exists, its' value is replaced by the second supplied argument,
			otherwise a new attribute is created.
			
			@param	aName	The name of the attribute for which to set a value.
			@param	aValue	The value to set the attribute to.
			
			@todo	@li Have to check if the name contains an invalid character.
						@li Have to check if this node is readonly.
		*/
		void setAttribute( const TDOMString aName, const TDOMString aValue );
		
		/// A function to remove an attribute from the element.
		/**
			This function removes the attribute of which the name is value of the supplied
			argument. If no attribute is found, nothing happens. If the attribute has a default
			value, a new attribute with the default value will be added.
			
			@param	aName	The name of the attribute to remove.
			
			@todo	@li Have to check if this element is readonly.
		*/
		void removeAttribute( const TDOMString aName );
		
		/// A function to get an attribute of the element.
		/**
			This function gets the attribute of this element which has the value of the
			supplied argument as a name and returns it. It will return a null attribute if
			the attribute does not exist or if it does not have a specified or default value.
			
			@param	aName	The name of the attribute to return the value of.
		*/
		TAttrPtr getAttributeNode( const TDOMString aName );
		
		/// A function to set an attribute of an element.
		/**
			This function sets the attribute which is supplied as the argument.
			If the attribute already exists, it is replaced by the supplied argument,
			otherwise a new attribute is created.

			@param	aNewAttr	The attribute to set for this element.
			
			@todo	@li Have to check if the attribute is from another document.
						@li Have to check if this element is readonly.
						@li Have to check if the attribute is in use by another element.
		*/
		TAttrPtr setAttributeNode( TAttrPtr aNewAttr );
		
		/// A function to remove an attribute from the element.
		/**
			This function removes the attribute which is supplied as the 	argument.
			If the attribute has a default value, a new attribute with the default value will be added.
			
			@param	aOldAttr	The name of the attribute to remove.
			
			@exception	NOT_FOUND_ERR	Thrown if the supplied attribute is not an attribute
															of this element.

			@todo	@li Have to check if this element is readonly.
		*/
		TAttrPtr removeAttributeNode( TAttrPtr aOldAttr );
		
		/// A function to get all elements with a certain name.
		/**
			This function retrieves a list of all descendant elements that have the name of
			the supplied argument. The special value '*' for the argument matches all tags.
			
			@param	aName	The name the element to search for has.
		*/
		TNodeListPtr getElementsByTagName( const TDOMString aName );
		
		/// A function to check if an element has an attribute with a certain name.
		/**
			This function checks if this element has an attribute with the name of the supplied
			argument.
			
			@param	aName	The name the attribute to search for has.
		*/
		bool hasAttribute( const TDOMString aName );
		
};

#endif
