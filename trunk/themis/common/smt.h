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

#ifndef SMT_H
#define SMT_H


#include <stdlib.h>
#include <string.h>
/*
	Supported MIME Type linked list structure.
*/
struct smt_st {
	char *type;
	smt_st *next;
	smt_st() {
		type=NULL;
		next=NULL;
	}
	~smt_st() {
		if (type!=NULL) {
			memset(type,0,strlen(type)+1);
			delete type;
			type=NULL;
		}
		next=NULL;
	}
};

#endif

