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
	Class Start Date: November 17, 2009
*/

/*	DOMTextView
	Derived from BTextView in order to reflow text.

	Mark Hellegers (mark@firedisk.net)
	17-11-2009
*/

#ifndef MSGTEXTVIEW_HPP
#define MSGTEXTVIEW_HPP

// BeOS headers
#include <TextView.h>

class MsgTextView : public BTextView {
	
	public:
		MsgTextView(BRect aFrame,
				    const char * aName,
				  	BRect aTextRect,
				  	uint32 aResizeMask,
				  	uint32 aFlags = B_WILL_DRAW | B_FRAME_EVENTS);
		~MsgTextView();
		void FrameResized(float aWidth, float aHeight);
};

#endif
