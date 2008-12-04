/*
	Copyright (c) 2003 Mark Hellegers. All Rights Reserved.
	
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
	
	Original Author: 	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	Project Start Date: October 18, 2000
	Class Start Date: April 22, 2003
*/

#ifndef ELEMENTDECLEXCEPTION_HPP
#define ELEMENTDECLEXCEPTION_HPP

/// Indicates the requested element declaration was not found.

/**
	The ElementDeclException is used when a requested element declaration is not found.
	An exception with an instance of this class is then thrown.

*/

class ElementDeclException	{
	
	public:
		/// Constructor of the ElementDeclException class.
		/**
			The constructor of the ElementDeclException class does nothing.
			It only exists to allow an exception of this class to be thrown.
		*/
		ElementDeclException();
		/// Destructor of the ElementDeclException class.
		/**
			The destructor of the ElementDeclException class does nothing.
			Everything is cleaned up	automatically.
		*/
		~ElementDeclException();
		
};

#endif
