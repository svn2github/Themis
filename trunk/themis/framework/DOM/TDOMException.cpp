/* DOMException implementation
	See TDOMException.h for some more information
*/

#include <string>

#include "TDOMException.h"

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
