/*
	Copyright (c) 2001 Mark Hellegers. All Rights Reserved.
	
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
	Class Start Date: December 25, 2001
*/

#ifndef TDOMEXCEPTION_H
#define TDOMEXCEPTION_H

// DOM headers
#include "DOMSupport.h"

/// DOMException implementation of the DOM core.

/**
	The DOMException class implements the general exception class
	of the DOM core.
	For an extensive explanation, see the DOM core pages at http://www.w3.org/DOM
*/

class TDOMException	{
	
	private:
		/// The exception code.
		ExceptionCode code;
		
		/// The exception code stored as a string
		TDOMString mString;
		
	public:
		/// Constructor of the TDOMException class.
		/**
			The constructor of the TDOMException class. It stores the argument as
			the exception code and stores it as a string as well.
			
			@param	aCode	The exception code.
		*/
		TDOMException( ExceptionCode aCode );
		/// Destructor of the TDOMException class.
		/**
			The destructor of the TDOMException class does nothing.
			Everything is cleaned up automatically.
		*/
		~TDOMException();
		
		/// A function to get the exception code.
		ExceptionCode getCode();

		/// A function to get the exception code string.
		const char * getString();
		
};

#endif
