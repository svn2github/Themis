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
#include "image.h"
#include <stdio.h>

Image::Image(Image *otherimage,bool truecopy) {
	if (otherimage!=NULL) {
		imagebmp=new BBitmap(otherimage->imagebmp->Bounds(),otherimage->imagebmp->ColorSpace(),false,false);
		memcpy(imagebmp->Bits(),otherimage->imagebmp->Bits(),otherimage->imagebmp->BitsLength());
		if (truecopy) {
			next=otherimage->next;
			prev=otherimage->prev;
		}
		
	} else
		imagebmp=NULL;
	url=NULL;
	next=prev=NULL;
}
Image::Image(BBitmap *bmp, char *URL) {
	printf("ImageHandler: Image %s\n",URL);
	if (URL!=NULL) {
		
		int32 len=strlen(URL);
		url=new char[len+1];
		memset(url,0,len+1);
		strcpy(url,URL);
	} else
		url=NULL;
	if (bmp!=NULL) {
		imagebmp=new BBitmap(bmp->Bounds(),bmp->ColorSpace(),false,false);
		memcpy(imagebmp->Bits(),bmp->Bits(),bmp->BitsLength());
	} else
		imagebmp=NULL;
	next=prev=NULL;
}
Image::~Image() {
	if (imagebmp!=NULL)
		delete imagebmp;
	imagebmp=NULL;
	if (url!=NULL)
		delete url;
	url=NULL;
	if (prev!=NULL)
		prev->next=next;
	if (next!=NULL)
		next->prev=prev;
	next=prev=NULL;
}
BBitmap* Image::Bitmap() {
	return imagebmp;
}
void Image::SetBitmap(BBitmap *bmp) {
	if (imagebmp!=NULL)
		delete imagebmp;
	imagebmp=new BBitmap(bmp->Bounds(),bmp->ColorSpace(),false,false);
	memcpy(imagebmp->Bits(),bmp->Bits(),bmp->BitsLength());
}
void Image::GetSize(BRect *size) {
	size->SetLeftTop(imagebmp->Bounds().LeftTop());
	size->SetRightBottom(imagebmp->Bounds().RightBottom());
}
Image* Image::Next() {
	return next;
}
Image* Image::Previous() {
	return prev;
}
Image* Image::SetNext(Image *nextimage) {
	Image *cur=NULL;
	if (next==NULL)
		cur=next=nextimage;
	else
		cur=next->SetNext(nextimage);
	return cur;
}
Image* Image::SetPrevious(Image *previmage) {
	prev=previmage;
	return prev;
}
char* Image::SetURL(char *URL) {
	if (url!=NULL) {
		memset(url,0,strlen(url)+1);
		delete url;
		url=NULL;
	}
	url=new char[strlen(URL)+1];
	memset(url,0,strlen(URL)+1);
	strcpy(url,URL);
	return url;
}
char* Image::URL() {
	return url;
}

