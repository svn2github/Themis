#include "CSSParseException.h"
#include "Locator.h"

using namespace sac;
using namespace std;

CSSParseException::CSSParseException(const string& message, const Locator& locator)
:CSSException(SAC_SYNTAX_ERR, message),
 uri(locator.getURI()),
 lineNumber(locator.getLineNumber()),
 columnNumber(locator.getColumnNumber())
{
}

CSSParseException::CSSParseException(const string& message, const string& uri, int lineNumber, int columnNumber)
:CSSException(SAC_SYNTAX_ERR, message),
 uri(uri),
 lineNumber(lineNumber),
 columnNumber(columnNumber)
{
}

string CSSParseException::getURI() const
{
	return uri;
}

int CSSParseException::getLineNumber() const
{
	return lineNumber;
}

int CSSParseException::getColumnNumber() const
{
	return columnNumber;
}