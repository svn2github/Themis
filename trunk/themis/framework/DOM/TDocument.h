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

#ifndef TDOCUMENT_H
#define TDOCUMENT_H

// DOM headers
#include "DOMSupport.h"
#include "TNode.h"

/// Document implementation of the DOM core

/*
	The Document class is used as the base of every DOM tree.
	All other nodes of the tree must be created through its' functions.
	For an extensive explanation, see the DOM core pages at http://www.w3.org/DOM
*/

class TDocument	:	public TNode	{
	
	private:
		// DocumentType doctype // Not yet implemented
		// DOMImplementation implementation // Not yet implemented
		// Element documentElement // Not yet implemented
		TDOMString mDocumentURI;

	public:
		/// Constructor of the TDocument class.
		/**
			The constructor of the TDocument class. It doesn't take any arguments,
			but sets the TNode arguments directly.
			
			@todo	Have to look at the TDocumentPtr being passed. I don't think it
						is correct.
		*/
		TDocument();
		
		/// Destructor of the TDocument class.
		/**
			The destructor of the TDocument class does nothing. Everything is cleaned up
			automatically.
		*/
		~TDocument();
		
		/// A function to create an element.
		/**
			This function creates an element with the value of the supplied argument
			as the tag name.
			
			@param aTagName	The tag name for the element.

			@todo	@li Have to check if the name contains an invalid character.
		*/
		TElementPtr createElement( const TDOMString aTagName );
		//DocumentFragment createDocumentFragment(); // Not yet implemented

		/// A function to create a text node.
		/**
			This function creates a text node with the value of the supplied argument
			as the text.
			
			@param aData	The text for the text node.
		*/
		TTextPtr createText( const TDOMString aData );
		
		/// A function to create a comment.
		/**
			This function creates a comment with the value of the supplied argument
			as the text.
			
			@param aData	The text for the comment.
		*/
		TCommentPtr createComment( const TDOMString aData );
		//CDataSection createCDataSection( const TDOMString aData ); // Not yet implemented
		//ProcessingInstruction createProcessingInstruction( const TDOMString aTarget, const TDOMString aData ); // Not yet implemented
		
		/// A function to create an attribute.
		/**
			This function creates an attribute with the value of the supplied argument
			as the name of the attribute.
			
			@param aName	The name of the attribute.

			@todo	@li Have to check if the name contains an invalid character.
		*/
		TAttrPtr createAttribute( const TDOMString aName );
		//EntityReference createEntityReference( const TDOMString aName ); // Not yet implemented
		//TNodeListShared getElementsByTagName( const TDOMString aTagName ); // Not yet implemented
		
		/// A function to set the document URI
		/**
			This function sets the document URI. No checking is done to see if it is
			a valid URI. It could be empty if the document was created with
			a call to the constructor.
			
			@param aURI		The URI of the document.
		*/
		void setDocumentURI( const TDOMString aURI );
		
		/// A function to get the URI. It might be empty.
		TDOMString getDocumentURI() const;

};

#endif
