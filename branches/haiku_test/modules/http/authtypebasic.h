/*
Copyright (c) 2004 Raymond "Z3R0 One" Rodgers. All Rights Reserved. 

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

Original Author & Project Manager: Raymond "Z3R0 One" Rodgers (z3r0_one@users.sourceforge.net)
Project Start Date: October 18, 2000
*/
/*!
	\file
	\brief Declaration for AuthTypeBasic class.


*/
#ifndef _auth_type_basic_
#define _auth_type_basic_

#include "authtype.h"

/*!
	\brief Basic Authentication class.

	This class handles the specifics of Basic authentication as described in RFCs 2068
	and 2617.
*/
class AuthTypeBasic:public AuthType
{
	public:
	AuthTypeBasic(void);
	AuthTypeBasic(const char *host,uint16 port, const char *uri,const char *user,const char *password,const char *realm);
	~AuthTypeBasic(void);
	const char *Credentials(void);
};

#endif
