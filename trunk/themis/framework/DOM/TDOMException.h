/*	DOMExcpetion implementation of the DOM core
	See the DOM core specification for more information
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	25-12-2001
*/

#ifndef TDOMEXCEPTION_H
#define TDOMEXCEPTION_H

#include "DOMSupport.h"

#include <String.h>

class TDOMException	{
	
	private:
		ExceptionCode code;
		BString string;
		
	public:
		TDOMException( ExceptionCode aCode );
		~TDOMException();
		ExceptionCode getCode();
		const char * getString();
		
};

#endif
