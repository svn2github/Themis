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
	Class Start Date: Februari 14, 2004
*/

#ifndef STYLESHEET_HPP
#define STYLESHEET_HPP

// DOM Style headers
#include "DOMStyleSupport.hpp"

// DOM headers
#include "TNode.h"

// DOM Style headers
#include "MediaList.hpp"

// Declarations used
class StyleSheet;

// Typedefs used
typedef boost::weak_ptr<StyleSheet> StyleSheetWeak;
typedef boost::shared_ptr<StyleSheet> StyleSheetPtr;

/// StyleSheet implementation of the DOM Style Sheets.

/**
	The StyleSheet represents an abstract style sheet.
	It represents a single style sheet associated with a document.
	For an extensive explanation, see the DOM Style Sheets pages at
	http://www.w3.org/TR/2000/REC-DOM-Level-2-Style-20001113/stylesheets.html
*/

class StyleSheet {

	private:
		/// The type of the stylesheet.
		TDOMString mType;
		/// The owner node of the stylesheet.
		TNodeWeak mOwnerNode;
		/// The parent stylesheet.
		StyleSheetWeak mParentStyleSheet;
		/// The href (location) of the stylesheet.
		TDOMString mHref;
		/// The title of the stylesheet.
		TDOMString mTitle;
		/// The media list of the stylesheet.
		MediaListPtr mMedia;
		/// The disabled status of the stylesheet.
		bool mDisabled;

	public:
		/// Constructor of the StyleSheet class.
		/**
			The constructor of the StyleSheet class.
			It initialized the read-only values.
			
			@param aType				The type of the stylesheet.
			@param aOwnerNode			The owner node of the stylesheet.
			@param aParentStyleSheet	The parent style sheet.
			@param aHref				The href (location) of the style sheet.
			@param aTitle				The title of the style sheet.
			@param aMedia				The media list of the style sheet.
		*/
		StyleSheet(const TDOMString aType,
				   TNodePtr aOwnerNode,
				   StyleSheetPtr aParentStyleSheet,
				   const TDOMString aHref,
				   const TDOMString aTitle,
				   MediaListPtr aMedia);
		
		/// Destructor of the StyleSheet class.
		/**
			The destructor of the StyleSheet class does nothing.
			Everything is cleaned up automatically.
		*/
		virtual ~StyleSheet();
		
		/// A function to get the type of the stylesheet.
		TDOMString getType() const;
		
		/// A function to check if the stylesheet is disabled or not.
		bool isDisabled() const;
		
		/// A function to en- or disable a stylesheet.
		/**
			This function en- or disables the stylesheet.
			Setting the parameter to true, disables the stylesheet.
			Setting it to false, enables it. 

			@param	aDisabled	The new disabled state of the stylesheet.
		*/
		void setDisabled(bool aDisabled);

		/// A function to get the owner of the stylesheet.
		/**
			This function gets the owner of the stylesheet.
			If this stylesheet is included by another stylesheet, the owner node
			is NULL.
		*/
		TNodePtr getOwnerNode() const;

		/// A function to get the parent stylesheet.
		/**
			This function gets the parent stylesheet.
			If this stylesheet is a top-level stylesheet or the stylesheet language
			does not support inclusion, the parent stylesheet is NULL.
		*/
		StyleSheetPtr getParentStyleSheet() const;

		/// A function to get the href of the stylesheet.
		/**
			This function gets the href of the stylesheet.
			The href is empty if it is an inline stylesheet, otherwise it points
			to the location of the stylesheet.
		*/
		TDOMString getHref() const;

		/// A function to get the title of the stylesheet.
		/**
			This function gets the title of the stylesheet.
			The title could be specified in the owner node.
		*/
		TDOMString getTitle() const;

		/// A function to get the media of the stylesheet.
		/**
			This function gets the media list of the stylesheet.
			The media list could be specified in the owner node.
			If no media have been specified, the media list is empty.
		*/
		MediaListPtr getMedia() const;

};

#endif
