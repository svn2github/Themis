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
	
	Original Author: 	Mark Hellegers (mark@firedisk.net)
	Project Start Date: October 18, 2000
	Class Start Date: March 25, 2003
*/

#ifndef READEXCEPTION_HPP
#define READEXCEPTION_HPP

// Standard C++ headers
#include <string>

// Namespaces used
using std::string;

/// The reason for the exception

/**
	This enum indicates the reason for the exception.
*/

enum ExceptionReason	{
	/// A generic error occured.
	GENERIC,
};

/// Indicates a read error.

/**
	The ReadException class is used when a read error occurs.
	An exception with an instance of this class is then thrown.
*/

class ReadException	{
	
	private:
		/// Indicates the character number the error occured on.
		unsigned int mCharNr;
		/// Indicates the line number the error occured on.
		unsigned int mLineNr;
		/// The error message.
		string mErrorMessage;
		/// The reason the exception occured.
		ExceptionReason mReason;
	
	public:
		/// Constructor of the ReadException class.
		/**
			The constructor of the ReadException class assigns the
			parameters to private members.
			
			@param aLineNr	Indicates the line number the error occured on.
			@param aCharNr	Indicates the character number the error occured on.
			@param aErrorMessage	The error message.
			@param aReason	The reason for the exception.

		*/
		ReadException(unsigned int aLineNr,
					  unsigned int aCharNr,
					  string aErrorMessage,
					  ExceptionReason aReason = GENERIC);
		/// Destructor of the ReadException class.
		/**
			The destructor of the ReadException class does nothing.
			Everything is cleaned up	automatically.
		*/
		~ReadException();
		/// A function to get the character number.
		unsigned int getCharNr() const;
		/// A function to get the line number.
		unsigned int getLineNr() const;
		/// A function to get the error message.
		string getErrorMessage() const;
		/// A function to get the reason for the exception.
		ExceptionReason getReason() const;
};

#endif
