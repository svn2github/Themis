/*	ReadException
	An exception that contains the details of where it occured and why.
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	25-03-2003
	
*/

#ifndef READEXCEPTION_HPP
#define READEXCEPTION_HPP

// Standard C++ headers
#include <string>

// Namespaces used
using namespace std;

enum ExceptionReason	{
	GENERIC,
	END_OF_FILE_REACHED,
	WRONG_TAG_FOUND,
	END_TAG_FOUND,
	NO_TAG_FOUND
};

class ReadException	{
	
	private:
		unsigned int mCharNr;
		unsigned int mLineNr;
		string mErrorMessage;
		bool mFatal;
		string mWrongTag;
		ExceptionReason mReason;
	
	public:
		ReadException( unsigned int aLineNr, unsigned int aCharNr,
							   string aErrorMessage, ExceptionReason aReason = GENERIC,
							   bool aFatal = false );
		~ReadException();
		unsigned int getCharNr() const;
		unsigned int getLineNr() const;
		string getErrorMessage() const;
		ExceptionReason getReason() const;
		void setReason( ExceptionReason aReason );
		bool isFatal() const;
		bool isEof() const;
		bool isWrongTag() const;
		bool isEndTag() const;
		void setWrongTag( string aWrongTag );
		string getWrongTag() const;
		void setFatal( bool aFatal = true );
		
};

#endif
