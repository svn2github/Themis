/*
	Copyright (c) 2009 Mark Hellegers. All Rights Reserved.
	
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
	Class Start Date: November 22, 2009
*/

/*	TTextView implementation
	See TTextView.h for some more information
*/

// Standard C headers
#include <stdio.h>

// Themis headers
#include "TTextView.hpp"

TTextView :: TTextView(BRect aFrame,
					   const char * aName,
					   BRect aTextRect,
					   uint32 aResizeMask,
					   uint32 aFlags)
		  : BTextView(aFrame, aName, aTextRect, aResizeMask, aFlags) {

}
				
TTextView :: ~TTextView() {

}

void TTextView :: FrameResized(float aWidth, float aHeight) {
	
	// See Be Newsletters: Issue 3-17, April 28, 1998
	BTextView::FrameResized(aWidth, aHeight);
	
	if (DoesWordWrap()) {
		// Get the new bounds of the view.
		BRect textRect = Bounds();
		
		// Make sure we are in the views coordinate system.
		textRect.OffsetTo(B_ORIGIN);

		// Inset the text rect by 3 pixels so that the text doesn't
	    // touch the frame of the view: this is the text rect
	    textRect.InsetBy(3.0, 3.0);

	    // tell TextView to reflow itself
    	SetTextRect(textRect);
    }
	
}
