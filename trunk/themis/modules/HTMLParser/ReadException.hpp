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

class ReadException	{
	
	private:
		unsigned int mCharNr;
		unsigned int mLineNr;
		string mErrorMessage;
		bool mFatal;
		bool mEof;
		bool mWrongTag;
	
	public:
		ReadException( unsigned int aLineNr, unsigned int aCharNr,
							   string aErrorMessage, bool aFatal = false, bool aEof = false,
							   bool aWrongTag = false );
		~ReadException();
		unsigned int getCharNr() const;
		unsigned int getLineNr() const;
		string getErrorMessage() const;
		bool isFatal() const;
		bool isEof() const;
		bool isWrongTag() const;
		void setFatal( bool aFatal = true );
		
};

#endif
