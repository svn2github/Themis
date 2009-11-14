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

/*
	DOMException implementation
	See TDOMException.h for some more information
*/

// Standard C++ headers
#include <string>

// DOM headers
#include "TDOMException.h"

// Namespaces used
using namespace std;

TDOMException	::	TDOMException( ExceptionCode aCode )	{
	
	code = aCode;
	switch ( aCode )	{
		case INDEX_SIZE_ERR:	{
			mString =  "INDEX_SIZE_ERR" ;
			break;
		}
		case DOMSTRING_SIZE_ERR:	{
			mString = "DOMSTRING_SIZE_ERR";
			break;
		}
		case HIERARCHY_REQUEST_ERR:	{
			mString = "HIERARCHY_REQUEST_ERR";
			break;
		}
		case WRONG_DOCUMENT_ERR:	{
			mString = "WRONG_DOCUMENT_ERR";
			break;
		}
		case INVALID_CHARACTER_ERR:	{
			mString = "INVALID_CHARACTER_ERR";
			break;
		}
		case NO_DATA_ALLOWED_ERR:	{
			mString = "NO_DATA_ALLOWED_ERR";
			break;
		}
		case NO_MODIFICATION_ALLOWED_ERR:	{
			mString = "NO_MODIFICATION_ALLOWED_ERR";
			break;
		}
		case NOT_FOUND_ERR:	{
			mString = "NOT_FOUND_ERR";
			break;
		}
		case NOT_SUPPORTED_ERR:	{
			mString = "NOT_SUPPORTED_ERR";
			break;
		}
		case INUSE_ATTRIBUTE_ERR:	{
			mString = "INUSE_ATTRIBUTE_ERR";
			break;
		}
		case INVALID_STATE_ERR:	{
			mString = "INVALID_STATE_ERR";
			break;
		}
		case SYNTAX_ERR:	{
			mString = "SYNTAX_ERR";
			break;
		}
		case INVALID_MODIFICATION_ERR:	{
			mString = "INVALID_MODIFICATION_ERR";
			break;
		}
		case NAMESPACE_ERR:	{
			mString = "NAMESPACE_ERR";
			break;
		}
		case INVALID_ACCESS_ERR:	{
			mString = "INVALID_ACCESS_ERR";
			break;
		}
		case TYPE_MISMATCH_ERR:	{
			mString = "TYPE_MISMATCH_ERR";
			break;
		}
		default:	{
			mString = "Undefined error";
			break;
		}
	}

}

TDOMException	::	~TDOMException()	{
	
}

ExceptionCode TDOMException	::	getCode()	{
	
	return code;
	
}

const char * TDOMException	::	getString()	{
	
	return mString.c_str();
	
}
