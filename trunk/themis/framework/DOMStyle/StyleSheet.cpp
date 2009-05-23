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
	Class Implementation Start Date: March 14, 2004
*/

/*
	StyleSheet implementation.
	See StyleSheet.hpp for more information.
*/

// Standard C headers
#include <stdio.h>

// DOM Style headers
#include "StyleSheet.hpp"

StyleSheet	::	StyleSheet( const TDOMString aType, TNodePtr aOwnerNode,
									  StyleSheetPtr aParentStyleSheet, const TDOMString aHref,
									  const TDOMString aTitle, MediaListPtr aMedia )	{

	printf( "Creating StyleSheet\n" );
	
	mType = aType;
	mOwnerNode = aOwnerNode;
	mParentStyleSheet = aParentStyleSheet;
	mHref = aHref;
	mTitle = aTitle;
	mMedia = aMedia;

	// Not yet implemented
	mDisabled = false;

}

StyleSheet	::	~StyleSheet()	{
	
}

TDOMString StyleSheet	::	getType() const	{

	return mType;
}

bool StyleSheet	::	isDisabled() const	{

	return mDisabled;
	
}

void StyleSheet	::	setDisabled( bool aDisabled )	{

	mDisabled = aDisabled;

}

TNodePtr StyleSheet	::	getOwnerNode() const	{

	return make_shared( mOwnerNode );

}

StyleSheetPtr StyleSheet	::	getParentStyleSheet() const	{

	return make_shared( mParentStyleSheet );

}

TDOMString StyleSheet	::	getHref() const	{
	
	return mHref;
	
}

TDOMString StyleSheet	::	getTitle() const	{
	
	return mTitle;
	
}

MediaListPtr StyleSheet	::	getMedia() const	{
	
	return mMedia;
	
}
