/* DOMException implementation
	See TDOMException.h for some more information
*/

#include "TDOMException.h"

TDOMException	::	TDOMException( ExceptionCode aCode )	{
	
	code = aCode;
	switch ( aCode )	{
		case INDEX_SIZE_ERR:	{
			string.SetTo( "INDEX_SIZE_ERR" );
			break;
		}
		case DOMSTRING_SIZE_ERR:	{
			string.SetTo( "DOMSTRING_SIZE_ERR" );
			break;
		}
		case HIERARCHY_REQUEST_ERR:	{
			string.SetTo( "HIERARCHY_REQUEST_ERR" );
			break;
		}
		case WRONG_DOCUMENT_ERR:	{
			string.SetTo( "WRONG_DOCUMENT_ERR" );
			break;
		}
		case INVALID_CHARACTER_ERR:	{
			string.SetTo( "INVALID_CHARACTER_ERR" );
			break;
		}
		case NO_DATA_ALLOWED_ERR:	{
			string.SetTo( "NO_DATA_ALLOWED_ERR" );
			break;
		}
		case NO_MODIFICATION_ALLOWED_ERR:	{
			string.SetTo( "NO_MODIFICATION_ALLOWED_ERR" );
			break;
		}
		case NOT_FOUND_ERR:	{
			string.SetTo( "NOT_FOUND_ERR" );
			break;
		}
		case NOT_SUPPORTED_ERR:	{
			string.SetTo( "NOT_SUPPORTED_ERR" );
			break;
		}
		case INUSE_ATTRIBUTE_ERR:	{
			string.SetTo( "INUSE_ATTRIBUTE_ERR" );
			break;
		}
		case INVALID_STATE_ERR:	{
			string.SetTo( "INVALID_STATE_ERR" );
			break;
		}
		case SYNTAX_ERR:	{
			string.SetTo( "SYNTAX_ERR" );
			break;
		}
		case INVALID_MODIFICATION_ERR:	{
			string.SetTo( "INVALID_MODIFICATION_ERR" );
			break;
		}
		case NAMESPACE_ERR:	{
			string.SetTo( "NAMESPACE_ERR" );
			break;
		}
		case INVALID_ACCESS_ERR:	{
			string.SetTo( "INVALID_ACCESS_ERR" );
			break;
		}
		default:	{
			string.SetTo( "Undefined error" );
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
	
	return string.String();
	
}
