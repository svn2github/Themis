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
	
	Original Author: 	Mark Hellegers (mark@firedisk.net)
	Project Start Date: October 18, 2000
	Class Start Date: March 18, 2004
*/

#ifndef PROPERTY_HPP
#define PROPERTY_HPP

// DOM Style headers
#include "DOMStyleSupport.hpp"

class Property	{

	private:
		/// The name of the property.
		TDOMString mName;
		/// The value of the property.
		TDOMString mValue;
		/// The priority of the property.
		TDOMString mPriority;

	public:
		/// Constructor of the Property class.
		/**
			The constructor of the Property class. It stores
			the name, value and priority of a property.

			@param aName		The name of the property.
			@param aValue		The value of the property.
			@param aPriority	The priority of the property.
		*/
		Property( TDOMString aName, TDOMString aValue,
					   TDOMString aPriority );

		/// Destructor of the Property class.
		/**
			The destructor of the Property class does nothing. Everything is cleaned up
			automatically.
		*/
		~Property();
		
		/// A function to get the name of the property.
		TDOMString getName() const;

		/// A function to get the value of the property.
		TDOMString getValue() const;

		/// A function to get the priority of the property.
		TDOMString getPriority() const;

		/// A function to set the value of the property.
		/**
			This function sets the value of the property.
			
			@param	aValue	The value to set.
		*/
		void setValue( TDOMString aValue );

		/// A function to set the priority of the property.
		/**
			This function sets the priority of the property.
			
			@param	aPriority	The priority to set.
		*/
		void setPriority( TDOMString aPriority );
		
};

#endif
