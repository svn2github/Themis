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
#ifndef _image_class_
#define _image_class_

#include <Bitmap.h>
#include <string.h>

class Image {
	private:
		BBitmap *imagebmp;
		char *url;
		Image *next,*prev;
	public:
		Image(Image *otherimage, bool truecopy=false);
		Image(BBitmap *bmp=NULL, char *URL=NULL);
		~Image();
		BBitmap *Bitmap();
		void SetBitmap(BBitmap *bmp);
		void GetSize(BRect *size);
		Image *Next();
		Image *Previous();
		Image *SetNext(Image *nextimage);
		Image *SetPrevious(Image *previmage);
		char *SetURL(char *URL);
		char *URL();
};

#endif
