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
	Class Start Date: March 25, 2003
*/

#ifndef READEXCEPTION_HPP
#define READEXCEPTION_HPP

// Standard C++ headers
#include <string>

// Namespaces used
using namespace std;

/// The reason for the exception

/**
	This enum indicates the reason for the exception.
*/

enum ExceptionReason	{
	/// A generic error occured.
	GENERIC,
	/// The end of the file was reached.
	END_OF_FILE_REACHED,
	/// This tag wasn't the one we wanted.
	WRONG_TAG_FOUND,
	/// An end tag was found, but not wanted.
	END_TAG_FOUND,
	/// A tag was wanted, but not found.
	NO_TAG_FOUND,
	/// PCDATA was wanted, but not found,
	NO_PCDATA_FOUND,
	/// Content had to be skipped, but was not possible.
	NO_SKIP_CONTENT
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
		/// Indicates if the error was fatal or not.
		bool mFatal;
		/// The name of the tag, if it was the wrong one.
		string mWrongTag;
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
			@param aFatal		Indicates if the error is fatal or not.

		*/
		ReadException( unsigned int aLineNr, unsigned int aCharNr,
							   string aErrorMessage, ExceptionReason aReason = GENERIC,
							   bool aFatal = false );
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
		/// A function to set the reason for the exception.
		/**
			This function allows you to set the reason for the exception.
			This can be handy when you want to rethrow an exception
			with a different reason.
			
			@param	aReason	The reason for the exception.
		*/
		void setReason( ExceptionReason aReason );
		/// A function to see if the error was fatal.
		bool isFatal() const;
		/// A function to see if the end of the file was reached.
		/**
			@todo	Might be unneeded as you can already
						get that from the exception reason.
		*/
		bool isEof() const;
		/// A function to see if the wrong tag was parsed.
		/**
			@todo	Might be unneeded as you can already
						get that from the exception reason.
		*/
		bool isWrongTag() const;
		/// A function to see if an end tag was parsed.
		/**
			@todo	Might be unneeded as you can already
						get that from the exception reason.
		*/
		bool isEndTag() const;
		/// A function to set the wrong tag that was found.
		/**
			This function allows you to set the wrong tag that
			was found while parsing.
			
			@param	aWrongTag	The wrong tag name.
		*/
		void setWrongTag( string aWrongTag );
		/// A function to get the wrong tag that was found.
		string getWrongTag() const;
		/// A function to set how fatal the error was.
		/**
			This function allows you to set if the error
			that was found was fatal or not.
			
			@param	aFatal	The fatal value of the error.
		*/
		void setFatal( bool aFatal = true );
		
};

#endif
