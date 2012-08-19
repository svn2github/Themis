/*
Copyright (c) 2002 Raymond "Z3R0 One" Rodgers. All Rights Reserved. 

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

Original Author & Project Manager: Raymond "Z3R0 One" Rodgers (z3r0_one@yahoo.com)
Project Start Date: October 18, 2000
*/
#ifndef _themis_about_view_
#define _themis_about_view_

// BeOS headers
#include <View.h>

// Declarations of BeOS classes
class BBox;
class BListView;
class BButton;
class BScrollView;
class BStringItem;

class aboutview : public BView {

	private:
		BButton *OkB;
		BScrollView *scroll;
		BListView *mListView;
		BBox *outerbox;
		BBox *innerbox;

		BView * CreateSimpleAboutView(
			BRect aRect,
			const char * aName,
			const char * aText);

	public:
		aboutview(BRect frame, const char *name, uint32 resizem, uint32 flags);
		~aboutview();
		void MessageReceived(BMessage *msg);
		void AttachedToWindow();
};

#endif
