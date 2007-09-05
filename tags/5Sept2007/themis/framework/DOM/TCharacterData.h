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
	Class Start Date: February 10, 2002
*/

#ifndef TCHARACTERDATA_H
#define TCHARACTERDATA_H

// DOM headers
#include "DOMSupport.h"
#include "TNode.h"

/// CharacterData implementation of the DOM core

/*
	The CharacterData class is used to store character data in the DOM tree.
	It has several extra attributes and functions to deal with character data.
	For an extensive explanation, see the DOM core pages at http://www.w3.org/DOM
*/

class TCharacterData	:	public TNode	{
	
	private:
		/// The character data that is stored here.
		TDOMString mData;
		
		// Not implemented variable. Implemented using function of mData
		// int length;
		// ---------------------------------------------------------------------------
		
	public:
		/// Constructor of the TCharacterData class.
		/**
			The constructor of the TCharacterData class. It passes all its' arguments on and also stores
			the last supplied argument locally.
			
			@param	aNodeType	The node type of the TCharacterData class.
			@param	aOwnerDocument	The owner document of this character data node.
			@param	aData	The character data to be stored.
		*/
		TCharacterData( const unsigned short aNodeType,
								 const TDocumentWeak aOwnerDocument,
								 const TDOMString aData = "" );
		
		/// Destructor of the TCharacterData class.
		/**
			The destructor of the TCharacterData class does nothing. Everything is cleaned up
			automatically.
		*/
		~TCharacterData();
		
		/// A function to get the character data.
		TDOMString getData() const;
		
		/// A function to set the character data.
		/**
			This function sets the character data of this character data node to the value of
			the supplied argument.
			
			@param	aData	The data to set the character data to.
			
			@todo	@li Have to check if this element is readonly.
		*/
		void setData( const TDOMString aData );
		
		/// A function to get the length of the character data.
		int getLength() const;
		
		/// A function to get a substring of the character data.
		/**
			This function returns a substring of the character data by using the first supplied argument
			as an offset and the second supplied argument as the number of characters to retrieve.
			
			@param	aOffset	The offset at which to start the substring.
			@param	aCount The number of characters to retrieve.
			
			@exception	INDEX_SIZE_ERR	Thrown if the specified offset is greater than the number of characters
								in this character data node.
		*/
		TDOMString substringData( const unsigned long aOffset, const unsigned long aCount ) const;
		
		/// A function to append character data.
		/**
			This function appends the character data of the first supplied argument to the character data
			in the character data node.
			
			@param	aArg	The character data to append.
			
			@todo	@li Have to check if this element is readonly.
		*/
		void appendData( const TDOMString aArg );
		
		/// A function to insert character data.
		/**
			This function inserts character data by using the first supplied argument as an offset and
			the second supplied argument as the character data to insert.

			@param	aOffset	The offset at which to insert the character data.
			@param	aArg	The character data to insert.

			@exception	INDEX_SIZE_ERR	Thrown if the specified offset is greater than the number of characters
								in this character data node.

			@todo	@li Have to check if this element is readonly.
		*/
		void insertData( const unsigned long aOffset, const TDOMString aArg );
		
		/// A function to delete character data.
		/**
			This function deletes a part of the character data by using the first supplied argument
			as an offset and the second supplied argument as the number of characters to delete.

			@param	aOffset	The offset at which to start deleting.
			@param	aCount The number of characters to delete.
			
			@exception	INDEX_SIZE_ERR	Thrown if the specified offset is greater than the number of characters
								in this character data node.

			@todo	@li Have to check if this element is readonly.
		*/
		void deleteData( const unsigned long aOffset, const unsigned long aCount );
		
		/// A function to replace character data.

		/**
			This function replaces character data by using the first supplied argument as an offset and
			the second supplied argument as the number of character data to replace and the last supplied
			argument as the character data to replace this character data node's character data with.
			If the number of character data to replace is bigger than what is available, all character data
			starting from the offset will be replaced.

			@param	aOffset	The offset at which to replace the character data.
			@param	aCount The number of characters to replace.
			@param	aArg	The character data to replace the old character data with.

			@exception	INDEX_SIZE_ERR	Thrown if the specified offset is greater than the number of characters
								in this character data node.

			@todo	@li Have to check if this element is readonly.
		*/
		void replaceData( const unsigned long aOffset, const unsigned long aCount, const TDOMString aArg );
		
};

#endif
