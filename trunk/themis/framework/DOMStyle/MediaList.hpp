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
	Class Start Date: Februari 13, 2004
*/

#ifndef MEDIALIST_HPP
#define MEDIALIST_HPP

// Boost headers
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

// Declarations used
class MediaList;

// Typedefs used
typedef boost::weak_ptr<MediaList> MediaListWeak;
typedef boost::shared_ptr<MediaList> MediaListPtr;

/// MediaList implementation of the DOM Style Sheets.

/**
	The MediaList represents an ordered collection of media.
	An empty list means that the medium is defined as "all".
	For an extensive explanation, see the DOM Style Sheets at
	http://www.w3.org/TR/2000/REC-DOM-Level-2-Style-20001113/stylesheets.html
*/

class MediaList	{

	public:
		/// Constructor of the MediaList class.
		/**
			The constructor of the MediaList class.
			It does nothing as there is nothing to initialize.
		*/
		MediaList();
		
		/// Destructor of the MediaList class.
		/**
			The destructor of the MediaList class does nothing.
			Everything is cleaned up automatically.
		*/
		virtual ~MediaList();
		
		/// A function to get the parsable textual representation of the media list.
		TDOMString getMediaText() const;
		
		/// A function to set the textual representation of the media list.
		/**
			This function sets the parsable textual representation of the media list
			to the value supplied in the parameter.
			
			@param	aText	The new parsable textual representation.
			
			@exception	SYNTAX_ERR	Thrown if the new value is unparsable.
			@exception	NO_MODIFICATION_ALLOWED_ERR
								Thrown if the media list is readonly.
		*/
		void setMediaText( const TDOMString aText );
		
		/// A function to get the number of media in the list.
		unsigned long getLength() const;

		/// A function to get an item.
		/**
			This function gets the item that is indexed at the value
			of the supplied argument.
			If the index is greater than or equal to the number of items
			in the map, a null item is returned.

			@param	aIndex	The index at which to get the item.
		*/
		TDOMString item( unsigned long aIndex );

		/// A function to delete a medium from the list.
		/**
			This function deletes the medium indicated in the parameter
			from the list.
			
			@param	aOldMedium	The medium to remove from the list.
			
			@exception	NOT_FOUND_ERR	Thrown if the medium can't be found
															in the list.
			@exception	NO_MODIFICATION_ALLOWED_ERR
								Thrown if the media list is readonly.
		*/
		void deleteMedium( const TDOMString aOldMedium );

		/// A function to append a medium from the list.
		/**
			This function appends the medium indicated in the parameter
			to the end of the list.
			If the medium is already used, it is first removed.
			
			@param	aNewMedium	The medium to append to the list.
			
			@exception	INVALID_CHARACTER_ERR	Thrown if the medium contains
																		characters that are not allowed
																		in the underlying style language.
			@exception	NO_MODIFICATION_ALLOWED_ERR
								Thrown if the media list is readonly.
		*/
		void appendMedium( const TDOMString aNewMedium );

};

#endif
