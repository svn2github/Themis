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
	
	public:
		ReadException( unsigned int aLineNr, unsigned int aCharNr,
							   string aErrorMessage, bool aFatal = false, bool aEof = false );
		~ReadException();
		unsigned int getCharNr();
		unsigned int getLineNr();
		string getErrorMessage();
		bool isFatal();
		bool isEof();
		void setFatal( bool aFatal = true );
		
};

#endif
