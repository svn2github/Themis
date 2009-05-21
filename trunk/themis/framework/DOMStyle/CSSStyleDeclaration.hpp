/*
	Copyright (c) 2004 Mark Hellegers. All Rights Reserved.
	
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
	Class Start Date: Februari 14, 2004
*/

#ifndef CSSSTYLEDECLARATION_HPP
#define CSSSTYLEDECLARATION_HPP

// DOM Style headers
#include "DOMStyleSupport.hpp"
#include "Property.hpp"

// Standard C++ headers
#include <vector>

// Declarations
class CSSValue;
class CSSRule;
class CSSStyleRule;
class CSSStyleDeclaration;

// Typedefs
typedef boost::shared_ptr<CSSValue> CSSValuePtr;
typedef boost::shared_ptr<CSSRule> CSSRulePtr;
typedef boost::shared_ptr<CSSStyleDeclaration> CSSStyleDeclarationPtr;

// Namespaces used
using namespace std;

/// CSSStyleDeclaration implementation of the DOM CSS.

/**
	The CSSStyleDeclaration represents a CSS declaration block.
	It provides access to all the properties in the declaration block.
	For an extensive explanation, see the DOM CSS pages at
	http://www.w3.org/TR/2000/REC-DOM-Level-2-Style-20001113/css.html
	
*/

class CSSStyleDeclaration	{

	private:
		TDOMString mCssText;
		CSSRulePtr mParentRule;
		vector<Property> mProperties;

	public:
		/// Constructor of the CSSStyleDeclaration class.
		/**
			The constructor of the CSSStyleDeclaration class.
			
			@param aParentRule	The parent rule of the declaration.
		*/
		CSSStyleDeclaration( CSSRulePtr aParentRule = CSSRulePtr() );
		
		/// Destructor of the CSSStyleDeclaration class.
		/**
			The destructor of the CSSStyleDeclaration class does nothing.
			Everything is cleaned up automatically.
		*/
		virtual ~CSSStyleDeclaration();
		
		/// A function to get the raw text of the declaration.
		/**
			This function gets the raw text of the declaration.
			This is all the text, without the surrounding curly braces.
		*/
		TDOMString getCssText();
		
		/// A function to set the raw text of the declaration.
		/**
			This function sets the raw text of the declaration to the value supplied
			by the parameter.
			The text is parsed and all properties are set to the values specified in the
			text.
			
			@param aText	The text to set the declaration block to.
			
			@exception SYNTAX_ERR	Thrown if the declaration is unparseble.
			@exception	NO_MODIFICATION_ALLOWED_ERR
								Thrown if the declaration or a property is readonly.

		*/
		void setCssText( const TDOMString aText );
		
		/// A function to get a property value.
		/**
			This function retrieves the property which has the value of the parameter
			as the name.
			If the property has not been set, an empty string is returned.
			
			@param aName		The name of the property to get the value of.
		*/
		TDOMString getPropertyValue( const TDOMString aName );
		
		/// A function to get a CSS property value.
		/**
			This function retrieves the CSS property which has the value of the parameter
			as the name.
			If the property has not been set or if it is a shorthand value,
			an empty string is returned.

			@param aName		The name of the property to get the value of.
		*/
		CSSValuePtr getPropertyCSSValue( const TDOMString aName );
		
		/// A function to remove a property.
		/**
			This function removes the property which has the value of the parameter
			as the name and returns the value.
			If the property has not been set, an empty string is returned.
			
			@param aName		The name of the property to remove.

			@exception	NO_MODIFICATION_ALLOWED_ERR
								Thrown if the declaration or a property is readonly.
		*/
		TDOMString removeProperty( const TDOMString aName );
		
		/// A function to get the priority of the property.
		/**
			This function retrieves the priority of the property if it has been set.
			If it has not been set, an empty string will be returned.
			
			@param aName		The name of the property to get the priority of.
		*/
		TDOMString getPropertyPriority( const TDOMString aName );
		
		/// A function to set a property.
		/**
			This function sets the value and priority of a property.
			
			@param aName		The name of the property to set.
			@param aValue		The value to set the property to.
			@param aPriority	The priority to set the property to.

			@exception	SYNTAX_ERR	Thrown if the value is unparsable.
			@exception	NO_MODIFICATION_ALLOWED_ERR
								Thrown if the declaration or a property is readonly.
		*/
		void setProperty( const TDOMString aName,
								  const TDOMString aValue,
								  const TDOMString aPriority );
								  
		/// A function to get the number of properties in the declaration block.
		unsigned long getLength();
		
		/// A function to get an item.
		/**
			This function gets the item that is indexed at the value of the supplied argument.
			If the index is greater than the number of items in the map,
			an empty string is returned.

			@param	aIndex	The index at which to get the item.
		*/
		TDOMString item( unsigned long aIndex );
		
		/// A function to get the parent rule.
		/**
			This function gets the parent rule if the declaration is contained in another rule.
			If it is not contained in another rule, it returns a null item.
		*/		
		CSSRulePtr getParentRule();

};

#endif
