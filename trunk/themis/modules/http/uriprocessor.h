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
\brief This file contains the declaration for the URIProcessor class.
*/
#ifndef _uri_processor_
#define _uri_processor_

#include <SupportDefs.h>
/*!
	\brief URI processing and comparison.
	
	This class does some BPath like processing of path locations, specifically for
	URLs. It includes a Contains function to help determine if the internal path
	is a parent of the passed it location. Eventually, this will likely include
	the unobfuscation of the URI portion of a URL that is currently a part of the
	ProtocolPlugClass base class.
*/
class URIProcessor
{
	private:
		const char *uri;
	public:
	URIProcessor(void);
	URIProcessor(const char *URI);
	~URIProcessor(void);
	status_t GetParent(URIProcessor *URI);
	bool Contains(const char *URI);
	/*!
		\brief Returns a const char * pointer to the URI.
		
		DO NOT DELETE THIS POINTER! Deleting it will cause harm to your sanity,
		Themis, and your loved ones.
	*/
	const char *String(void);
	/*!
		\brief Returns the length of the URI.
	*/
	int32 Length(void);
	const char *Set(const char *URI);
	void operator=(const char *URI);
	void operator=(URIProcessor &URI);
};

#endif

