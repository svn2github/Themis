#include "CSSException.h"

using namespace std;
using namespace sac;

const string CSSException::S_SAC_UNSPECIFIED_ERR = "unknown error";
const string CSSException::S_SAC_NOT_SUPPORTED_ERR	= "not supported";
const string CSSException::S_SAC_SYNTAX_ERR = "syntax error";

CSSException::CSSException()
{
}

CSSException::CSSException(const string& s)
:s(s),
 code(SAC_UNSPECIFIED_ERR)
{
}

CSSException::CSSException(int code)
:code(code)
{
}

CSSException::CSSException(int code, const string& s)
:s(s),
 code(code)
{
}

CSSException::~CSSException()
{
}

string CSSException::getMessage() const
{
	if(s.length() != 0)
		return s;
	else
	{
		switch(code)
		{
			case SAC_UNSPECIFIED_ERR:
				return S_SAC_UNSPECIFIED_ERR;
		    case SAC_NOT_SUPPORTED_ERR:
				return S_SAC_NOT_SUPPORTED_ERR;
		    case SAC_SYNTAX_ERR:
				return S_SAC_SYNTAX_ERR;			
		}
	}
	return string("");
}

short CSSException::getCode() const
{
	return code;
}